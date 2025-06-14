#ifndef S2VNA_SCPI_H
#define S2VNA_SCPI_H

#include <QVariant>
#include <QVariantMap>
#include <QString>
#include <QStringList>

//==================================================================//
//     SPCI Прослойка между [Интерфейсом Связи] и [GUI]             //
//==================================================================//
class S2VNA_SCPI
{ /* [1] - Парсинг scpi, [2] - Сборка scpi */
public:
    QVector<QPair<QPair<double, double>, double>> parseResponse(const QString& response, const QVector<double>& Frequency);  // [1]
    QString generateCommand(const QList<QPair<QString, QVariant>>& params) const;                                            // [2]
    //Вых. параметр частоты для парсера.
    QVector<double> Frequency;
};

#endif // S2VNA_SCPI_H
