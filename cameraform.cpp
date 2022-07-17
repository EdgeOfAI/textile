#include "cameraform.h"
#include "ui_cameraform.h"
#include <QComboBox>

CameraForm::CameraForm(QWidget *parent, QSqlDatabase *tmp_db) :
    QDialog(parent),
    ui(new Ui::CameraForm)
{
    ui->setupUi(this);
    db = tmp_db;
    loadMachines();
}
void CameraForm::loadMachines(){
    QSqlQuery query("SELECT * FROM db_machine ORDER BY name ASC");
    ui->cbMachine->clear();
    while(query.next()){
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        ui->cbMachine->addItem(name, QVariant::fromValue(id));
    }
}
CameraForm::~CameraForm()
{
    delete ui;
}

void CameraForm::on_buttonBox_accepted()
{
    int parent = ui->cbMachine->currentData(Qt::UserRole).toInt();
    QString name = ui->editName->text();
    QString ip = ui->editIp->text();
    QString login = ui->editLogin->text();
    QString password = ui->editPassword->text();
    int port = ui->editPort->value();

    if(!db->open()){
        qDebug() << "Error opening database\n";
    }
    else {
        QSqlQuery query;
        query.prepare("INSERT INTO db_camera (name, ip, login, password, port, status, parent) VALUES (:name, :ip, :login, :password, :port, 1, :parent);");
        query.bindValue(":name", name);
        query.bindValue(":login", login);
        query.bindValue(":password", password);
        query.bindValue(":ip", ip);
        query.bindValue(":parent", parent);
        query.bindValue(":port", port);

        if (query.exec()) {
            qDebug() << "Added camera\n";
        }
        else {
            qDebug() << "Error in adding camera\n";
        };
    }
}

