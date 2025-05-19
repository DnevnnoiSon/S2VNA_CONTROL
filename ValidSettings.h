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
private:
/* Валидные параметры ВАЦ: */
    QVariantMap measurement;
};

#endif // VALIDSETTINGS_H
