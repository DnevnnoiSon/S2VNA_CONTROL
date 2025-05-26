#include "s2vna_scpi.h"

QVector<QPair<double, double>> S2VNA_SCPI::parseResponse(const QString& response)
{
    QVector<QPair<double, double>> sParams;
    QStringList parts = response.split(',');

// Проверка четности количества параметров:
    if (parts.size() % 2 != 0) {
    // qWarning() << "Invalid response format!";
        return QVector<QPair<double, double>>();
    }
    auto it = parts.constBegin();
    while (it != parts.constEnd()) {
        double re = (it++)->toDouble();
        double im = (it++)->toDouble();
        sParams.append({re, im});
    }
    return sParams;
}

QString S2VNA_SCPI::generateCommand(const QList<QPair<QString, QVariant>>& params) const
{ // Команда - ключ, Значение - строка
    QStringList commands;

    for (const auto &[key, value] : params) {
        // Формирование команды: "SCPI_CMD VALUE" + учитывать отсутствие VALUE:
        commands.append(value.toInt() ? QString("%1 %2").arg(key, value.toString()) : key);
    }
    // Объеденение команд "\n;":
    return commands.join("\n;") + "\n";
}
