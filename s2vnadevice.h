#ifndef S2VNADEVICE_H
#define S2VNADEVICE_H

#include"SCPIDevice.h"

class S2VNADevice : public SCPIDevice {
public:
    explicit S2VNADevice(int sens = 1, int sour = 1);

// Класс для генерации scpi команды s2vna устройства:
    class S2VNACommandGenerator : public SCPIDevice::CommandGenerator {
    public:
        explicit S2VNACommandGenerator(S2VNADevice* dev = nullptr) : device(dev) {}
        QString operator()(const QString& operation, const QVariantMap& config) const override;
        QString operator()(const char* operation, const QVariantMap& config) const override;
    private:
        S2VNADevice *device;
    };

    /* [1] - Парсинг scpi, [2] - Сборка scpi */
    QVariant parseResponse(const QString& response) override; // [1]
    S2VNACommandGenerator generateCommand; // [2]
protected:
    friend class CommandGenerator; // Дружественный доступ
    QString buildFrequencySweep(const QVariantMap& config);

private:
    class ScpiBuilder {
    public:
        ScpiBuilder(int sens, int sour) :
            sens_prefix(QString(":SENS%1").arg(sens)),
            sour_prefix(QString(":SOUR%1").arg(sour)) {}

        QString freqStart(double value) const {
            return QString("%1:FREQ:STAR %2").arg(sens_prefix).arg(value);
        }

        QString freqStop(double value) const {
            return QString("%1:FREQ:STOP %2").arg(sens_prefix).arg(value);
        }

        QString freqStep(double value) const {
            return QString("%1:FREQ:STEP %2").arg(sens_prefix).arg(value);
        }

        QString powerLevel(double value) const {
            return QString("%1:POW:LEV %2dBm").arg(sour_prefix).arg(value);
        }

    private:
        QString sens_prefix;
        QString sour_prefix;
    };

    ScpiBuilder builder;
};

#endif // S2VNADEVICE_H
