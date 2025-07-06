#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "socketcommunication.h"
#include "filecache.h"
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
    m_commThread(new QThread(this)),
    m_cacheThread(new QThread(this)),
    m_cacheModel(new QStandardItemModel(this))
{
    ui->setupUi(this);

    this->setWindowTitle("S2VNA CONTROL");

    InitUI();            // Создание виджетов
    setupUiAppearance(); // Стилизация
    setupConnections();  // Сигналы/слоты

    // Запуск дочернего потока - поток кэширования [файловое управление кэшем]
    m_cacheThread->start();
    // Вызов внутри дочернего потока
    QMetaObject::invokeMethod(m_fileCache.get(),
    &FileCache::getCacheFromResources, Qt::QueuedConnection);

    // Запуск дочернего потока - поток связи
    m_commThread->start();
    // Вызов внутри дочернего потока
    QMetaObject::invokeMethod(static_cast<SocketCommunication*>(m_communicator.get()),
    &SocketCommunication::initialize);

/// [ Под указателем на интерфейс ICommunication -> объект класса SocketCommunication ]
}

MainWindow::~MainWindow(){
    // Посылаю сигнал потокам на завершение их циклов событий
    m_commThread->quit();
    m_cacheThread->quit();

    // Ожидание, пока потоки обработают все события в очереди
    m_commThread->wait();
    m_cacheThread->wait();
}

//================ Создание виджетов ===============//
void MainWindow::InitUI()
{
    m_communicator = std::make_unique<SocketCommunication>();
    m_communicator->moveToThread(m_commThread);

    m_fileCache = std::make_unique<FileCache>();
    m_fileCache->moveToThread(m_cacheThread);

    m_plotter = new SParameterPlotter(this);
    ui->frame_6->setLayout(new QVBoxLayout());
    ui->frame_6->layout()->addWidget(m_plotter);
    ui->frame_6->layout()->setContentsMargins(0, 0, 0, 0);
}

//========================Установка стилей ==========================//
void MainWindow::setupUiAppearance(){}

//================== Настройка сигналов и слотов=====================//
void MainWindow::setupConnections(){

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

    // Пришли сырые данные sParams --> Вывод в график
    connect(m_communicator.get(), &ICommunication::sParamsReceived,
            m_plotter, &SParameterPlotter::updateChart, Qt::QueuedConnection);

    // Данные были сформированы --> Занести в кэш перед построением
    connect(m_plotter, &SParameterPlotter::CacheReady,
            m_fileCache.get(), &FileCache::saveDataToCache, Qt::QueuedConnection);

    // Данные были занесены в кэш --> Обновление панели UI: QListView
    connect(m_fileCache.get(), &FileCache::CacheUpdate,
            this, &MainWindow::updateCacheListView, Qt::QueuedConnection);

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
        {"SENSe:SWEep:POINts",    points },
        {"SOURce:POWer",          power},
        {"SENSe:FREQuency:STARt", startFreq * MEGAHERTZ_MULTIPLIER },
        {"SENSe:FREQuency:STOP",  endFreq * MEGAHERTZ_MULTIPLIER },
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

void MainWindow::updateCacheListView(const QQueue<QString> &cachedFiles) {
    ui->historylistWidget->clear();

    // 2. Проходим по актуальному списку файлов в кэше.
    for (const QString &filePath : cachedFiles) {
        QFileInfo fileInfo(filePath);
        QString fileName = fileInfo.fileName();

        // 3. Создаем кнопку и задаем ей имя файла в качестве текста.
        QPushButton *button = new QPushButton(fileName, this);

        button->setFixedHeight(30);

        // 4. Создаем элемент списка (контейнер для нашей кнопки).
        QListWidgetItem *item = new QListWidgetItem();

        // 5. Добавляем элемент-контейнер в QListWidget.
        ui->historylistWidget->addItem(item);

        // 6. Устанавливаем нашу кнопку в качестве виджета для этого элемента.
        // QListWidget автоматически управляет размером и положением кнопки.
        item->setSizeHint(button->sizeHint() / 2); // Задаем элементу размер кнопки
        ui->historylistWidget->setItemWidget(item, button);

    }

}
