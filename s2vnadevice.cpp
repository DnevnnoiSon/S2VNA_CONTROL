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

QString S2VNADevice::buildFrequencySweep(const QVariantMap& config){
    return QString("%1;%2")
        .arg(builder.freqStart(config["start_freq"].toDouble()))
        .arg(builder.freqStart(config["stop_freq"].toDouble()));
}

//======================================== ПЕРЕГРУЖЕННЫЕ ОПЕРАТОРЫ ==========================================//
QString S2VNADevice::CommandGenerator::operator()(const QString& operation, const QVariantMap& config) const {
    if (!device) throw std::invalid_argument("Device not initialized");
    if (operation == "FREQ_SWEEP") {
        return device->buildFrequencySweep(config);
    } else if (operation == "SET_POWER") {
        return device->builder.powerLevel(config["power"].toDouble());
    }
    //qDebug() << "Invalid operation!";
    return 0;
}

QString S2VNADevice::CommandGenerator::operator()(const char* op, const QVariantMap& cfg) const {
    return (*this)(QString(op), cfg);
}


