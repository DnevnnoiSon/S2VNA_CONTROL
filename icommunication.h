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
    virtual int sendScpiCommand(const QString &command) = 0;
   // virtual void setMeasurementConfig(const MeasurementConfig& config) = 0; // передача введенных данных
    // в будущем добавить функцию обработки

// При подключении к хосту:
    virtual void startPolling() = 0;  // Запуск опроса состояния
    virtual void stopPolling() = 0;   // Остановка опроса состояния
protected:
    MeasurementConfig measurement_config;
signals:
    void deviceStatusChanged(bool isReady);
    void errorOccurred(const QString& message);
};

#endif // ICOMMUNICATION_H
