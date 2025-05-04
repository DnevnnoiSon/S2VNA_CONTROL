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
    virtual void sendMeasurementParams(MeasurementConfig &measurement_config) = 0;
    virtual void getMeasurementParams(void) =  0;

    MeasurementConfig measurement_config;
signals:
    void dataReceived(const QVector<double>& frequencies, const QVector<double>& sParams);
    void errorOccurred(const QString& message);
};

#endif // ICOMMUNICATION_H
