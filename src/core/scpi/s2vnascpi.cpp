#include "s2vnascpi.h"
#include <QDebug>

QVector<QPair<QPair<double, double>, double>> S2VNA_SCPI::parseResponse(const QString& response, const QVector<double>& frequency)
{
    QVector<QPair<QPair<double, double>, double>> coordinates;
    QStringList parts = response.split(',');

    for(auto &el : parts){
        // удаление всех '+':
        el.replace("+", "");
    }

    if( (parts.size() / 2) != frequency.size() ){
        qWarning() << "Количество точек частоты: " << frequency.size();
        qWarning() << "Количество точек S параметров: " << parts.size() / 2;
        return coordinates;
    }

    // Проверка четности количества параметров:
    if (parts.size() % 2 != 0 ) {
        qWarning() << "Неверный формат ответа!";
        return coordinates;
    }

    coordinates.reserve(frequency.size());

    auto it_x = frequency.constBegin();
    auto it_y = parts.constBegin();
    while (it_y != parts.constEnd()) {
        coordinates.append({ {(it_y++)->toDouble(), (it_y++)->toDouble()}, *(it_x++) });
    }
    return coordinates;
}

QString S2VNA_SCPI::generateCommand(const QList<QPair<QString, QVariant>>& params) const
{
    QStringList commands;

    for (const auto &[key, value] : params) {
        // Формирование команды: "SCPI_CMD VALUE" + учитывать отсутствие VALUE:
        if (value.isValid() && !value.toString().isEmpty()) {
            if (value.typeId() == QMetaType::Int && value.toInt() == 0 && key.endsWith("?")) {
                commands.append(key);
            } else {
                commands.append(QString("%1 %2").arg(key, value.toString()));
            }
        } else {
            commands.append(key);
        }
    }
    // Объеденение команд ";":
    return commands.join("\n;") + "\n";
}
