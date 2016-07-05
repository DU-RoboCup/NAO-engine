#ifndef ROBOTSTATUSBOX_H
#define ROBOTSTATUSBOX_H

#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include <QString>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QHoverEvent>

namespace Ui {
class robotstatusbox;
}

class robotstatusbox : public QWidget
{
    Q_OBJECT

public:
    explicit robotstatusbox(QWidget *parent = 0, unsigned short r = 0);
    ~robotstatusbox();

    //Mouse Events
    void hoverEnter(QHoverEvent *event);
    void hoverLeave(QHoverEvent *event);
    void hoverMove(QHoverEvent *event);
    bool hoverEvents(QEvent *event);

    void update_status_values(unsigned short rn, QString s, double bl, QString dm, QString it);
    void update_status();
    
    QGroupBox *main_box;
    //Labels
    QLabel *status_, *robot_number_, *battery_life_, *debug_mode_, *internal_temps_;

private:
    Ui::robotstatusbox *ui;
    unsigned short robot_number;
    QString status;
    double battery_life;
    QString debug_mode;
    QString internal_temps;
};

#endif // ROBOTSTATUSBOX_H
