#include "sparameterplotter.h"

SParameterPlotter::SParameterPlotter(QWidget *parent)
    : QWidget(parent)
{   //=== !Костыльный корявый фронт! ===//
    chart = new QChart();
    chartView = new QChartView(chart, this);
    series = new QLineSeries();
    series->setPointsVisible(false);


    // Настройка на заполнение пространства:
    chartView->setMinimumSize(300, 250); // Фиксированный размер
    chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Полная прозрачность графика
    chart->setBackgroundBrush(Qt::transparent);
    chart->setBackgroundRoundness(0);
    chart->setMargins(QMargins(0, 0, 0, 0));
    chart->layout()->setContentsMargins(0, 0, 0, 0);

    // Настройка осей
    QValueAxis *axis_Spar = new QValueAxis;
    QValueAxis *axis_freq = new QValueAxis;

    axis_Spar->setTitleText("S-параметр");
    axis_freq->setTitleText("Частота");

    axis_Spar->setGridLineVisible(true);
    axis_freq->setGridLineVisible(true);
    axis_Spar->setGridLineColor(QColor(255, 255, 255, 30)); // Светло-белая с прозрачностью
    axis_freq->setGridLineColor(QColor(255, 255, 255, 30));

    // Убрать линии рамки осей
    axis_Spar->setLineVisible(false);
    axis_freq->setLineVisible(false);

    chart->addAxis(axis_freq, Qt::AlignBottom);
    chart->addAxis(axis_Spar, Qt::AlignLeft);

    chart->addSeries(series);
    series->attachAxis(axis_Spar);
    series->attachAxis(axis_freq);

    // Стиль для chartView
    chartView->setStyleSheet(
        "background: rgba(30, 30, 30, 200);" // Темный фон с прозрачностью
        "border: 1px solid rgba(255, 255, 255, 100);"
        "border-radius: 8px;"
        );

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0); // Убрать отступы макета
    layout->addWidget(chartView);
    setLayout(layout);
}

void SParameterPlotter::updateChart(const QVector<QPair<double, double>> &sParams)
{
    series->clear();
    if(sParams.isEmpty()) return;

    // Собираем отдельно частоты и значения S-параметров:
    QVector<double> freqs, sValues;
    for(const auto &el : sParams) {
        freqs.append(el.first);
        sValues.append(el.second);
        series->append(el.first, el.second);
    }

    // Автоматическое масштабирование осей:
    auto xAxis = chart->axes(Qt::Horizontal).first();
    auto yAxis = chart->axes(Qt::Vertical).first();

    xAxis->setRange(*std::min_element(freqs.begin(), freqs.end()),
    *std::max_element(freqs.begin(), freqs.end()));
    yAxis->setRange(*std::min_element(sValues.begin(), sValues.end()),
    *std::max_element(sValues.begin(), sValues.end()));
}






