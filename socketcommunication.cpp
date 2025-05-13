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
            socket->connectToHost(QHostAddress::LocalHost, 5025);
            socket->waitForConnected(100);
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
//          ПЕРЕДАЧА ДАННЫХ [GUI поток --> S2VNA поток]             //
//==================================================================//
// void SocketCommunication::setMeasurementConfig(const MeasurementConfig &config)
// {    /* Перед этим: Передача данных из основного потока --> сокетному потоку */

// //Завертка данных под scpi[отдельный scpi модуль]
//     QStringList commands; /* = scpi конвертирующий модуль */
// //Отправка
//     for (const QString &command_el : commands) {
//         if (sendCommand(command_el) != 0) {
//             break;
//         }
//     }
// }

//==================================================================//
//                  ОТПРАВКА КОМАНД [SCPI] --> S2VNA                //
//==================================================================//
int SocketCommunication::sendCommand(const QString &command)
{   /* Сюда поступает уже упакованная scpi команда */

//Проверка сетевого подключения:
    if (!socket || socket->state() != QAbstractSocket::ConnectedState){
        emit errorOccurred("Device is not connected");
        return 1;
    }
//Отправка команды --> S2VNA
    QByteArray scpi_cmd = command.toUtf8();

    auto result = socket->write(scpi_cmd);
    if (result == -1 || !socket->waitForBytesWritten(500)){
        emit errorOccurred("Failed to send command");
        return 1;
    }
    return 0;
}

void SocketCommunication::connectToDevice(){
// Здесь будут разовые действия при самом начале подключения:
}

//========================== СОКЕТНЫЕ ОБРАБОТЧИКИ ===========================//
void SocketCommunication::onConnected(){   /* успешное подключение */
    stopPolling();

 //Отправка команды идиентификации:
    sendCommand("*IDN?\n");

    emit deviceStatusChanged(true);
}

//==================================================================//
//                      ПРИНЯТИЕ ДАННЫХ                             //
//==================================================================//
void SocketCommunication::onReadyRead(){     /* Готовность чтения[прием данных] */
    responseBuffer += socket->readAll();
    // Проверка на завершенность ответа - [\n]
    if (responseBuffer.endsWith('\n')) {
        //пришло завершенное сообщение:
    }
}

void SocketCommunication::onError(){ /* Ошибка подключения */
    emit errorOccurred(socket->errorString());
    emit deviceStatusChanged(false);
}

void SocketCommunication::accept_measurement_config(const QString &command){
    if (!command.isEmpty()) {
//потоковая отправка данных: / отправка одной большой команды:
    }
}

//==================================================================//
//                  ОПРОС ТАЙМЕРА О СОСТОЯНИИ ХОСТА                 //
//==================================================================//
void SocketCommunication::startPolling(){
    QMetaObject::invokeMethod(pollTimer.get(), [this]() {
        pollTimer->start(1000); // Опрос каждую секунду
    }, Qt::QueuedConnection);
}

void SocketCommunication::stopPolling(){
    QMetaObject::invokeMethod(pollTimer.get(), &QTimer::stop);
}










