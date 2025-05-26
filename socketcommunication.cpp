#include "socketcommunication.h"
#include <QHostAddress>

SocketCommunication::SocketCommunication(QObject *parent): ICommunication(parent),
    thread(new QThread())
{
    QMetaObject::invokeMethod(thread.get(), [this]() {
        socket.reset(new QTcpSocket());
        pollTimer.reset(new QTimer());

        // Настройка соединения - периодический опрос
        connect(pollTimer.get(), &QTimer::timeout, this, [this]() {
            socket->connectToHost(targetAddress, port);
            socket->waitForConnected(500);
        }, Qt::QueuedConnection);

        connect(socket.get(), &QTcpSocket::connected, this, &SocketCommunication::onConnected);
        connect(socket.get(), &QTcpSocket::readyRead, this, &SocketCommunication::onReadyRead);
        connect(socket.get(), QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
                this, &SocketCommunication::onError);

        startPolling();
    });
    thread->start();
}

SocketCommunication::~SocketCommunication(){
    thread->quit();
    if (!thread->wait(100)) {
        thread->terminate();
    }
}

//==================================================================//
//                  ОТПРАВКА КОМАНД [SCPI] --> S2VNA                //
//==================================================================//
int SocketCommunication::sendCommand(const QString &command)
{
//Проверка сетевого подключения:
    if (!socket || socket->state() != QAbstractSocket::ConnectedState){
        emit errorOccurred("Device is not connected");
        return 1;
    }
//Отправка команды --> S2VNA
    QByteArray scpi_cmd = command.toUtf8();

    auto result = socket->write(scpi_cmd);
    if (result == -1 || !socket->waitForBytesWritten(1000)){
        emit errorOccurred("Failed to send command");
        return 1;
    }
    return 0;
}

void SocketCommunication::connectToDevice(){
// Здесь будут разовые действия при самом начале подключения:
    sendCommand("*RST\n");
}

//========================== СОКЕТНЫЕ ОБРАБОТЧИКИ ===========================//
void SocketCommunication::onConnected(){   /* успешное подключение */
    stopPolling();
    isExpectingIDN = true;
 //Отправка команды идиентификации:
    sendCommand("*IDN?\n");

    emit deviceStatusChanged(true);
}

//==================================================================//
//                      ПРИНЯТИЕ ДАННЫХ                             //
//==================================================================//
void SocketCommunication::onReadyRead(){     /* Готовность чтения[прием данных] */
    QString response;
    responseBuffer += socket->readAll();
    qDebug() << "Response: " << responseBuffer;
    // Проверка на завершенность ответа - [\n]
    if (responseBuffer.endsWith('\n')) {
         // флаг isExpectingIDN? true -> парсинг IDN?\n /сигнал на вывод в QLabel вглавном потоке
        if(isExpectingIDN){
            response = QString::fromUtf8(responseBuffer).trimmed();
            isExpectingIDN = false;

            emit idnReceived(response);
        } //Пришли другие данные:
        else{
            // Пришли зависимости S-параметров:
            response = QString::fromUtf8(responseBuffer).trimmed();
            if(!response.isEmpty()) {
                emit sParamsReceived(response);
            }
        }
        responseBuffer.clear();
    }
    else{
       //сообщение неоконченно, требуется кэширование:

       //а может и просто ошибка:
        emit errorOccurred("Incorrect response data");
    }

}

void SocketCommunication::onError(){ /* Ошибка подключения */
    isDeviceReady = false;
    emit errorOccurred(socket->errorString());
    emit deviceStatusChanged(false);
}

//Отправка валидных UI данных:
void SocketCommunication::accept_measure_config(const QString &command)
{   /* Вх. данные - упакованная scpi команда */
    if (command.isEmpty()) {
        emit errorOccurred("communication data error");
        return;
    }
//Парсинг на части -> отправка
    const auto multiple_parts = command.split(';');
    for (const auto &part : multiple_parts){
        sendCommand(part.trimmed());
    }
}

//Отправка валидных UI настроек модулю связи:
void SocketCommunication::accept_setting_config(const Settings &setting)
{
    stopPolling();
    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->disconnectFromHost();
    }
    if(setting.network.ip_addr.isEmpty() || setting.network.port == 0){
         emit errorOccurred("Settings update error");
    }
    // Обновление параметров подключения:
    targetAddress.setAddress(setting.network.ip_addr);
    port = setting.network.port;

    if (targetAddress.isNull()) {
        emit errorOccurred("Error IP address update");
        return;
    }
    startPolling();
}

//==================================================================//
//                  ОПРОС ТАЙМЕРА О СОСТОЯНИИ ХОСТА                 //
//==================================================================//
void SocketCommunication::startPolling(){
    QMetaObject::invokeMethod(pollTimer.get(), [this]() {
        pollTimer->start(500); // Опрос каждую секунду
    }, Qt::QueuedConnection);
}

void SocketCommunication::stopPolling(){
    QMetaObject::invokeMethod(pollTimer.get(), &QTimer::stop);
}










