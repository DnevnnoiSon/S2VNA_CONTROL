#include "sparameterplotter.h"
//=== !Костыльный фронт! ===//

SParameterPlotter::SParameterPlotter(QWidget *parent)
    : QWidget(parent)
{
    // Инициализация объектов
    chart = new QChart();
    chartView = new QChartView(chart, this);
    series = new QLineSeries(this);
    axisX = new QValueAxis();
    axisY = new QValueAxis();
    layout = new QVBoxLayout(this);

    // Базовая настройка
    setupChart();
    setupAxes();
    setupStyle();

    // Добавление элементов в макет
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(chartView);
    setLayout(layout);
}

void SParameterPlotter::setupChart() {
    chart->addSeries(series);
    chart->legend()->hide();
    chart->setBackgroundBrush(Qt::transparent);
    chart->setBackgroundRoundness(0);
    chart->setMargins(QMargins(0, 0, 0, 0));
    chart->layout()->setContentsMargins(0, 0, 0, 0);
}

void SParameterPlotter::setupAxes() {
    // Настройка осей
    axisX->setTitleText("Frequency, GHz");
    axisY->setTitleText("S-Parameter, dB");
    axisX->setLabelFormat("%.2f");
    axisY->setLabelFormat("%.1f");

    // Стиль сетки
    axisX->setGridLineColor(QColor(255, 255, 255, 30));
    axisY->setGridLineColor(QColor(255, 255, 255, 30));
    axisX->setLineVisible(false);
    axisY->setLineVisible(false);

    // Добавление осей к графику
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    // Привязка серии к осям
    series->attachAxis(axisX);
    series->attachAxis(axisY);
}

void SParameterPlotter::setupStyle() {
    series->setPen(QPen(QColor("#2c3e50"), 2));
    series->setPointsVisible(false);
    chartView->setMinimumSize(300, 250);
    chartView->setStyleSheet(
        "background: rgba(30, 30, 30, 200);"
        "border: 1px solid rgba(255, 255, 255, 100);"
        "border-radius: 8px;"
        );
}

//========== Установка графика с новыми значениями =========//
void SParameterPlotter::updateChart(const QString &response) {
    series->clear();
    const auto sParams = scpi.parseResponse(response);

    if (sParams.isEmpty()) {
        qWarning() << "Invalid or empty S-parameters data";
        return;
    }

    // Расчет min/max
    double minX = sParams.first().first, maxX = minX;
    double minY = sParams.first().second, maxY = minY;

    for (const auto &[x, y] : sParams) {
        series->append(x, y);
        minX = qMin(minX, x);
        maxX = qMax(maxX, x);
        minY = qMin(minY, y);
        maxY = qMax(maxY, y);
    }

    // Обновление диапазонов осей
    const double margin = 0.05;
    axisX->setRange(minX - (maxX - minX) * margin,
    maxX + (maxX - minX) * margin);
    axisY->setRange(minY - (maxY - minY) * margin,
    maxY + (maxY - minY) * margin);
}




