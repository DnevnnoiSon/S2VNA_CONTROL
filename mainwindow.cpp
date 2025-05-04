#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->measureButton, &QPushButton::clicked, this, &MainWindow::on_measureButton_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_measureButton_clicked(){
    measurement_config = {
        .start_freq = ui->startSpinBox->value(),
        .stop_freq = ui->endSpinBox->value(),
        .step = ui->stepSpinBox->value(),
        .power = ui->powerSpinBox->value(),
        .samples = ui->samplesspinBox->value()
    };
}

