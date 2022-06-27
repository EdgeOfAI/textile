#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
//    treeWidget->setColumnCount(1);
//    QList<QTreeWidgetItem *> items;
//    for (int i = 0; i < 10; ++i)
//        items.append(new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr), QStringList(QString("item: %1").arg(i))));
//    treeWidget->insertTopLevelItems(0, items);
    ui->setupUi(this);
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_treeWidget_itemPressed(QTreeWidgetItem *item, int column)
{
    qInfo() << column << endl;
}


