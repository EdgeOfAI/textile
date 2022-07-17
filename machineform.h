#ifndef MACHINEFORM_H
#define MACHINEFORM_H

#include <QDialog>
#include <QSqlDatabase>
#include <QtCore>
#include <QMutex>
#include <QSqlQuery>

namespace Ui {
class MachineForm;
}

class MachineForm : public QDialog
{
    Q_OBJECT

public:
    explicit MachineForm(QWidget *parent = nullptr, QSqlDatabase *tmp_db = 0);
    ~MachineForm();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::MachineForm *ui;
    QSqlDatabase *db;
};

#endif // MACHINEFORM_H
