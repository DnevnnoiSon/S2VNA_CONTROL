#include "dialognetwork.h"
#include "ui_dialognetwork.h"

DialogNetwork::DialogNetwork(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogNetwork)
{
    ui->setupUi(this);
}

DialogNetwork::~DialogNetwork()
{
    delete ui;
}
