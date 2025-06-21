#ifndef SPARAMETERPLOTTER_H
#define SPARAMETERPLOTTER_H

#include <QWidget>
#include <QtCharts>
#include "s2vna_scpi.h"

// Прямые объявления для уменьшения зависимостей в заголовках
class QChart;
class QChartView;
class QLineSeries;
class QValueAxis;

/**
 * @brief Виджет для отображения графика S-параметров.
 * @details Инкапсулирует всю логику создания, настройки и обновления
 * графика на основе данных, полученных от устройства.
 */
class SParameterPlotter : public QWidget {
    Q_OBJECT
public:
    /**
     * @brief Конструктор виджета.
     * @param parent Родительский виджет.
     */
    explicit SParameterPlotter(QWidget *parent = nullptr);

    /**
     * @brief Устанавливает вектор частот для последующего парсинга.
     * @param frequencies Вектор значений частот в Гц.
     */
    void setFrequencyData(const QVector<double>& frequencies);

public slots:
    /**
     * @brief Обновляет график на основе нового ответа от устройства.
     * @details Парсит строку ответа, вычисляет значения в дБ и перерисовывает график.
     * @param response Строка ответа с S-параметрами.
     */
    void updateChart(const QString &response);

private:
    /**
     * @brief Выполняет первоначальную настройку объекта QChart.
     */
    void setupChart();

    /**
     * @brief Настраивает оси X и Y графика.
     */
    void setupAxes();

    // Графические компоненты
    QChart *m_chart;          ///< Основной объект графика.
    QChartView *m_chartView;  ///< Виджет для отображения графика.
    QLineSeries *m_series;    ///< Серия данных для линейного графика.
    QValueAxis *m_axisX;      ///< Ось абсцисс (частота).
    QValueAxis *m_axisY;      ///< Ось ординат (амплитуда в дБ).

    // Вспомогательные данные
    S2VNA_SCPI m_scpiParser;       ///< Объект для парсинга SCPI-ответов.
    QVector<double> m_frequencies; ///< Вектор частот для текущего измерения.
};

#endif // SPARAMETERPLOTTER_H
