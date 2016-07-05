#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QGridLayout *gridlayout = new QGridLayout;

    QTextBrowser *log_console = new QTextBrowser;
    QLabel *main_label = new QLabel("NAO User Control Interface");
    main_label->setAlignment(Qt::AlignCenter);

    robot_status_widget *main_status = new robot_status_widget;
    //Add widgets (*Widget, row, column, rowspan, colspan)

    //Row 0:
    gridlayout->addWidget(main_label, 0, 0, 1, 3);


    //Row 1:
    /*gridlayout->addWidget(b1, 1, 0, 1, 1);
    gridlayout->addWidget(b2, 1, 1, 1, 1);
    gridlayout->addWidget(b3, 1, 2, 1, 1);*/

    gridlayout->addWidget(main_status);

    //Row 2:
    gridlayout->addWidget(log_console, 2, 0, 1, 3);

    ui->centralWidget->setLayout(gridlayout);



}

MainWindow::~MainWindow()
{
    delete ui;
}
