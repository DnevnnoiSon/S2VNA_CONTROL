#ifndef CONNECTIONSETTINGS_H
#define CONNECTIONSETTINGS_H

#include <QString>

/**
 * @brief Структура для хранения сетевых параметров подключения.
 */
struct Network {
    QString ip_addr; ///< IP-адрес хоста.
    int port;        ///< Порт для подключения.
};

/**
 * @brief Класс-контейнер для хранения всех настроек подключения.
 * @details Может быть расширен для поддержки других типов соединений (COM, USB и т.д.).
 */
class ConnectionSettings {
public:
    /** @brief Настройки для TCP/IP сокетного взаимодействия. */
    Network network;

    // Здесь можно добавить другие типы настроек, например:
    // SerialPortSettings serial;
};

#endif // CONNECTIONSETTINGS_H
