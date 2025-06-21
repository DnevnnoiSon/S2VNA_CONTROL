#ifndef SOCKETCOMMUNICATION_H
#define SOCKETCOMMUNICATION_H

#include "icommunication.h"

#include <QTcpSocket>
#include <QThread>
#include <QTimer>
#include <memory>

class QThread;

/**
 * @brief Реализация интерфейса ICommunication для связи через TCP/IP сокеты.
 * @details Класс спроектирован для работы в отдельном потоке связи - [работа в не блокирующем режиме]
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
    ~SocketCommunication() override = default;

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
     * @brief Слот для инициализации объекта после перемещения в поток.
     * @details Создает внутренние объекты (сокет, таймер) и настраивает сигналы.
     * Должен быть вызван один раз после вызова moveToThread().
     */
    void initialize();

    /**
    * @brief Слот для передачи данных потоку связи
    * @param Сырые отпралвяемые команды - последовательность разделенная - ';'.
    */
    void acceptMeasureConfig(const QString &command) override;

    /**
    * @brief @copydoc ICommunication::acceptSettingConfig
    * @param Порт и Айпи адрес
    */
    void acceptSettingConfig(const ConnectionSettings &setting) override;
private slots:
/// Методы будут вызываться из главного потока через очередь событий и выполняться текущем потоке:
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
    void onErrorOccurred(QAbstractSocket::SocketError socketError);

    /**
    * @brief Слот, являющийся попыткой соединения
    */
    void attemptConnection();

private:
/// Константы соединения:
    static constexpr int DEFAULT_PORT = 5025;
    static constexpr int CONNECTION_TIMEOUT_MS = 200;
    static constexpr int POLLING_INTERVAL_MS = 200;
    static constexpr int WRITE_TIMEOUT_MS = 1000;

    QHostAddress m_targetAddress = QHostAddress::LocalHost; ///< Настройка соединения по умолчанию.
    int m_port = DEFAULT_PORT;                              ///< Настройка соединения по умолчанию.

    std::unique_ptr<QTcpSocket> m_socket; ///< Умный указатель на TCP сокет.

    QByteArray m_responseBuffer; ///< Буффер для накопления принятых данных.

    //Периодический опрос состояния:
    std::unique_ptr<QTimer> m_pollTimer; ///< Таймер для периодических попыток переподключения.
    bool isDeviceReady = false;    ///< Флаг готовности устройства [ сокетное подключение ]
    bool isExpectingIDN = false;   ///< Флаг, указывающий на ожидание ответа *IDN?.
};

#endif // SOCKETCOMMUNICATION_H

