#include "socketcommunication.h"
#include <QHostAddress>

SocketCommunication::SocketCommunication(QObject *parent)
    : ICommunication(parent),
    socket(make_unique<QTcpSocket>()),
    thread(make_unique<QThread>())
{
//выделение потока под связь с хостом:
    socket->moveToThread(thread.get());

    connect(socket.get(), &QTcpSocket::connected, this, &SocketCommunication::onConnected);
    connect(socket.get(), &QTcpSocket::readyRead, this, &SocketCommunication::onReadyRead);

    connect(socket.get(), QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
            this, &SocketCommunication::onError);

    thread->start();
}


void SocketCommunication::onConnected()
{
    emit errorOccurred("Connected to S2VNA");
}

void SocketCommunication::onReadyRead()
{

}

void SocketCommunication::onError(QAbstractSocket::SocketError socketError)
{
    emit errorOccurred(socket->errorString());
}


