/*
Copyright (c) 2016 "University of Denver"

This file is part of Pineapple.

Pineapple is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

VERSION HISTORY
-- Created by Paul Heinen 06/16/16

// FILE DESCRIPTION
This is the network connection widget and module for the master control program.
It, like the rest of imperium, only relies on QT dependencies.

This specific widget gets information about the host computer, creates a server,
then scans for the listening NAO robots by sending a UDP broadcast. When the NAO's
reply, a connection is established.
*/

// INCLUDES

#include "connect.h"
#include "ui_connect.h"


Connect::Connect(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Connect)
{
    ui->setupUi(this);

    //Get the hosts ipv4 address to display
    ui->user_ip_label->setText(getHostIP());
    try{
        host_config = LuaTable::fromFile("../../config/network/host.lua");
    } catch (...) {
        qDebug() << "Lua Tables have failed!";
    }

    scan_timer = new QTimer(this);
    connect(this, SIGNAL(clicked()), this, SLOT(scanNetwork()));
    connect(this, SIGNAL(readySend()), this, SLOT(scanNetwork()));
    connect(scan_timer, SIGNAL(timeout()), this, SLOT(listen()));
}

Connect::~Connect()
{
    delete ui;
}

QString Connect::getHostIP()
{
    QString IP = "NULL";
    foreach (const QHostAddress &addr, QNetworkInterface::allAddresses()) {
        qDebug() << addr.toString();
        if (addr.protocol() == QAbstractSocket::IPv4Protocol && addr != QHostAddress(QHostAddress::LocalHost))
            IP = addr.toString();
    }
    return IP;
}
void Connect::initSocket()
{


    quint16 port = host_config["port"];
    qDebug() << "Port: " + QString::number(port);

    //Bind the ip and port
    QString ip = host_config["ip"];
    udpSocket->bind(QHostAddress::LocalHost, port);
}

void Connect::scanNetwork()
{
    qDebug() << "Beginning Network Scan...";

    //Send out a UDP Broadcast
    ui->scan_button->setEnabled(false);
    //scan_timer->start(1000);
    QByteArray message = "Hello_NAO";
    udpSocket->writeDatagram(message.data(), QHostAddress::Broadcast, port);

    //Begin listening to NAO's for response.


}
void Connect::listen()
{
    while(udpSocket->hasPendingDatagrams()) {
        QByteArray new_message;
        new_message.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(new_message.data(), new_message.size());
        qDebug() << "Recieved a Datagram: " << new_message.data();
    }

}

void Connect::configNetwork()
{
    //Create a popup dialog prompting for network information
    bool ok;
    QString config_info = QInputDialog::getText(this,
                                                "Enter Network info",
                                                "IPV4_address:PORT (ex: 127.0.0.1:9999",
                                                QLineEdit::Normal,
                                                "127.0.0.1:9999",
                                                &ok);
    if (ok && !config_info.isEmpty())
    {
        QStringList host = config_info.split(":");
        qDebug() << "Host info: " << host[0] << ":" << host[1];

        //Update the Luatable config file

        host_config["ip"] = host[0];
        host_config["port"] = host[1];
    } else {
        qDebug() << "Network Configuration was not changed";
    }
}



