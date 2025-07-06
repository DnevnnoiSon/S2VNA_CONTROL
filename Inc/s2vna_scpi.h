#ifndef S2VNA_SCPI_H
#define S2VNA_SCPI_H

#include <QVariant>
#include <QString>
#include <QVector>
#include <QPair>

/**
 * @brief Класс-прослойка для работы с SCPI-командами.
 * @details Отвечает за две основные задачи:
 * 1. Генерация SCPI-команд из параметров, заданных в GUI.
 * 2. Парсинг ответов от устройства (в частности, S-параметров).
 */
class S2VNA_SCPI {
public:
    /**
     * @brief Парсит строку ответа с S-параметрами.
     * @param response Строка ответа от устройства, содержащая пары (real, imag).
     * @param frequency Вектор частот, соответствующий полученным S-параметрам.
     * @return Вектор пар, где каждая пара представляет координаты форматом: { {real, imag}, frequency }.
     */
    QVector<QPair<QPair<double, double>, double>> parseResponse(const QString& response, const QVector<double>& frequency);

    /**
     * @brief Генерирует строку SCPI-команд из списка параметров.
     * @param params Список пар "Команда-Значение".
     * @return Отформатированная строка SCPI-команд, готовая к отправке.
     */
    QString generateCommand(const QList<QPair<QString, QVariant>>& params) const;
};

#endif // S2VNA_SCPI_H
