#ifndef SOCKETCOMMUNICATION_H
#define SOCKETCOMMUNICATION_H

#include "icommunication.h"
#include <QTcpSocket>
#include <QTimer>
#include <memory>

/**
 * @brief Реализация интерфейса ICommunication для связи через TCP/IP сокеты.
 * @details Управляет подключением, отправкой/приемом данных и переподключением
 * в случае потери связи с хостом.
 */
class SocketCommunication : public ICommunication {
    Q_OBJECT
public:
    /**
     * @brief Конструктор класса.
     * @param parent Родительский объект QObject.
     */
    explicit SocketCommunication(QObject *parent = nullptr);

    /**
     * @brief Деструктор.
     */
    ~SocketCommunication() override;

    /**
     * @brief @copydoc ICommunication::sendCommand
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
    void onDisconnected();

    /**
     * @brief Слот для обработки ошибок сокета.
     * @param socketError Код ошибки сокета.
     */
    void onErrorOccurred(QAbstractSocket::SocketError socketError);

    /**
     * @brief Выполняет попытку подключения к хосту.
     */
    void attemptConnection();

private:
    ConnectionSettings m_settings; ///< Текущие настройки подключения.

    std::unique_ptr<QTcpSocket> m_socket; ///< Умный указатель на TCP сокет.
    QByteArray m_responseBuffer;          ///< Буфер для накопления данных из сокета.

    std::unique_ptr<QTimer> m_pollTimer; ///< Таймер для периодических попыток переподключения.

    bool m_isExpectingIDN = false; ///< Флаг, указывающий на ожидание ответа *IDN?.
};

#endif // SOCKETCOMMUNICATION_H
