#ifndef TOOLSPANEL_H
#define TOOLSPANEL_H

#include <QWidget>
#include "ui/tools/itoolplugin.h"

class SParameterPlotter;

namespace Ui {
class ToolPanel;
}

/**
 * @brief The ToolsPanel class - Виджет-посредник.
 * @details Управляет UI-представлением инструментов и их жизненным циклом.
 * Инкапсулирует логику активации/деактивации плагинов.
*/
class ToolPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ToolPanel(QWidget *parent = nullptr);
    ~ToolPanel();

    /**
     * @brief Внедряет зависимость в панель.
     * @param plotter Указатель на объект графика.
    */
    void setPlotter(SParameterPlotter* plotter);

    /**
     * @brief Добавляет инструмент, загруженный из плагина.
     * @details Создает для инструмента кнопку и настраивает логику ее работы.
     * @param tool Указатель на экземпляр плагина.
    */
    void addTool(IToolPlugin* tool);

private slots:
    /**
     * @brief Обрабатывает нажатие на главную кнопку для скрытия/отображения панели.
    */
    void on_toggleButton_clicked();

private:
    Ui::ToolPanel *ui;

    SParameterPlotter* m_plotter = nullptr; ///< Указатель на виджет с графиком.
    IToolPlugin* m_activeTool = nullptr;    ///< Указатель на текущий инструмент.
};

#endif // TOOLSPANEL_H
