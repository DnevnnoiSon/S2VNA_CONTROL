#ifndef SOCKETCOMMUNICATION_H
#define SOCKETCOMMUNICATION_H

#include "icommunication.h"
#include <QTcpSocket>
#include <QThread>
#include <memory>
//Пользовательское определение связи с хостом - сокеты
using namespace std;

class SocketCommunication : public ICommunication
{
    Q_OBJECT
public:
    explicit SocketCommunication(QObject *parent = nullptr);
//    void connectToDevice() override;
//    void sendMeasurementParams(MeasurementConfig &measurement_config) override;
//    void getMeasurementParams() override;

private slots:
    void onConnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError);

private:

    unique_ptr<QTcpSocket> socket;
    unique_ptr<QThread> thread;
};

#endif // SOCKETCOMMUNICATION_H
