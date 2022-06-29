#include "formcontrol.h"
#include "ui_formcontrol.h"

FormControl::FormControl(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormControl)
{
    ui->setupUi(this);
}

FormControl::~FormControl()
{
    delete ui;
}
