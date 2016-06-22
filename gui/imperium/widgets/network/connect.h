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

#ifndef CONNECT_H
#define CONNECT_H

// INCLUDES
#include <QWidget>
#include <QHostAddress>
#include <QAbstractSocket>
#include <QNetworkInterface>
#include <QUdpSocket>
#include <QTimer>
#include <QInputDialog>
#include <QFormLayout>
#include <QDebug>

#include "../../lib/luatables/luatables.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"


namespace Ui {
class Connect;
}

class Connect : public QWidget
{
    Q_OBJECT

public:

    explicit Connect(QWidget *parent = 0);
    QString getHostIP();
    QHostAddress host;
    void configNetwork();
    void initSocket();
    LuaTable host_config;
    short port;
    ~Connect();

private slots:
    void sendDatagram();


private:
    Ui::Connect *ui;
    void scanNetwork();
    void listen();
    QUdpSocket *udpSocket;
    QTimer *scan_timer;
};

#endif // CONNECT_H
