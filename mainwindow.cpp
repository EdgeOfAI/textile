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

QMutex mutex;

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
void CThread::doDefectDetection(cv::Mat frame){
}

void CThread::run(){


    qDebug() << "Camera " << this->camera_id << "\n";
    qDebug() << "IP " << this->ip << "\n";
    qDebug() << "Login " << this->login << "\n";
    qDebug() << "Password " << this->password << "\n";
    qDebug() << "Port " << this->port << "\n";

    a:
    QString name = "/home/mansurbek/Desktop/" + QString::number(this->camera_id) +".mp4";
    qDebug() << name << "\n";
    VideoCapture cap(name.toStdString().c_str());

    if(!cap.isOpened()){
        qDebug() << "Error opening video stream or file" << endl;
        return;
    }

    int cnt_defects = 0;
    cv::Mat inFrame;
    while(true){
        cap >> inFrame;
        if(inFrame.empty()){
            qDebug() << this->camera_id << " running again\n";
            goto a;
            continue;
        }
        doDefectDetection(inFrame);
        if(this->camera_id == 2 && rand() % 2 == 0){
            cnt_defects ++;
        }
        if(cnt_defects >= 20){
            emit(signalDefect(this->camera_id));
            msleep(10);
            if (CThread::currentThread()->isInterruptionRequested()) {
              qDebug() << Q_FUNC_INFO << " terminated";
              return;
            }
        }
        mutex.lock();

        if(this->active_label){
            QPixmap p = QPixmap::fromImage(QImage(inFrame.data, inFrame.cols, inFrame.rows, inFrame.step, QImage::Format_RGB888).rgbSwapped());
            int w = this->label->width();
            int h = this->label->height();
            this->label->setAlignment(Qt::AlignCenter);
            this->label->setPixmap(p.scaled(w, h, Qt::KeepAspectRatio));

        }
        mutex.unlock();
        usleep(40000);
    }
    cap.release();
    destroyAllWindows();
    exit(0);
}

void MainWindow::setStatus(){
    for(it_m = machine_items.begin(); it_m != machine_items.end(); it_m++){
        if(getMachineById(it_m.key()).status == 0){
            it_m.value()->setBackgroundColor(0, QColor(255, 255, 255, 255));
        }
        else if(getMachineById(it_m.key()).status == 1){
            it_m.value()->setBackgroundColor(0, QColor(0, 255, 0, 255));
        }
    }
}
void MainWindow::loadCameras(QVector<Camera>cameras){

    mutex.lock();
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
    mutex.unlock();
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



