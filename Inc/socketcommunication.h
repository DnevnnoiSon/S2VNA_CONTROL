#ifndef SOCKETCOMMUNICATION_H
#define SOCKETCOMMUNICATION_H

#include "icommunication.h"

#include <QTcpSocket>
#include <QThread>
#include <QTimer>
#include <memory>
//==================================================================//
//    Модуль связи с хостом через сокеты [S2VNA поток].             //
//    Входным параметром является сконвертированная в scpi команда. //
//==================================================================//
using namespace std;

class SocketCommunication : public ICommunication
{
    Q_OBJECT
public:
    explicit SocketCommunication(QObject *parent = nullptr);
    ~SocketCommunication();

    int sendCommand(const QString &command) override;

    void connectToDevice() override;
// При подключении к хосту: [периодическая идиентификация -> pollTimer]
    void startPolling() override;
    void stopPolling() override;
public slots:
//принятие валидных данных с GUI потока:
    void accept_measure_config(const QString &command) override;
    void accept_setting_config(const Settings &setting) override;
private slots:
    void onReadyRead();
    void onConnected();
    void onError();

private:
//Настройка соединения по умолчанию:
    int port = 5025;
    QHostAddress targetAddress = QHostAddress::LocalHost;
//Буффер для накопления принятых данных:
    QByteArray responseBuffer;

    unique_ptr<QTcpSocket> socket;
    unique_ptr<QThread> thread;

//Периодический опрос состояния:
    unique_ptr<QTimer> pollTimer;  // Таймер для периодического опроса [идиентификация]
    bool isDeviceReady = false;    // Флаг готовности устройства [сокетное подключение]
    bool isExpectingIDN = false;   // Флаг ожидания ответа от  *IDN?
};

#endif // SOCKETCOMMUNICATION_H


