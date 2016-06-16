#ifndef CONNECT_H
#define CONNECT_H

#include <QWidget>
#include <QHostAddress>
#include <QDebug>

namespace Ui {
class Connect;
}

class Connect : public QWidget
{
    Q_OBJECT

public:
    explicit Connect(QWidget *parent = 0);
    QHostAddress host;

    ~Connect();

private:
    Ui::Connect *ui;
};

#endif // CONNECT_H
