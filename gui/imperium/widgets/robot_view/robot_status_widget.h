#ifndef ROBOT_STATUS_WIDGET_H
#define ROBOT_STATUS_WIDGET_H

#include "robotstatusbox.h"

#include <QWidget>
#include <QTabWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QPushButton>

namespace Ui {
class robot_status_widget;
}

class robot_status_widget : public QWidget
{
    Q_OBJECT

public:
    explicit robot_status_widget(QWidget *parent = 0);
    ~robot_status_widget();

private:
    Ui::robot_status_widget *ui;

};

#endif // ROBOT_STATUS_WIDGET_H
