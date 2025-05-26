#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "socketcommunication.h"

#define MAX_POWER 60

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    InitUI();           // Создание виджетов
    ApplyStyles();      // Стилизация
    SetupConnections(); // Сигналы/слоты
}

//================ Создание виджетов ===============//
void MainWindow::InitUI(){
    // Подключение к хосту --> авто-пробуждение S2VNA потока
    communicator = new SocketCommunication(this);
    //График ВАЦ [полученные с S2VNA параметры]
    plotter = new SParameterPlotter(this);
    ui->frame_6->setLayout(new QVBoxLayout());
    ui->frame_6->layout()->addWidget(plotter);
    ui->frame_6->layout()->setContentsMargins(0, 0, 0, 0);
}

//========================Установка стилей ==========================//
void MainWindow::ApplyStyles()
{
    //Метки:
    ui->modelLabel->setStyleSheet("color: green; font-weight: bold;");
    ui->vendorLabel->setStyleSheet("color: green; font-weight: bold;");
    ui->modelLabel->clear();
    ui->vendorLabel->clear();

    // Кнопки:
    ui->decoreButton->setFlat(true);
    ui->decoreButton->setStyleSheet("background: transparent; border: none;");
    ui->settingButton->setFlat(true);
    ui->settingButton->setStyleSheet("background: transparent; border: none;");

    // Cетевые настройки по умолчанию:
    ui->ipLineEdit->setText("127.0.0.1");
    ui->portSpinBox->setValue(5025);

    //Кастомизация:
    ui->portSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
}

//================== Настройка сигналов и слотов=====================//
void MainWindow::SetupConnections(){
    // Нажатие Измерить --> Валидация даннных конфигурации ВАЦ
//    connect(ui->measureButton, &QPushButton::clicked, this, &MainWindow::on_measureButton_clicked);
    // Нажатие Обновить --> Валидация настроек сетевого подключения
//    connect(ui->updateButton,&QPushButton::clicked, this, &MainWindow::on_updateButton_clicked);
    // Изменение статуса хоста --> Кнопка измерить в зеленный цвет
    connect(communicator, &ICommunication::deviceStatusChanged,
    this, &MainWindow::onDeviceStatusChanged, Qt::QueuedConnection);

    // Сигнал сокетной ошибки --> Обработка ошибки
    connect(communicator, &ICommunication::errorOccurred,
    this, &MainWindow::handleDeviceError, Qt::QueuedConnection);

    // Передача валидных данных --> в поток связи
    connect(this, &MainWindow::transfer_measure_config,
    communicator, &ICommunication::accept_measure_config, Qt::QueuedConnection);

    // Передача сетевых настроек --> в поток связи
    connect(this, &MainWindow::transfer_setting_config,
    communicator, &ICommunication::accept_setting_config, Qt::QueuedConnection);

    // Ответ от IDN? --> Обработка/Вывод на экран
    connect(communicator, &ICommunication::idnReceived,
    this, &MainWindow::handleIdnResponse, Qt::QueuedConnection);

    // Пришли данные sParams --> Вывод в график
    connect(communicator, &SocketCommunication::sParamsReceived,
    plotter, &SParameterPlotter::updateChart, Qt::QueuedConnection);
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::on_measureButton_clicked()
{
    if( (ui->startSpinBox->value() >= ui->endSpinBox->value()) ||
        (ui->powerSpinBox->value() > MAX_POWER) )
    {
        handleDeviceError("Incorrect valid parameters");
    }
    if (!ui->cense1Button->isChecked() && !ui->cense2Button->isChecked()) {
        handleDeviceError("Incorrect valid cense");
        return;
    }
    // Мега => 1 лям:
    double coeficent = 1000000;
//Ввалидные данные:
    QVariantMap config{
    //СОГЛАШЕНИЕ МОЕГО CONFIG КОНТЕЙНЕРА: если параметра нет - 0;
        {"SENSe :FREQuency:STARt", (ui->startSpinBox->value() * coeficent) },  // Начальная частота
        {"SENSe :FREQuency:STOP",  (ui->endSpinBox->value() * coeficent) },  // Конечная частота
        {"SENSe :SWEep:POINts", ui->stepspinBox->value() },     // Кол-во точек
        {"SOURce :POWer",          ui->powerSpinBox->value()},  // Мощность
        {"CALCulate :DATA:SDATa?", 0},  //Запрос на считывания -> [SS,частоты]..
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

void MainWindow::on_updateButton_clicked(){
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
    ui->modelLabel->clear();
    ui->vendorLabel->clear();

    statusBar()->showMessage("Status: " + errorMessage, 600);
    //ui->measureButton->setEnabled(false);
}

void MainWindow::handleIdnResponse(const QString &idnInfo) {

// Текущий ответ от устройства: "Planar, C1209, , 25.1.1/1"
    QStringList parts = idnInfo.split(',');
    if (parts.size() >= 4){
        ui->modelLabel->setText(parts[1].trimmed());
        ui->vendorLabel->setText( parts[0].trimmed());
    } else {
        ui->modelLabel->setText("Unknown device");
        ui->vendorLabel->setText("Unknown vendor");
    }
}


