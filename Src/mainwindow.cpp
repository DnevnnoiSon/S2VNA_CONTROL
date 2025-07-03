#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "socketcommunication.h"
#include "connectionSettings.h"
#include "sparameterplotter.h"
#include "s2vna_scpi.h"

#include <QThread>
#include <QVector>
#include <numeric>

static constexpr int MAX_POWER = 60;
static constexpr double MEGAHERTZ_MULTIPLIER = 1e6;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_commThread(new QThread(this))
{
    ui->setupUi(this);

    this->setWindowTitle("S2VNA CONTROL");

    InitUI();            // Создание виджетов
    setupUiAppearance(); // Стилизация
    setupConnections();  // Сигналы/слоты

    // Запуск дочернего потока - поток связи
    m_commThread->start();
    // Вызов внутри дочернего потока

    QMetaObject::invokeMethod(static_cast<SocketCommunication*>(m_communicator.get()),
    &SocketCommunication::initialize);
/// [ Под указателем на интерфейс ICommunication -> объект класса SocketCommunication ]
}

MainWindow::~MainWindow(){
    // Завершение дочернего потока:
    m_commThread->quit();
    if (!m_commThread->wait(3000)) {
        m_commThread->terminate();
        m_commThread->wait();
    }
}

//================ Создание виджетов ===============//
void MainWindow::InitUI()
{
    auto communicator = std::make_unique<SocketCommunication>();

    m_communicator.reset(communicator.release());
    m_communicator->moveToThread(m_commThread);

    m_plotter = new SParameterPlotter(this);
    ui->frame_6->setLayout(new QVBoxLayout());
    ui->frame_6->layout()->addWidget(m_plotter);
    ui->frame_6->layout()->setContentsMargins(0, 0, 0, 0);
}

//========================Установка стилей ==========================//
void MainWindow::setupUiAppearance(){}

//================== Настройка сигналов и слотов=====================//
void MainWindow::setupConnections(){
    // Нажатие Измерить --> Валидация даннных конфигурации ВАЦ
    //connect(ui->measureButton, &QPushButton::clicked, this, &MainWindow::on_measureButton_clicked);
    // Нажатие Обновить --> Валидация настроек сетевого подключения
    //connect(ui->updateButton,&QPushButton::clicked, this, &MainWindow::on_updateButton_clicked);

    // Завершения потока --> удаление объекта связи
    connect(m_commThread, &QThread::finished, m_communicator.get(), &QObject::deleteLater);

    // Изменение статуса хоста --> Кнопка измерить в зеленный цвет
    connect(m_communicator.get(), &ICommunication::deviceStatusChanged,
            this, &MainWindow::onDeviceStatusChanged, Qt::QueuedConnection);

    // Сигнал сокетной ошибки --> Обработка ошибки
    connect(m_communicator.get(), &ICommunication::errorOccurred,
            this, &MainWindow::handleDeviceError, Qt::QueuedConnection);

    // Передача валидных данных --> в поток связи
    connect(this, &MainWindow::measureConfigTransferred,
            m_communicator.get(), &ICommunication::acceptMeasureConfig, Qt::QueuedConnection);

    // Передача сетевых настроек --> в поток связи
    connect(this, &MainWindow::settingConfigTransferred,
            m_communicator.get(), &ICommunication::acceptSettingConfig, Qt::QueuedConnection);

    // Ответ от IDN? --> Обработка/Вывод на экран
    connect(m_communicator.get(), &ICommunication::idnReceived,
            this, &MainWindow::handleIdnResponse, Qt::QueuedConnection);

    // Пришли данные sParams --> Вывод в график
    connect(m_communicator.get(), &ICommunication::sParamsReceived,
            m_plotter, &SParameterPlotter::updateChart, Qt::QueuedConnection);
}

void MainWindow::on_measureButton_clicked()
{
    const auto [startFreq, endFreq, points, power] = std::make_tuple(
        ui->startSpinBox->value(),
        ui->endSpinBox->value(),
        ui->pointspinBox->value(),
        ui->powerSpinBox->value()
        );

    if (startFreq >= endFreq || power > MAX_POWER || points <= 1) {
        handleDeviceError("Некорректные параметры измерения");
        return;
    }

    // Ввалидные данные:
    QList<QPair<QString, QVariant>> config {
        /// Соглашение CONFIG контейнера для удобства:
        /// если параметра нет - 0;
        {"SENSe:FREQuency:STARt", startFreq * MEGAHERTZ_MULTIPLIER },
        {"SENSe:FREQuency:STOP",  endFreq * MEGAHERTZ_MULTIPLIER },
        {"SENSe:SWEep:POINts",    points },
        {"SOURce:POWer",          power},
        {"CALCulate:DATA:SDATa?", 0},  // Запрос на считывания -> [SS,частоты]..
    };

    //Передача в сокетный поток для отправки:
    QString command = m_scpi.generateCommand(config); //= конвертация
    //Формирование шага частоты, заполнение шагом
    QVector<double> frequencies(points);

    const double startFreqHz = startFreq * MEGAHERTZ_MULTIPLIER;
    const double endFreqHz = endFreq * MEGAHERTZ_MULTIPLIER;
    const double freqStep = (points > 1) ? (endFreqHz - startFreqHz) / (points - 1) : 0;

    // равномерное распределение в диапозоне:
    std::iota(frequencies.begin(), frequencies.end(), 0);
    for (double& freq : frequencies) {
        freq = startFreqHz + freq * freqStep;
    }

    qWarning() << "Количество точек частоты: " << frequencies.size();
    // Передача команд для построения графика:
    m_plotter->setFrequencyData(frequencies);

    // Передача сырой последовательности команд scpi билдеру
    emit measureConfigTransferred(command);
}

void MainWindow::on_updateButton_clicked(){
    ConnectionSettings setting;
    //Настройка сетевого подключения:
    setting.network.ip_addr = ui->ipLineEdit->text();
    setting.network.port = ui->portSpinBox->value();

    emit settingConfigTransferred(setting);
}

void MainWindow::onDeviceStatusChanged(bool isReady){
    // Смена цвета:
    QString style = isReady ? "background-color: darkgreen;" : "";
    ui->measureButton->setStyleSheet(style);
    // Доступность кнопки:
    ui->measureButton->setEnabled(isReady);
    if(!isReady){
        ui->modelLabel->clear();
        ui->vendorLabel->clear();
    }
}

void MainWindow::handleDeviceError(const QString& errorMessage){
    statusBar()->showMessage("Статус: " + errorMessage, 1000);
}

void MainWindow::handleIdnResponse(const QString &idnInfo) {

    // Текущий ответ от устройства: "Planar, C1209, , 25.1.1/1"
    QStringList parts = idnInfo.split(',');
    if (parts.size() >= 4){
        ui->modelLabel->setText(parts[1].trimmed());
        ui->vendorLabel->setText( parts[0].trimmed());
    } else {
        ui->modelLabel->setText("Неизвестно");
        ui->vendorLabel->setText("Неизвестно");
    }
}
