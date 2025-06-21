#ifndef SOCKETCOMMUNICATION_H
#define SOCKETCOMMUNICATION_H

#include "icommunication.h"

#include <QTcpSocket>
#include <QThread>
#include <QTimer>
#include <memory>

/**
 * @brief Реализация интерфейса ICommunication для связи через TCP/IP сокеты.
 * @details Управляет подключением, отправкой/приемом данных и переподключением
 * в случае потери связи с хостом.
 */
class SocketCommunication : public ICommunication
{
    Q_OBJECT
public:
    /**
    * @brief Конструктор класса.
    * @param parent Родительский объект QObject.
    */
    explicit SocketCommunication(QObject *parent = nullptr);

    /**
    * @brief Деструктор - (неактивен).
    */
    ~SocketCommunication();

    /**
    * @brief @copydoc ICommunication::sendCommand.
    * @param Отправляемая команда.
    * @return Статус отправки 0 - успешно, 1 - socket ошибка.
    */
    int sendCommand(const QString &command) override;


    /**
    * @brief @copydoc ICommunication::connectToDevice
    */
    void connectToDevice() override;


    /**
    * @brief @copydoc ICommunication::startPolling
    */
    void startPolling() override;

    /**
    * @brief @copydoc ICommunication::stopPolling
    */
    void stopPolling() override;
public slots:
    /**
    * @brief @copydoc ICommunication::acceptMeasureConfig
    */
    void acceptMeasureConfig(const QString &command) override;

    /**
    * @brief @copydoc ICommunication::acceptSettingConfig
    */
    void acceptSettingConfig(const ConnectionSettings &setting) override;
private slots:
    /**
    * @brief Слот, вызываемый при готовности сокета к чтению данных.
    */
    void onReadyRead();

    /**
    * @brief Слот, вызываемый при успешном установлении соединения.
    */
    void onConnected();

    /**
    * @brief Слот, вызываемый при разрыве соединения.
    */
    void onError();

private:
    int port = 5025; ///< Настройка соединения по умолчанию.
    QHostAddress targetAddress = QHostAddress::LocalHost; ///< Настройка соединения по умолчанию.

    QByteArray responseBuffer; ///< Буффер для накопления принятых данных.

    std::unique_ptr<QTcpSocket> socket; ///< Умный указатель на TCP сокет.
    std::unique_ptr<QThread> thread;

    //Периодический опрос состояния:
    std::unique_ptr<QTimer> pollTimer; ///< Таймер для периодических попыток переподключения.
    bool isDeviceReady = false;    ///< Флаг готовности устройства [ сокетное подключение ]
    bool isExpectingIDN = false;   ///< Флаг, указывающий на ожидание ответа *IDN?.
};

#endif // SOCKETCOMMUNICATION_H

