#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Layout
    QVBoxLayout *main_layout = new QVBoxLayout();


    //Widgets
    connect_window = new Connect(this);

    //Connecting elements with layouts
    main_layout->addWidget(connect_window);
    QWidget *main = new QWidget();
    main->setLayout(main_layout);
    MainWindow::setCentralWidget(main);
}

MainWindow::~MainWindow()
{
    delete ui;
}
