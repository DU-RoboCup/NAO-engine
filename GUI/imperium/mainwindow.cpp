#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Create Qt elements here
    QGridLayout *gridLayout = new QGridLayout;
    QPushButton *addRobot = new QPushButton("Connect a Robot");


}

MainWindow::~MainWindow()
{
    delete ui;
}
