#ifndef SPARAMETERPLOTTER_H
#define SPARAMETERPLOTTER_H

#include <QWidget>
#include <QtCharts>
class SParameterPlotter : public QWidget
{
    Q_OBJECT
public:
    explicit SParameterPlotter(QWidget *parent = nullptr);
    void updateChart(const QVector<QPair<double, double>> &sParams);
private:
//Хранилище данных о графике:
    QChart *chart;
//Отображение:
    QChartView *chartView;
//Диаграмма для соединения точек:
    QLineSeries *series;
};

#endif // SPARAMETERPLOTTER_H
