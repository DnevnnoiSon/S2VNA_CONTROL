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
 //фрейм над графико:
    ui->frame_5->setStyleSheet
    ("background-color: rgb(35,35,35);"
     "border: 1px solid rgba(255, 255, 255, 100);"
     "border-radius: 8px;");               //метка планара:
    ui->planarButton->setStyleSheet("border: 2px solid transparent;");

    //Метки:
    ui->modelLabel->setStyleSheet("color: green; font-weight: bold;");
    ui->vendorLabel->setStyleSheet("color: green; font-weight: bold;");
    ui->modelLabel->clear();
    ui->vendorLabel->clear();

    // Кнопки:
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
    // Нажатие Обновить --> Валидация настроек сетевого подключения

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
    connect(communicator, &ICommunication::sParamsReceived,
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
        return;
    }
    if(ui->pointspinBox->value() <= 1){
        handleDeviceError("Incorrect valid parameters");
        return;
    }
    double coeficent = 1e6; // Мега => 1 лям:
//Ввалидные данные:
    QList<QPair<QString, QVariant>> config {
    //СОГЛАШЕНИЕ МОЕГО CONFIG КОНТЕЙНЕРА: если параметра нет - 0;
        {"SENSe:FREQuency:STARt", (ui->startSpinBox->value() * coeficent) },  // Начальная частота
        {"SENSe:FREQuency:STOP",  (ui->endSpinBox->value() * coeficent) },  // Конечная частота
        {"SENSe:SWEep:POINts", ui->pointspinBox->value() },     // Кол-во точек
        {"SOURce:POWer",          ui->powerSpinBox->value()},  // Мощность
        {"CALCulate:DATA:SDATa?", 0},  //Запрос на считывания -> [SS,частоты]..
    };

//Передача в сокетный поток для отправки:
    QString command = scpi.generateCommand(config); //= конвертация
//Формирование шага частоты, заполнение шагом
    scpi.Frequency.clear();
    scpi.Frequency.reserve(ui->pointspinBox->value());

    double frequency_step = (( ui->endSpinBox->value() - ui->startSpinBox->value() ) * coeficent) / (ui->pointspinBox->value() - 1);
    double frequency_el = (ui->startSpinBox->value() * coeficent);

    for(int index = 0; index < ui->pointspinBox->value(); index++){
        scpi.Frequency.append(frequency_el);
        frequency_el += frequency_step;
    }

    qWarning() << "Frequency points: " << scpi.Frequency.size();
    // Передача частоты для последующей отрисовки на графике
    plotter->setFrequencyData(scpi.Frequency);

    // Передача команд для последующей отправки
    emit transfer_measure_config(command);
}

void MainWindow::on_updateButton_clicked(){
    ConnectionSettings setting;
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
    ui->loadButton->setEnabled(isReady);
    ui->saveButton->setEnabled(isReady);
    if(!isReady){
        ui->modelLabel->clear();
        ui->vendorLabel->clear();
    }
}

void MainWindow::handleDeviceError(const QString& errorMessage){
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


