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
#include <QSqlQuery>

#include <opencv2/opencv.hpp>

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
    QLabel *label;
    bool active_label;
    CThread(int cam_id, QString ip, QString login, QString password, int port);
    CThread();
    void doDefectDetection(cv::Mat frame);
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
    void stopThreads();
    void loadCameras(QVector<Camera>cameras);
    Camera getCameraById(int id);
    Camera getCameraFromMachineById(Machine machine, int id);
    Machine getMachineById(int id);
    void setStatus(Machine machine, int status);
    void on_btnStart_clicked();
    void on_btnStop_clicked();
    void on_btnMachines_clicked();

    void on_btnStopThread_clicked();

    void on_btnStartThread_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
