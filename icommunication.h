#ifndef ICOMMUNICATION_H
#define ICOMMUNICATION_H

#include <QObject>
#include <QVector>

#include "MeasurementConfig.h"

//Модуль связи с хостом:
class ICommunication : public QObject {
    Q_OBJECT
public:
    explicit ICommunication(QObject *parent = nullptr) : QObject(parent) {}

    virtual void connectToDevice() = 0;
    virtual int sendCommand(const QString &command) = 0;

// При подключении к хосту:
    virtual void startPolling() = 0;  // Запуск опроса состояния
    virtual void stopPolling() = 0;   // Остановка опроса состояния
public slots:
    virtual void accept_measurement_config(const QString& command) = 0;
protected:
    MeasurementConfig measurement_config;
signals:
    void deviceStatusChanged(bool isReady);
    void errorOccurred(const QString& message);
};

#endif // ICOMMUNICATION_H
