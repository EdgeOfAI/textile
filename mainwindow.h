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

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class CThread;
class CThread: public QThread{
public:
    int camera_id;
    QLabel *label;
    CThread(){
    };
    void run();
    void doDefectDetection(cv::Mat frame);
};

class Camera{
public:
    int id;
    QString ip;
    QString name;
    QString login;
    QString password;
    QString port;
    int status;
    int parent;
    CThread *thread;
    Camera(){
        thread = new CThread();
    }
    Camera(int id){
        this->id = id;
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
    ~MainWindow();
private slots:
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
