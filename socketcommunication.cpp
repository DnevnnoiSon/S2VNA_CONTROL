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
//                  ОТПРАВКА КОМАНД [SCPI] --> S2VNA                //
//==================================================================//
int SocketCommunication::sendScpiCommand(const QString &command)
{
    //Упаковка под SCPI формат:

    //Отправка команды --> S2VNA

    //return 1 - ошибка
    return 0;
}

void SocketCommunication::connectToDevice()
{
// Здесь будут разовые действия при самом начале подключения:
}

//========================== СОКЕТНЫЕ ОБРАБОТЧИКИ ===========================//
void SocketCommunication::onConnected() {   /* успешное подключение */
    stopPolling();
 //Отправка команды идиентификации:
    sendScpiCommand("*IDN?\n");

    emit deviceStatusChanged(true);

}

//==================================================================//
//                      ПРИНЯТИЕ ДАННЫХ                             //
//==================================================================//
void SocketCommunication::onReadyRead()     /* Готовность чтения[прием данных] */
{
    responseBuffer += socket->readAll();
    // Проверка на завершенность ответа - [\n]
    if (responseBuffer.endsWith('\n')) {
        //пришло завершенное сообщение:
    }
}

void SocketCommunication::onError() /* Ошибка подключения */
{
    emit errorOccurred(socket->errorString());
    emit deviceStatusChanged(false);
}

//==================================================================//
//                  ОПРОС ТАЙМЕРА О СОСТОЯНИИ ХОСТА                 //
//==================================================================//
void SocketCommunication::startPolling() {
    QMetaObject::invokeMethod(pollTimer.get(), [this]() {
        pollTimer->start(1000); // Опрос каждую секунду
    }, Qt::QueuedConnection);
}

void SocketCommunication::stopPolling() {
    QMetaObject::invokeMethod(pollTimer.get(), &QTimer::stop);
}


//========== ПЕРЕДАЧА ДАННЫХ [GUI поток --> S2VNA поток] =============//
/*void setMeasurementConfig(const MeasurementConfig& config)
{

}
*/







