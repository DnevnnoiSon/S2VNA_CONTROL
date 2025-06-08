#ifndef CONNECTIONSETTINGS_H
#define CONNECTIONSETTINGS_H

#include <QVariantMap>

struct Network{
    QString ip_addr;
    int port;
};

class ConnectionSettings{
public:
/* Для socket взаимодействия: */
    Network network;
//Другие типы соединения:
};

#endif // CONNECTIONSETTINGS_H

