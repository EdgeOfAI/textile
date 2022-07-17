#ifndef FORMCONTROL_H
#define FORMCONTROL_H

#include <QDialog>
#include <QSqlDatabase>
#include <QtCore>
#include <QMutex>
#include <QSqlQuery>
#include "machineform.h"
#include "cameraform.h"

namespace Ui {
class FormControl;
}

class FormControl : public QDialog
{
    Q_OBJECT

public:
    explicit FormControl(QWidget *parent = nullptr, QSqlDatabase *tmp_db = 0);
    ~FormControl();

private slots:
    void on_FormControl_destroyed();
    QString getMachineName(int id);
    void loadMachines();
    void loadCameras();

    void on_btnAddMachine_clicked();

    void on_btnDeleteMachine_clicked();

    void on_btnDeleteCamera_clicked();

    void on_btnAddCamera_clicked();

private:
    Ui::FormControl *ui;
    QSqlDatabase *db;
};

#endif // FORMCONTROL_H
