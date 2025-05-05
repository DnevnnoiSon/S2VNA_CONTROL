#include "mainwindow.h"
#include "ui_mainwindow.h"


#include "socketcommunication.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

// Подключение к хосту --> авто-пробуждение S2VNA потока
    communicator = new SocketCommunication(this);
//График ВАЦ [полученные с S2VNA параметры]
    chart = new QChart();

    // Установка дефолтных значений
    ui->ipLineEdit->setText("127.0.0.1");

    ui->portSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    ui->portSpinBox->setValue(5025);

// Нажатие measure --> Валидация данных
    connect(ui->measureButton, &QPushButton::clicked, this, &MainWindow::on_measureButton_clicked);
// Изменение статуса хоста --> measure в зеленный цвет
    connect(communicator, &SocketCommunication::deviceStatusChanged,
    this, &MainWindow::onDeviceStatusChanged, Qt::QueuedConnection); // связь между потоками
// Сигнал сокетной ошибки --> Обработка ошибки
    connect(communicator, &SocketCommunication::errorOccurred,
    this, &MainWindow::handleDeviceError, Qt::QueuedConnection); // связь между потоками

}

MainWindow::~MainWindow(){
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

void MainWindow::onDeviceStatusChanged(bool isReady){
// Смена цвета:
    QString style = isReady ? "background-color: green;" : "";
    ui->measureButton->setStyleSheet(style);
// Доступность кнопки:
    ui->measureButton->setEnabled(isReady);
}

void MainWindow::handleDeviceError(const QString& errorMessage){
    statusBar()->showMessage("Ошибка: " + errorMessage);
    ui->measureButton->setEnabled(false);
}





