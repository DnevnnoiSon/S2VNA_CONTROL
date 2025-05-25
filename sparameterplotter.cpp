#include "sparameterplotter.h"
#include "ui_mainwindow.h"

SParameterPlotter::SParameterPlotter(QWidget *parent)
    : QWidget(parent)
{
    chart = new QChart();
    chartView = new QChartView(chart, this);
    series = new QLineSeries();

//Катсом:
    chart->setTitle("График зависимости:");
    series->setColor(Qt::white);
    chart->setAnimationOptions(QChart::SeriesAnimations);

//Настройка размерностей [делаю исходя из фрейма]
    chartView->setMinimumSize(600, 400);

    // Оси:
    QValueAxis *axis_Spar = new QValueAxis;
    QValueAxis *axis_freq = new QValueAxis;

    axis_Spar->setTitleText("S-параметр");
    axis_freq->setTitleText("Частота");

    chart->addAxis(axis_Spar, Qt::AlignBottom); //выравнивание по нижнему краю
    chart->addAxis(axis_freq, Qt::AlignLeft);   //по левому

    // Добавление серии данных
    chart->addSeries(series);
    series->attachAxis(axis_Spar);
    series->attachAxis(axis_freq);

    // Разметка
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(chartView);
    setLayout(layout);

}

void SParameterPlotter::updateChart(const QVector<QPair<double, double>> &sParams)
{
    series->clear();
    for(const auto &el : sParams)
    {
        series->append(el.first, el.second);
    }

    // chart->axes(Qt::Horizontal).first()->setRange(xData.first(), xData.last());
    // chart->axes(Qt::Vertical).first()->setRange(*std::min_element(yData.begin(), yData.end()),
    // *std::max_element(yData.begin(), yData.end()));
}






