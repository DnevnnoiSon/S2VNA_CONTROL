#ifndef VALIDSETTINGS_H
#define VALIDSETTINGS_H

#include <QVariantMap>

struct Network{
    QString ip_addr;
    int port;
};

class Settings{
public:
/* Для socket взаимодействия: */
    Network network;
//Другие типы соединения:
};

#endif // VALIDSETTINGS_H
