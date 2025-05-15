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

    ui->consoleButton->setFlat(true);
    ui->consoleButton->setStyleSheet("background: transparent; border: none;");
    ui->networkButton->setFlat(true);
    ui->networkButton->setStyleSheet("background: transparent; border: none;");

// Нажатие measure --> Валидация данных
    connect(ui->measureButton, &QPushButton::clicked, this, &MainWindow::on_measureButton_clicked);
// Нажатие update --> Обновление настроек сетевого подключения
    connect(ui->updateButton,&QPushButton::clicked, this, &MainWindow::on_updateButton_clicked);
// Изменение статуса хоста --> measure в зеленный цвет
    connect(communicator, &ICommunication::deviceStatusChanged,

    this, &MainWindow::onDeviceStatusChanged, Qt::QueuedConnection); // связь между потоками
// Сигнал сокетной ошибки --> Обработка ошибки
    connect(communicator, &ICommunication::errorOccurred,
    this, &MainWindow::handleDeviceError, Qt::QueuedConnection); // связь между потоками

// Передача валидных данных --> в поток связи
    connect(this, &MainWindow::transfer_measure_config,
    communicator, &ICommunication::accept_measure_config,
    Qt::QueuedConnection);
// Передача сетевых настроек --> в поток связи
    connect(this, &MainWindow::transfer_setting_config,
    communicator, &ICommunication::accept_setting_config,
    Qt::QueuedConnection);
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::on_measureButton_clicked(){
    //Вводные данные:
    MeasurementConfig measurement_config;
    //Передаваемые замеры:
    measurement_config = {
        .start_freq = ui->startSpinBox->value(),
        .stop_freq = ui->endSpinBox->value(),
        .step = ui->stepSpinBox->value(),
        .power = ui->powerSpinBox->value(),
        .samples = ui->samplesspinBox->value()
    };
//Проверка принятых данных:

//Передача в сокетный поток для отправки:
    QString command; //= конвертация
    emit transfer_measure_config(command);
}

void MainWindow::on_updateButton_clicked()
{
    Settings setting;
    //Настройка сетевого подключения:
    setting.network.ip_addr = ui->ipLineEdit->text();
    setting.network.port = ui->portSpinBox->value();

    emit transfer_setting_config(setting);
}


void MainWindow::onDeviceStatusChanged(bool isReady){
// Смена цвета:
    QString style = isReady ? "background-color: green;" : "";
    ui->measureButton->setStyleSheet(style);
// Доступность кнопки:
    ui->measureButton->setEnabled(isReady);
}

void MainWindow::handleDeviceError(const QString& errorMessage){
    statusBar()->showMessage("Status: " + errorMessage);
    //ui->measureButton->setEnabled(false);
}





