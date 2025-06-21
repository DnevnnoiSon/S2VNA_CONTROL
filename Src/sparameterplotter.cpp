#include "sparameterplotter.h"
#include <QVBoxLayout>
#include <limits>

SParameterPlotter::SParameterPlotter(QWidget *parent)
    : QWidget(parent),
    m_chart(std::make_unique<QChart>())
{
    m_series = new QLineSeries(this);

    m_chartView = new QChartView(m_chart.get(), this);

    m_axisX = new QValueAxis();
    m_axisY = new QValueAxis();
    m_layout = new QVBoxLayout(this);

    // Базовая настройка
    setupChart();
    setupAxes();
    setupStyle();

    // Добавление элементов в макет
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addWidget(m_chartView);
    setLayout(m_layout);
}

SParameterPlotter::~SParameterPlotter() = default;

void SParameterPlotter::setupChart() {
    m_chart->addSeries(m_series);
    m_chart->legend()->hide();
    m_chart->setBackgroundBrush(Qt::transparent);
}

void SParameterPlotter::setupAxes() {
    // Настройка осей
    m_axisX->setTitleText("Частота, МГц");
    m_axisY->setTitleText("S-параметры, дБ");
    m_axisX->setLabelFormat("%.2f");
    m_axisY->setLabelFormat("%.1f");
    m_axisX->setTickCount(7);
    m_axisY->setTickCount(5);


    // Стиль сетки
    QPen gridPen(QColor(255, 255, 255, 30));
    m_axisX->setGridLinePen(gridPen);
    m_axisY->setGridLinePen(gridPen);
    m_axisX->setLineVisible(false);
    m_axisY->setLineVisible(false);

    // Добавление осей к графику
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);

    // Привязка серии к осям
    m_series->attachAxis(m_axisX);
    m_series->attachAxis(m_axisY);
}

void SParameterPlotter::setupStyle() {
    m_series->setPen(QPen(QColor("darkgreen"), 2));
    m_series->setPointsVisible(false);

    // Стиль для фона и рамки
    m_chartView->setStyleSheet(
        "background: transparent;"
        "border: 1px solid rgba(255, 255, 255, 100);"
        "border-radius: 8px;"
        );
    // Стиль самого графика внутри View
    m_chart->setBackgroundBrush(QColor(35, 35, 35));
}

void SParameterPlotter::setFrequencyData(const QVector<double>& frequencies) {
    m_frequencies = frequencies;
}

void SParameterPlotter::updateChart(const QString &response)
{
    const auto coordinates = m_scpiParser.parseResponse(response, m_frequencies);
    if (coordinates.isEmpty()) {
        qWarning() << "Invalid or empty S-parameters data received.";
        m_series->clear();
        return;
    }

    QVector<QPointF> points;
    points.reserve(coordinates.size());

    double minX = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();

    constexpr double MEGA = 1e6;

    for (const auto& [sparams, freq_hz] : coordinates) {
        // [ <real, imag>, freq_hz]
        const double freq_mhz = freq_hz / MEGA;

        // Вычисление амплитуды в дБ. Формула была верной.
        const double magnitude = std::hypot(sparams.first, sparams.second);
        // Тернарный оператор для защиты от log10(0)
        const double dB = (magnitude > 0) ? (20 * std::log10(magnitude)) : -std::numeric_limits<double>::infinity();

        //====== Корректность построения ======//
        qDebug() << "Freq (MHz):" << freq_mhz
        << "| Real:" << sparams.first
        << "| Imag:" << sparams.second
        << "| dB:" << dB;

        points.append({freq_mhz, dB});

        // Обновление границ
        minX = std::min(minX, freq_mhz);
        maxX = std::max(maxX, freq_mhz);
        if (std::isfinite(dB)) {
            minY = std::min(minY, dB);
            maxY = std::max(maxY, dB);
        }
    }

    // Обновление серии за один вызов
    m_series->replace(points);

    // Корректировка границ для лучшего вида
    const auto adjustRange = [](double min, double max) -> std::pair<double, double> {
        if (!std::isfinite(min) || !std::isfinite(max) || qFuzzyCompare(min, max)) {
            return {min - 1.0, max + 1.0};
        }
        const double margin = 0.05 * (max - min);
        if (margin < std::numeric_limits<double>::epsilon()) {
            return {min - 1.0, max + 1.0};
        }
        return {min - margin, max + margin};
    };

    auto [finalMinX, finalMaxX] = adjustRange(minX, maxX);
    auto [finalMinY, finalMaxY] = adjustRange(minY, maxY);

    // Установка диапазонов осей
    m_axisX->setRange(finalMinX, finalMaxX);
    m_axisY->setRange(finalMinY, finalMaxY);
}
