#ifndef CAMERAFORM_H
#define CAMERAFORM_H

#include <QDialog>
#include <QSqlDatabase>
#include <QtCore>
#include <QMutex>
#include <QSqlQuery>

namespace Ui {
class CameraForm;
}

class CameraForm : public QDialog
{
    Q_OBJECT

public:
    explicit CameraForm(QWidget *parent = nullptr, QSqlDatabase *tmp_db = 0);
    ~CameraForm();

private slots:
    void on_buttonBox_accepted();
    void loadMachines();
private:
    Ui::CameraForm *ui;
    QSqlDatabase *db;
};

#endif // CAMERAFORM_H
