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
    axisX->setTitleText("Частота, Мгц");
    axisY->setTitleText("S-параметры, дБ");
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

void SParameterPlotter::setFrequencyData(const QVector<double>& frequencies) {
    scpi.Frequency = frequencies;
}

//========== Установка графика с новыми значениями =========//
//x — частота (МГц),
//y — значение S-параметра (дБ).
void SParameterPlotter::updateChart(const QString &response) {
    // Парсинг данных - формирование полей 0X/0Y
    const auto coordinates = scpi.parseResponse(response, scpi.Frequency);
    if (coordinates.isEmpty()) {
        qWarning() << "Invalid or empty S-parameters data";
        series->clear();
        return;
    }

    for(const auto &el : coordinates){
        qDebug() << "Parsed sParams:" << el; //!проверка парсера
    }


    // Подготовка контейнера для точек
    QVector<QPointF> points;
    points.reserve(coordinates.size());

    // Вычисление границ
    double minX = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();

    // Единый проход для преобразования данных и вычисления min/max
    for (const auto &[complex, freq] : coordinates) {
        const double real = complex.first;
        const double imag = complex.second;

        // Вычисление амплитуды в дБ
        const double magnitude = std::hypot(real, imag);
        const double dB = 20 * std::log10(magnitude);

        // Обновление границ
        minX = std::min(minX, freq);
        maxX = std::max(maxX, freq);
        minY = std::min(minY, dB);
        maxY = std::max(maxY, dB);

        points.append({freq, dB});
    }

// Обновление серии за один вызов
    series->replace(points);

    // Корректировка границ при одинаковых значениях
    const auto adjustRange = [](double min, double max) -> std::pair<double, double> {
        if (qFuzzyCompare(min, max)) {
            return {min - 1.0, max + 1.0};
        }
        const double margin = 0.05 * (max - min);
        return {min - margin, max + margin};
    };

    const auto [xMin, xMax] = adjustRange(minX, maxX);
    const auto [yMin, yMax] = adjustRange(minY, maxY);

    // Установка диапазонов осей
    axisX->setRange(xMin, xMax);
    axisY->setRange(yMin, yMax);

    // Настройка форматов
    axisX->setLabelFormat((maxX - minX < 1.0) ? "%.3f" : "%.1f");
    axisY->setLabelFormat("%.1f");
    axisX->setTickCount(5);
    axisY->setTickCount(5);
}




