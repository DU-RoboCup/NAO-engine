#include "robotstatusbox.h"
#include "ui_robotstatusbox.h"

robotstatusbox::robotstatusbox(QWidget *parent, unsigned short r) :
    QWidget(parent),
    robot_number(r),
    status("OFFLINE"),
    battery_life(-1),
    debug_mode("NULL"),
    internal_temps("NULL"),
    ui(new Ui::robotstatusbox)
{
    ui->setupUi(this);

    //Layout
    QVBoxLayout *main_layout = new QVBoxLayout;
    QGridLayout *grid = new QGridLayout;
    //For grid layouts spacing to work well we need to add dummy widgets
    QWidget *dummyVert = new QWidget(this);
    dummyVert->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);


    main_box = new QGroupBox(QString("NAO: " + QString::number(robot_number)));
    //Create/Initialize labels

    QLabel *status_label = new QLabel("<strong>Status: <font color='red'>OFFLINE</font></strong>", this);
    //status_ = new QLabel(status, this);
    grid->addWidget(status_label, 0, 2, Qt::AlignCenter);
    //grid->addWidget(status_, 0, 1, Qt::AlignCenter);

    QLabel *battery_life_label = new QLabel("Battery Life:", this);
    battery_life_ = new QLabel(QString::number(battery_life), this);
    grid->addWidget(battery_life_label, 1, 0, 1, 1);
    grid->addWidget(battery_life_, 1, 1, 1, 1);

    grid->addWidget(dummyVert, 1, 2);


    QLabel *debug_mode_label = new QLabel("Debug Mode:", this);
    debug_mode_ = new QLabel(debug_mode, this);
    grid->addWidget(debug_mode_label, 1, 3, 1, 1);
    grid->addWidget(debug_mode_, 1, 4, 1, 1);


    QLabel *internal_temps_label = new QLabel("Internal Temps:", this);
    internal_temps_ = new QLabel(internal_temps, this);
    grid->addWidget(internal_temps_label, 2, 0, 1, 1);
    grid->addWidget(internal_temps_, 2, 1, 1, 1);

    QLabel *connection_label = new QLabel("connection:", this);
    QLabel *connection_stat = new QLabel("NULL", this);
    grid->addWidget(connection_label, 2, 3, 1, 1);
    grid->addWidget(connection_stat, 2, 4, 1, 1);

    main_box->setLayout(grid);
    main_layout->addWidget(main_box);

    //Enable hover events
    main_box->setAttribute(Qt::WA_Hover, true);
    
    //Done
    this->setLayout(main_layout);

}

robotstatusbox::~robotstatusbox()
{
    qDebug() << "Deleting main_box from robotstatusbox";
    delete main_box;
    delete ui;
    qDebug() << "robotstatusbox succesfully destructed";
}

void robotstatusbox::update_status_values(unsigned short rn, QString s, double bl, QString dm, QString it)
{
    robot_number = rn;
    status = s;
    battery_life = bl;
    debug_mode = dm;
    internal_temps = it;
    //Values have been update, now update the widget
    update_status();
}

void robotstatusbox::update_status()
{
    //Essentially redraws all the values in the widget
    status_->setText(status);
    robot_number_->setText(QString::number(robot_number));
    battery_life_->setText(QString::number(battery_life));
    debug_mode_->setText(debug_mode);
    internal_temps_->setText(internal_temps);
}

void robotstatusbox::hoverEnter(QHoverEvent *event)
{
    qDebug() << Q_FUNC_INFO << this->objectName();
}

void robotstatusbox::hoverLeave(QHoverEvent *event)
{
    qDebug() << Q_FUNC_INFO << this->objectName();
}
void robotstatusbox::hoverMove(QHoverEvent *event)
{
    qDebug() << Q_FUNC_INFO << this->objectName();
}

bool robotstatusbox::hoverEvents(QEvent *event)
{
    switch(event->type())
    {
    case QEvent::HoverEnter:
        hoverEnter(static_cast<QHoverEvent *>(event));
        return true;
        break;

    case QEvent::HoverLeave:
        hoverEnter(static_cast<QHoverEvent *>(event));
        return true;
        break;
    case QEvent::HoverMove:
        hoverEnter(static_cast<QHoverEvent *>(event));
        return true;
        break;
    }
}


