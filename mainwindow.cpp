#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "socketcommunication.h"

#define MAX_POWER 60

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

// Подключение к хосту --> авто-пробуждение S2VNA потока
    communicator = new SocketCommunication(this);
// Выбор scpi устройства

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

    if( (ui->startSpinBox->value() >= ui->endSpinBox->value()) ||
        (ui->powerSpinBox->value() > MAX_POWER) )
    {
        handleDeviceError("Incorrect valid parameters");
    }
    if (!ui->cense1Button->isChecked() && !ui->cense2Button->isChecked()) {
        handleDeviceError("Incorrect valid cense");
        return;
    }
//Ввалидные данные:
    QVariantMap config{
        {":SENS :FREQ:START", ui->startSpinBox->value()}, // Начальная частота
        {":SENS :FREQ:STOP",  ui->endSpinBox->value()  }, // Конечная частота
        {":SENS :BWID",       ui->stepspinBox->value() }, // Полоса фильтра ПЧ - [шаг измерения]
        {":SOUR :POW1",       ui->powerSpinBox->value()}, // Мощность
    };
    //если не senc2, то sence1:
    bool isAnySenc = ui->cense2Button->isChecked();
    const QChar replacement = isAnySenc ? '1' : '0';

//Валидные данные + нужный sense:
    QVariantMap modifiedConfig;

    for (const auto &[key, value] : config.asKeyValueRange()){
        QString modifiedKey = key;
        modifiedKey.replace(' ', replacement);
        modifiedConfig.insert(modifiedKey, value);
    }
//Передача в сокетный поток для отправки:
    QString command = scpi.generateCommand(modifiedConfig); //= конвертация
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
    QString style = isReady ? "background-color: darkgreen;" : "";
    ui->measureButton->setStyleSheet(style);
// Доступность кнопки:
    ui->measureButton->setEnabled(isReady);
}

void MainWindow::handleDeviceError(const QString& errorMessage){
    statusBar()->showMessage("Status: " + errorMessage, 1000);
    //ui->measureButton->setEnabled(false);
}





