#include "connectwidget.h"
#include "ui_connectwidget.h"

ConnectWidget::ConnectWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConnectWidget)
{
    ui->setupUi(this);
}

ConnectWidget::~ConnectWidget()
{
    delete ui;
}
