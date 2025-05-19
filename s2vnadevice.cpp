#include "s2vnadevice.h"

S2VNADevice::S2VNADevice(int sens, int sour)
    : generateCommand(this),
    builder(sens, sour)
{

}

QVariant S2VNADevice::parseResponse(const QString& response)
{
    QVector<QPair<double, double>> sParams;
    QStringList parts = response.split(',');

// Проверка четности количества параметров:
    if (parts.size() % 2 != 0) {
    // qWarning() << "Invalid response format!";
        return QVariant();
    }

    auto it = parts.constBegin();
    while (it != parts.constEnd()) {
        double re = (it++)->toDouble();
        double im = (it++)->toDouble();
        sParams.append({re, im});
    }
    return QVariant::fromValue(sParams);
}

//======================================== ПЕРЕГРУЖЕННЫЕ ОПЕРАТОРЫ ==========================================//
QString S2VNADevice::S2VNACommandGenerator::operator()(const QString& operation, const QVariantMap& config) const
{
    if (!this->device){ qWarning() << "Device not initialized"; }

    if (operation == "FREQ_SWEEP") {
        return this->device->buildFrequencySweep(config);
    } else if (operation == "SET_POWER") {
        return this->device->builder.powerLevel(config["power"].toDouble());
    }

    //qWarning() << "Invalid operation!";
    return 0;
}

QString S2VNADevice::S2VNACommandGenerator::operator()(const char* op, const QVariantMap& cfg) const {
    return (*this)(QString(op), cfg);
}

QString S2VNADevice::buildFrequencySweep(const QVariantMap& config){
    QStringList commands;
    commands << builder.freqStart(config["start_freq"].toDouble())
             << builder.freqStop(config["stop_freq"].toDouble())
             << builder.freqStep(config["step"].toDouble());
    return commands.join(";");
}
