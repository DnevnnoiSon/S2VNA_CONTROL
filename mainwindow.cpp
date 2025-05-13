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
    connect(communicator, &ICommunication::deviceStatusChanged,
    this, &MainWindow::onDeviceStatusChanged, Qt::QueuedConnection); // связь между потоками
// Сигнал сокетной ошибки --> Обработка ошибки
    connect(communicator, &ICommunication::errorOccurred,
    this, &MainWindow::handleDeviceError, Qt::QueuedConnection); // связь между потоками
// Передача валидных данных --> в поток связи
    connect(this, &MainWindow::transfer_measurement_config,
    communicator, &ICommunication::accept_measurement_config,
    Qt::QueuedConnection);
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::on_measureButton_clicked(){
//Передаваемые замеры:
    measurement_config = {
        .start_freq = ui->startSpinBox->value(),
        .stop_freq = ui->endSpinBox->value(),
        .step = ui->stepSpinBox->value(),
        .power = ui->powerSpinBox->value(),
        .samples = ui->samplesspinBox->value()
    };
//Настройка сетевого подключения:
    //..........



//Передача в сокетный поток для отправки
//передаваться будет сразу конвертированная в scpi команда:
    QString command; //= конвертация
    emit transfer_measurement_config(command);
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





