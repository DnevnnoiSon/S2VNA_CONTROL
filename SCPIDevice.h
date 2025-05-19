#ifndef SCPIDEVICE_H
#define SCPIDEVICE_H

#include <QString>
#include <QVariant>
#include <QVariantMap>

class SCPIDevice {
public:
    class CommandGenerator {
    public:
        virtual ~CommandGenerator() = default;
        virtual QString operator()(const QString& operation, const QVariantMap& config) const = 0;
        virtual QString operator()(const char* operation, const QVariantMap& config) const = 0;
    };

    virtual ~SCPIDevice() = default;
    // Парсинг ответа устройства:
    virtual QVariant parseResponse(const QString& response) = 0;

    // Преобразование числа:
    static QString toScientific(double value, int precision = 6) {
        return QString::number(value, 'e', precision);
    }
};

#endif // SCPIDEVICE_H
