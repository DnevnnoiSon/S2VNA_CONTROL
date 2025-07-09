#ifndef SPARAMETERPLOTTER_H
#define SPARAMETERPLOTTER_H

#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include "s2vna_scpi.h"

QT_BEGIN_NAMESPACE
    class QChartView;
    class QLineSeries;
    class QValueAxis;
    class QVBoxLayout;
QT_END_NAMESPACE

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
    * @brief Деструктор.
    */
     ~SParameterPlotter();

    /**
     * @brief Устанавливает вектор частот для последующего парсинга.
     * @param frequencies Вектор значений частот в Гц.
     */
    void setFrequencyData(const QVector<double>& frequencies);

    /**
     * @brief Включает или отключает кэширование следующей отрисовки.
     * @param enabled true для включения кэширования, false для отключения.
    */
    void setCachingEnabled(bool enabled);
public slots:
    /**
     * @brief Обновляет график на основе нового ответа от устройства.
     * @details Парсит строку ответа, вычисляет значения в дБ и перерисовывает график.
     * @param response Строка ответа с S-параметрами.
     */
    void updateChart(const QString &response);

signals:
    /**
     * @brief Данные готовы к кэшированию
     */
    void CacheReady(const QVector<QPair<QPair<double, double>, double>>& parsedData);
private:
    /**
     * @brief Выполняет первоначальную настройку объекта QChart.
     */
    void setupChart();

    /**
     * @brief Настраивает оси X и Y графика.
     */
    void setupAxes();

    /**
     * @brief Применяет стили к графическим компонентам.
     */
    void setupStyle();

    // Графические компоненты
    std::unique_ptr<QChart> m_chart; ///< Основной объект графика.

    QChartView *m_chartView = nullptr;  ///< Виджет для отображения графика.
    QLineSeries *m_series = nullptr;    ///< Серия данных для линейного графика.
    QValueAxis *m_axisX = nullptr;      ///< Ось абсцисс (частота).
    QValueAxis *m_axisY = nullptr;      ///< Ось ординат (амплитуда в дБ).
    QVBoxLayout *m_layout = nullptr;    ///< Менеджер компоновки.

    // Вспомогательные данные
    S2VNA_SCPI m_scpiParser;       ///< Объект для парсинга SCPI-ответов.
    QVector<double> m_frequencies; ///< Вектор частот для текущего измерения.

    bool m_isCachingEnabled = true; ///< Флаг, разрешающий кэширование
};

#endif // SPARAMETERPLOTTER_H
