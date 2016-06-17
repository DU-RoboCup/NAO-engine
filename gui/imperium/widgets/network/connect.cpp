#include "connect.h"
#include "ui_connect.h"

Connect::Connect(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Connect)
{
    ui->setupUi(this);



    qDebug() << "Local IPv4 Address: " + QString::number(host.toIPv4Address());
    qDebug() << "Local Broadcast Address: " + QString::number(host.Broadcast);

    /* Network Initialization
     * Probably best to keep network related things in try/catch blocks
     */
    try {
        //Get the hosts ipv4 address to display
        ui->user_ip_label->setText(QString::number(host.toIPv4Address()));
    } catch (std::exception &e) {
        qDebug() << "Networking Error! Failed to get local host information";
        ui->user_ip_label->setText("NULL");
    }

}

Connect::~Connect()
{
    delete ui;
}
