#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QDebug>
#include <QLabel>
#include <QSqlDatabase>
#include <formcontrol.h>
#include <QtCore>
#include <QMutex>
#include <opencv2/opencv.hpp>
#include <fstream>
using namespace std;
using namespace cv;
using namespace cv::dnn;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class CThread: public QThread{
    Q_OBJECT
public:
    int camera_id;
    QString ip;
    QString login;
    QString password;
    int port;
    // Constants.
    const float INPUT_WIDTH = 640.0;
    const float INPUT_HEIGHT = 640.0;
    const float SCORE_THRESHOLD = 0.3;
    const float NMS_THRESHOLD = 0.45;
    const float CONFIDENCE_THRESHOLD = 0.4;

    // Text parameters.
    const float FONT_SCALE = 0.7;
    const int FONT_FACE = FONT_HERSHEY_SIMPLEX;
    const int THICKNESS = 1;

    // Colors.
    Scalar BLACK = Scalar(0,0,0);
    Scalar BLUE = Scalar(255, 178, 50);
    Scalar YELLOW = Scalar(0, 255, 255);
    Scalar RED = Scalar(0,0,255);
    Net net;
    vector<Mat> detections;
    vector<string> class_list;

    QLabel *label;
    bool active_label;
    CThread(int cam_id, QString ip, QString login, QString password, int port);
    CThread();
    Mat doDefectDetection(cv::Mat frame);
    void draw_label(Mat& input_image, string label, int left, int top);
    vector<Mat> pre_process(Mat &input_image, Net &net);
    Mat post_process(Mat input_image, vector<Mat> &outputs, const vector<string> &class_name);
//    static long long int get_time();
    void init_model();
signals:
    void signalDefect(int cam_id);
protected:

    void run();
};

class Camera{
public:
    int id;
    QString ip;
    QString name;
    QString login;
    QString password;
    int port;
    int status;
    int parent;
    CThread *thread;
    Camera(){
        thread = new CThread();
    }
    bool operator <(const Camera& other) const
    {
        return id < other.id;
    }
};
class Machine{

public:
    int id;
    QString name;
    QVector<Camera>cameras;
    int status;
    Machine(){

    }
    Machine(int id){
        this->id = id;
    }
    bool operator <(const Machine& other) const
    {
        return id < other.id;
    }
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    void setPixmap(QPixmap pixmap);
    MainWindow(QWidget *parent = nullptr);
    QThread *thread;
    ~MainWindow();
private slots:
    void signalDefect(int cam_id);
    void on_treeWidget_itemPressed(QTreeWidgetItem *item, int column);
    void updateList();
    void getDataFromDB();
    void setStatus();
    void startThreads();
    void loadCameras(QVector<Camera>cameras);
    Camera getCameraById(int id);
    Camera getCameraFromMachineById(Machine machine, int id);
    Machine getMachineById(int id);
    void setStatus(Machine machine, int status);
    void on_btnStart_clicked();
    void on_btnStop_clicked();
    void on_btnMachines_clicked();


private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
