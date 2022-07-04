#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QThread>

using namespace cv;

QVector<Machine>machines;
QMap<int, QTreeWidgetItem *>machine_items;
QMap<Camera, QTreeWidgetItem *>camera_items;

QMap<int, QTreeWidgetItem *>::iterator it_m;
QMap<Camera, QTreeWidgetItem *>::iterator it_c;

Machine cur_m = NULL;

QSqlDatabase db;

QMutex mutex1;

// Draw the predicted bounding box.
void CThread::draw_label(Mat& input_image, string label, int left, int top)
{
    // Display the label at the top of the bounding box.
    int baseLine;
    Size label_size = getTextSize(label, FONT_FACE, FONT_SCALE, THICKNESS, &baseLine);
    top = max(top, label_size.height);
    // Top left corner.
    Point tlc = Point(left, top);
    // Bottom right corner.
    Point brc = Point(left + label_size.width, top + label_size.height + baseLine);
    // Draw black rectangle.
    rectangle(input_image, tlc, brc, BLACK, FILLED);
    // Put the label on the black rectangle.
    putText(input_image, label, Point(left, top + label_size.height), FONT_FACE, FONT_SCALE, YELLOW, THICKNESS);
}


vector<Mat> CThread::pre_process(Mat &input_image, Net &net)
{
    Mat blob;
    blobFromImage(input_image, blob, 1. / 255., Size(INPUT_WIDTH, INPUT_HEIGHT),
                  Scalar(), true, false);
    net.setInput(blob);
    // Forward propagate.
    vector<Mat> outputs;
    net.forward(outputs);

    return outputs;
}


Mat CThread::post_process(Mat input_image, vector<Mat> &outputs, const vector<string> &class_name)
{
    // Initialize vectors to hold respective outputs while unwrapping detections.
    vector<int> class_ids;
    vector<float> confidences;
    vector<Rect> boxes;

    // Resizing factor.
    float x_factor = input_image.cols / INPUT_WIDTH;
    float y_factor = input_image.rows / INPUT_HEIGHT;
//    for (int i=0;i<10;i++){
//        qDebug()<<outputs[0].data[i]<<" ";
//    }
    float *data = (float *)outputs[0].data;

    const int rows = 25200;
    // Iterate through 25200 detections.
    for (int i = 0; i < rows; ++i)
    {
        float confidence = data[4];

        // Discard bad detections and continue.
        if (confidence >= CONFIDENCE_THRESHOLD)
        {
//            qDebug()<<confidence<<"\n";
            float * classes_scores = data + 5;
            // Create a 1x85 Mat and store class scores of 80 classes.
            Mat scores(1, class_name.size(), CV_32FC1, classes_scores);
            // Perform minMaxLoc and acquire index of best class score.
            Point class_id;
            double max_class_score;
            minMaxLoc(scores, 0, &max_class_score, 0, &class_id);
            // Continue if the class score is above the threshold.
            if (max_class_score > SCORE_THRESHOLD)
            {
                // Store class ID and confidence in the pre-defined respective vectors.

                confidences.push_back(confidence);
                class_ids.push_back(class_id.x);

                // Center.
                float cx = data[0];
                float cy = data[1];
                // Box dimension.
                float w = data[2];
                float h = data[3];
//                qDebug()<<"cx: "<<cx<<"cy: "<<cy<<"w: "<<w<<"h "<<h<<"conf "<<data[4]<<" " << data[5] << " " << data[6] << " " << data[7] << "\n";
                // Bounding box coordinates.
                int left = int((cx - 0.5 * w) * x_factor);
                int top = int((cy - 0.5 * h) * y_factor);
                int width = int(w * x_factor);
                int height = int(h * y_factor);
                // Store good detections in the boxes vector.
                boxes.push_back(Rect(left, top, width, height));
            }

        }
        // Jump to the next column.
        data += (class_name.size() + 5);
    }
    // Perform Non Maximum Suppression and draw predictions.
    vector<int> indices;
    NMSBoxes(boxes, confidences, SCORE_THRESHOLD, NMS_THRESHOLD, indices);
    for (int i = 0; i < indices.size(); i++)
    {

        int idx = indices[i];
        Rect box = boxes[idx];

        int left = box.x;
        int top = box.y;
        int width = box.width;
        int height = box.height;
        // Draw bounding box.
        rectangle(input_image, Point(left, top), Point(left + width, top + height), RED, 3*THICKNESS);
        //qDebug() << left << " " << top << " " << width << " " << height << "\n";
        // Get the label for the class name and its confidence.
//        string label = format("%.2f", confidences[idx]);
//        label = class_name[class_ids[idx]] + ":" + label;
//        // Draw class labels.
//        draw_label(input_image, label, left, top);
    }

    return input_image;
}

void CThread::init_model(){
    // Load class list.
    ifstream ifs("/home/cv-startup/projectAI/Qt/textile_new/models/classes.txt");
    string line;

    while (getline(ifs, line))
    {
        class_list.push_back(line);
    }
    net = readNet("/home/cv-startup/projectAI/Qt/textile_new/models/defect_best_sim.onnx");
    net.setPreferableBackend(DNN_BACKEND_CUDA);
    net.setPreferableTarget(DNN_TARGET_CUDA);

}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("db.sqlite");

    if (!db.open()) {
        qDebug() << "Database not opened!\n";
    }
    else{
        qDebug() << "Database opened!\n";
    }
    QVector<Camera>vec;
    getDataFromDB();
    updateList();
    loadCameras(vec);

    setStatus();

    startThreads();
}
MainWindow::~MainWindow()
{

    delete ui;
}

CThread::CThread(){

}
CThread::CThread(int cam_id, QString ip, QString login, QString password, int port){
    this->camera_id = cam_id;
    this->ip = ip;
    this->login = login;
    this->password = password;
    this->port = port;
};
Mat CThread::doDefectDetection(cv::Mat frame){
    detections = pre_process(frame, net);
    frame = post_process(frame.clone(), detections, class_list);

    return frame;
}

void CThread::run(){


    qDebug() << "Camera " << this->camera_id << "\n";
    qDebug() << "IP " << this->ip << "\n";
    qDebug() << "Login " << this->login << "\n";
    qDebug() << "Password " << this->password << "\n";
    qDebug() << "Port " << this->port << "\n";

    a:
    QString name = "/home/cv-startup/projectAI/Qt/textile/" + QString::number(this->camera_id) +".mp4";
    qDebug() << name << "\n";
    VideoCapture cap(name.toStdString().c_str());

    if(!cap.isOpened()){
        qDebug() << "Error opening video stream or file\n";
        return;
    }

    int cnt_defects = 0;
    cv::Mat inFrame;
    init_model();
    while(true){
        cap >> inFrame;
        if(inFrame.empty()){
            qDebug() << this->camera_id << " running again\n";
            goto a;
            continue;
        }
       inFrame = doDefectDetection(inFrame);
        if(this->camera_id == 2 && rand() % 2 == 0){
            cnt_defects ++;
        }
        if(cnt_defects >= 20){
            //emit(signalDefect(this->camera_id));
            /*msleep(10);
            if (CThread::currentThread()->isInterruptionRequested()) {
              qDebug() << Q_FUNC_INFO << " terminated";
              return;
            }*/
        }
        mutex1.lock();

        if(this->active_label){
            QPixmap p = QPixmap::fromImage(QImage(inFrame.data, inFrame.cols, inFrame.rows, inFrame.step, QImage::Format_RGB888).rgbSwapped());
            int w = this->label->width();
            int h = this->label->height();
            this->label->setAlignment(Qt::AlignCenter);
            this->label->setPixmap(p.scaled(w, h, Qt::KeepAspectRatio));

        }
        mutex1.unlock();
        usleep(40000);
    }
    cap.release();
    destroyAllWindows();
    exit(0);
}

void MainWindow::setStatus(){
    for(it_m = machine_items.begin(); it_m != machine_items.end(); it_m++){
        if(getMachineById(it_m.key()).status == 0){
//            it_m.value()->setBackgroundColor(0, QColor(255, 255, 255, 255));
        }
        else if(getMachineById(it_m.key()).status == 1){
//            it_m.value()->setBackgroundColor(0, QColor(0, 255, 0, 255));
        }
    }
}
void MainWindow::loadCameras(QVector<Camera>cameras){

    mutex1.lock();
    int col = 0;
    int row = 0;
    QGridLayout *grid = ui->cameraGrid;

    while(QLayoutItem *item = grid->takeAt(0)){
        if(QWidget *widget = item->widget()){
            widget->deleteLater();
        }
        delete item;
    }
    int col_limit = (cameras.size() + 1) / 2;

    if(cameras.empty()){
        QLabel *label = new QLabel(this);
        ui->cameraGrid->addWidget(label, row, col);
    }
    else{
        for(auto c: cameras){
            QLabel *label = new QLabel(this);
            ui->cameraGrid->addWidget(label, row, col++);
            if(col == col_limit){
                row ++;
                col = 0;
            }
            c.thread->label = label;
        }
    }
    for (auto m: machines){
        for (auto c: m.cameras){
            c.thread->active_label = false;
        }
    }
    for(auto c: cameras){
        qDebug() << c.id << Qt::endl;
        c.thread->active_label = true;
    }
    mutex1.unlock();
}
void MainWindow::signalDefect(int cam_id){
    qDebug() << "Defect " << cam_id << "\n";
    //system("aplay /home/mansurbek/Desktop/sound.wav");
}
void MainWindow::startThreads(){

    for (auto m: machines){
        for (auto c: m.cameras){
            c.thread->start();
        }
    }

}
Machine MainWindow::getMachineById(int id){
    for(auto m: machines){
        if(m.id == id){
            return m;
        }
    }
    return Machine(-1);
}
Camera MainWindow::getCameraFromMachineById(Machine machine, int id){
    for(auto c: machine.cameras){
        if(c.id == id){
            return c;
        }
    }
}

Camera MainWindow::getCameraById(int id){
    for(auto m: machines){
        Camera c = getCameraFromMachineById(m, id);
        if(c.id != -1){
            return c;
        }
    }
}
void MainWindow::on_treeWidget_itemPressed(QTreeWidgetItem *item, int column)
{
    int id = item->data(0, Qt::UserRole).toInt();
    QVector<Camera>vec;
    ui->btnStart->setEnabled(false);
    ui->btnStop->setEnabled(false);
    cur_m = NULL;
    if (!item->parent()) {
        Machine machine = getMachineById(id);
        vec = machine.cameras;
        if(machine.status == 0){
            ui->btnStart->setEnabled(true);
        }
        else{
            ui->btnStop->setEnabled(true);
        }
        cur_m = machine;
    }
    else{
        vec.push_back(getCameraById(id));
    }
    loadCameras(vec);
}
void MainWindow::getDataFromDB(){

    Machine m1;
    m1.id = 1;
    m1.name = "Machine 1";
    m1.status = 1;

    Camera c1;
    c1.id = 1;
    c1.name = "Cam 1";
    c1.ip = "172.";
    c1.login = "172.";
    c1.password = "172.";
    c1.port = 80;
    c1.parent = m1.id;
    c1.status = 1;

    Camera c2;
    c2.id = 2;
    c2.name = "Cam 2";
    c2.ip = "172.";
    c2.login = "172.";
    c2.password = "172.";
    c2.port = 80;
    c2.parent = m1.id;
    c2.status = 1;

    Camera c3;
    c3.id = 3;
    c3.name = "Cam 3";
    c3.ip = "172.";
    c3.login = "172.";
    c3.password = "172.";
    c3.port = 80;
    c3.parent = m1.id;
    c3.status = 1;

    Camera c4;
    c4.id = 4;
    c4.name = "Cam 4";
    c4.ip = "172.";
    c4.login = "172.";
    c4.password = "172.";
    c4.port = 80;
    c4.parent = m1.id;
    c4.status = 1;

    Camera c5;
    c5.id = 5;
    c5.name = "Cam 5";
    c5.ip = "172.";
    c5.login = "172.";
    c5.password = "172.";
    c5.port = 80;
    c5.parent = m1.id;
    c5.status = 1;

    Camera c6;
    c6.id = 6;
    c6.name = "Cam 6";
    c6.ip = "172.";
    c6.login = "172.";
    c6.password = "172.";
    c6.port = 80;
    c6.parent = m1.id;
    c6.status = 1;

    m1.cameras.push_back(c1);
    m1.cameras.push_back(c2);
    m1.cameras.push_back(c3);
    m1.cameras.push_back(c4);
    m1.cameras.push_back(c5);
    m1.cameras.push_back(c6);



    machines.push_back(m1);



}
void MainWindow::updateList(){

    machine_items.clear();
    camera_items.clear();

    ui->treeWidget->clear();
    QList<QTreeWidgetItem *> items;
    for (int i = 0; i < machines.size(); ++i){
        QTreeWidgetItem *item = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr), QStringList(machines[i].name));
        item->setData(0, Qt::UserRole, QVariant::fromValue(machines[i].id));
        machine_items[machines[i].id] = item;
        for(int j = 0; j < machines[i].cameras.size(); j++){
            QTreeWidgetItem *inner_item = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr), QStringList(machines[i].cameras[j].name));
            inner_item->setData(0, Qt::UserRole, QVariant::fromValue(machines[i].cameras[j].id));
            camera_items[machines[i].cameras[j]] = inner_item;
            item->addChild(inner_item);
            machines[i].cameras[j].thread = new CThread();
            machines[i].cameras[j].thread->camera_id = machines[i].cameras[j].id;
            machines[i].cameras[j].thread->ip = machines[i].cameras[j].ip;
            machines[i].cameras[j].thread->login = machines[i].cameras[j].login;
            machines[i].cameras[j].thread->password = machines[i].cameras[j].password;
            machines[i].cameras[j].thread->port = machines[i].cameras[j].port;
            machines[i].cameras[j].thread->active_label = false;
            connect(machines[i].cameras[j].thread, SIGNAL(signalDefect(int)), this, SLOT(signalDefect(int)));
        }
        items.append(item);
    }
    ui->treeWidget->insertTopLevelItems(0, items);
}

void MainWindow::setStatus(Machine machine, int status){
    //Todo set status

    ui->btnStart->setEnabled(false);
    ui->btnStop->setEnabled(false);
    machine.status = status;

    for(int i = 0; i < machines.size(); i++){
        if(machines[i].id == machine.id){
            machines[i] = machine;
            break;
        }
    }
    if(machine.status == 0){
        machine_items[machine.id]->setBackground(0, QColor(255, 0, 0, 123));
        ui->btnStart->setEnabled(true);
    }
    else if(machine.status == 1){
        machine_items[machine.id]->setBackground(0, QColor(255, 255, 255, 123));
        ui->btnStop->setEnabled(true);
    }
    setStatus();

}
void MainWindow::on_btnStart_clicked()
{
    setStatus(cur_m, 1);
}


void MainWindow::on_btnStop_clicked()
{
    setStatus(cur_m, 0);
}

void MainWindow::on_btnMachines_clicked()
{
    FormControl *form = new FormControl();
    form->setAttribute(Qt::WA_DeleteOnClose);
    form->setModal(true);
    form->show();
}



