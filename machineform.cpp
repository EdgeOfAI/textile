#include "machineform.h"
#include "ui_machineform.h"

MachineForm::MachineForm(QWidget *parent, QSqlDatabase *tmp_db) :
    QDialog(parent),
    ui(new Ui::MachineForm)
{
    ui->setupUi(this);
    db = tmp_db;

}
MachineForm::~MachineForm()
{
    delete ui;
}

void MachineForm::on_buttonBox_accepted()
{
    QString name = ui->editName->text();
    if(!db->open()){
        qDebug() << "Error opening database\n";
    }
    else {
        QSqlQuery query;
        query.prepare("INSERT INTO db_machine (name, status) VALUES (:name, 1);");
        query.bindValue(":name", name);

        if (query.exec()) {
            qDebug() << "Added machine\n";
        }
        else {
            qDebug() << "Error in adding machine\n";
        };
    }
}

