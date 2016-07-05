#include "robot_status_widget.h"
#include "ui_robot_status_widget.h"

robot_status_widget::robot_status_widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::robot_status_widget)
{
    ui->setupUi(this);

    QTabWidget *tabs = new QTabWidget;

    //Placeholder buttons
    robotstatusbox *r0 = new robotstatusbox(this, 1);
    robotstatusbox *r1 = new robotstatusbox(this, 2);
    robotstatusbox *r2 = new robotstatusbox(this, 3);
    robotstatusbox *r3 = new robotstatusbox(this, 4);
    robotstatusbox *r4 = new robotstatusbox(this, 5);
    robotstatusbox *r5 = new robotstatusbox(this, 6);

    //Layouts
    QGridLayout *main_tab_layout = new QGridLayout;
    QVBoxLayout *parent_layout = new QVBoxLayout;

    //Set up placeholders

    //Row 1:
    main_tab_layout->addWidget(r0, 0, 0, 1, 1);
    main_tab_layout->addWidget(r1, 0, 1, 1, 1);
    main_tab_layout->addWidget(r2, 0, 2, 1, 1);

    //Row 2:

    main_tab_layout->addWidget(r3, 1, 0, 1, 1);
    main_tab_layout->addWidget(r4, 1, 1, 1, 1);
    main_tab_layout->addWidget(r5, 1, 2, 1, 1);

    //Create the main tab with a widget containing the gridlayout
    QWidget  *main = new QWidget;
    main->setLayout(main_tab_layout);
    tabs->addTab(main, "Main");

    //Finalize
    parent_layout->addWidget(tabs);
    this->setLayout(parent_layout);

}

robot_status_widget::~robot_status_widget()
{
    delete ui;
}
