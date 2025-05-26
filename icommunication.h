#ifndef ICOMMUNICATION_H
#define ICOMMUNICATION_H

#include <QObject>
#include <QVector>

#include "ValidSettings.h"

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
// Передача валидных данных модулю связи:
    virtual void accept_measure_config(const QString& command) = 0;
    virtual void accept_setting_config(  const Settings &setting) = 0;
signals:
    void deviceStatusChanged(bool isReady);
    void errorOccurred(const QString& message);
//Прием информации:
    void idnReceived(const QString &idnInfo);
    void sParamsReceived(const QString &response);
};

#endif // ICOMMUNICATION_H
