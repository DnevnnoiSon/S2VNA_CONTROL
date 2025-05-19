#ifndef SCPIDEVICE_H
#define SCPIDEVICE_H

#include <QString>
#include <QVariant>
#include <QVariantMap>

class SCPIDevice {
public:
    virtual ~SCPIDevice() = default;

    // Парсинг ответа устройства:
    virtual QVariant parseResponse(const QString& response) = 0;

    // Преобразование числа в научную нотацию
    static QString toScientific(double value, int precision = 6) {
        return QString::number(value, 'e', precision);
    }
};

#endif // SCPIDEVICE_H
