#ifndef CONNECTWIDGET_H
#define CONNECTWIDGET_H

#include <QWidget>

namespace Ui {
class ConnectWidget;
}

class ConnectWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectWidget(QWidget *parent = 0);
    ~ConnectWidget();

private:
    Ui::ConnectWidget *ui;
};

#endif // CONNECTWIDGET_H
