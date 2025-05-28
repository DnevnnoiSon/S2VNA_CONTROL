#ifndef SPARAMETERPLOTTER_H
#define SPARAMETERPLOTTER_H

#include "s2vna_scpi.h"

#include <QWidget>
#include <QtCharts>
class SParameterPlotter : public QWidget
{
    Q_OBJECT
public:
    explicit SParameterPlotter(QWidget *parent = nullptr);
public slots:
    void updateChart(const QString &response);
private:
    S2VNA_SCPI scpi;
    QVBoxLayout *layout;
//Хранилище данных о графике:
    QChart *chart;
//Отображение:
    QChartView *chartView;
//Диаграмма для соединения точек:
    QLineSeries *series;
//Оси OX OY:
    QValueAxis *axisX;
    QValueAxis *axisY;

    void setupChart();
    void setupAxes();
    void setupStyle();
};

#endif // SPARAMETERPLOTTER_H
