#ifndef SOCKETCOMMUNICATION_H
#define SOCKETCOMMUNICATION_H

#include "icommunication.h"
#include <QTcpSocket>
#include <QThread>
#include <QTimer>
#include <memory>
//Пользовательское определение связи с хостом - сокеты
using namespace std;

class SocketCommunication : public ICommunication
{
    Q_OBJECT
public:
    explicit SocketCommunication(QObject *parent = nullptr);
    ~SocketCommunication();

    int sendScpiCommand(const QString &command) override;
   // void setMeasurementConfig(const MeasurementConfig& config) override;

    void connectToDevice() override;
// При подключении к хосту: [периодическая идиентификация -> pollTimer]
    void startPolling() override;
    void stopPolling() override;

private slots:
    void onReadyRead();
    void onConnected();
    void onError();

private:
//Буффер для накопления принятых данных:
    QByteArray responseBuffer;

    unique_ptr<QTcpSocket> socket;
    unique_ptr<QThread> thread;

//Периодический опрос состояния:
    unique_ptr<QTimer> pollTimer;  // Таймер для периодического опроса [идиентификация]
    bool isDeviceReady = false;    // Флаг готовности устройства [сокетное подключение]
};

#endif // SOCKETCOMMUNICATION_H
