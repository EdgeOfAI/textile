#include "formcontrol.h"
#include "ui_formcontrol.h"
#include <QListWidgetItem>

FormControl::FormControl(QWidget *parent, QSqlDatabase *tmp_db) :
    QDialog(parent),
    ui(new Ui::FormControl)
{
    ui->setupUi(this);
    db = tmp_db;

    if(!db->open()){
        qDebug() << "Error opening database\n";
    }
    else{
        loadMachines();
        loadCameras();
    }
}
QString FormControl::getMachineName(int id){
    QSqlQuery query("SELECT * FROM db_machine WHERE id = (:machine)");
    query.bindValue(":machine", id);

    if (query.exec())
    {
       if(query.next())
       {
           QString name = query.value(1).toString();
           return name;
       }
       else {
           return "";
       }
    }
    return "";

}
void FormControl::loadMachines(){
    ui->listMachines->clear();
    QSqlQuery query("SELECT * FROM db_machine");

    while(query.next()){
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        QListWidgetItem *item = new QListWidgetItem(name);
        item->setData(Qt::UserRole, QVariant::fromValue(id));
        ui->listMachines->addItem(item);
    }
}
void FormControl::loadCameras(){
    ui->listCameras->clear();
    QSqlQuery query("SELECT db_camera.id, db_camera.name, db_machine.name, db_camera.ip, db_camera.port FROM db_camera LEFT JOIN db_machine ON db_machine.id = db_camera.parent ORDER BY db_machine.id ASC, db_camera.name ASC");

    while(query.next()){
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        QString m_name = query.value(2).toString();
        QString ip = query.value(3).toString();
        int port = query.value(4).toInt();
        QListWidgetItem *item = new QListWidgetItem(name + " (" + m_name + "," + ip + ":" + QString::number(port) + ")");
        item->setData(Qt::UserRole, QVariant::fromValue(id));
        ui->listCameras->addItem(item);
    }
}
FormControl::~FormControl()
{
    delete ui;
}

void FormControl::on_FormControl_destroyed()
{

}

void FormControl::on_btnAddMachine_clicked()
{
    MachineForm *form = new MachineForm(this, db);
    connect(form, SIGNAL(destroyed()), this, SLOT(loadMachines()));
    form->setAttribute(Qt::WA_DeleteOnClose);
    form->setModal(true);
    form->show();
}


void FormControl::on_btnDeleteMachine_clicked()
{
    if(ui->listMachines->selectedItems().length() > 0){
        int m_id = ui->listMachines->selectedItems()[0]->data(Qt::UserRole).toInt();
        if(!db->open()){
            qDebug() << "Error opening database\n";
        }
        else {
            QSqlQuery query;
            query.prepare("DELETE FROM db_machine WHERE id = (:id);");
            query.bindValue(":id", m_id);

            if (query.exec()) {
                qDebug() << "Machine deleted\n";
                loadMachines();
                loadCameras();
            }
            else {
                qDebug() << "Error in delete machine\n";
            };
        }
    }
}
void FormControl::on_btnDeleteCamera_clicked()
{
    if(ui->listCameras->selectedItems().length() > 0){
        int c_id = ui->listCameras->selectedItems()[0]->data(Qt::UserRole).toInt();
        if(!db->open()){
            qDebug() << "Error opening database\n";
        }
        else {
            QSqlQuery query;
            query.prepare("DELETE FROM db_camera WHERE id = (:id);");
            query.bindValue(":id", c_id);

            if (query.exec()) {
                qDebug() << "Camera deleted\n";
                loadCameras();
            }
            else {
                qDebug() << "Error in delete camera\n";
            };
        }
    }
}
void FormControl::on_btnAddCamera_clicked()
{
    CameraForm *form = new CameraForm(this, db);
    connect(form, SIGNAL(destroyed()), this, SLOT(loadCameras()));
    form->setAttribute(Qt::WA_DeleteOnClose);
    form->setModal(true);
    form->show();
}

