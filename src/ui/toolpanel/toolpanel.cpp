#include "toolpanel.h"
#include "ui_toolpanel.h" // Сгенерированный заголовок из .ui файла
#include <QToolButton>
#include <QDebug>

ToolPanel::ToolPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ToolPanel)
{
    ui->setupUi(this);
    ui->toolContainer->setVisible(false);
}

ToolPanel::~ToolPanel()
{
    delete ui;
}

void ToolPanel::setPlotter(SParameterPlotter* plotter)
{
    m_plotter = plotter;
}

void ToolPanel::addTool(IToolPlugin* tool)
{
    if (!tool || !m_plotter) {
        qWarning() << "Неудачная попытка добавить инструмент";
        return;
    }

    auto* button = new QToolButton();
    button->setText(tool->name());
    button->setIcon(tool->icon());
    button->setCheckable(true);
    button->setAutoExclusive(true);

    ui->toolLayout->addWidget(button);

    connect(button, &QToolButton::clicked, this, [this, tool](bool checked)
    {
        if (m_activeTool){
            m_activeTool->deactivate();
        }

        if (checked){
            qDebug() << "Activating tool:" << tool->name();
            m_activeTool = tool;
            m_activeTool->activate(m_plotter);
        }
        else{
            qDebug() << "No tool is active.";
            m_activeTool = nullptr;
        }
    });
}

void ToolPanel::on_toggleButton_clicked()
{
    bool isVisible = ui->toolContainer->isVisible();
    ui->toolContainer->setVisible(!isVisible);
    //TODO: Сделать норм отображение
}
