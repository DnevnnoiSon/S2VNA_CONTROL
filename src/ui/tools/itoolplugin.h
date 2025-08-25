#ifndef ITOOLPLUGIN_H
#define ITOOLPLUGIN_H

#include <QString>
#include <QIcon>
#include <QtPlugin>

class SParameterPlotter;

constexpr auto IToolPlugin_iid = "s2vnacontrol.IToolPlugin";

class IToolPlugin
{
public:
    virtual ~IToolPlugin() = default;

    // Возвращает имя инструмента для отображения на кнопке
    virtual QString name() const = 0;

    // Возвращает иконку для кнопки
    virtual QIcon icon() const = 0;

    // Метод, вызываемый при активации инструмента.
    // Сюда внедряется зависимость — указатель на график.
    virtual void activate(SParameterPlotter* plotter) = 0;

    // Метод, вызываемый при деактивации (выборе другого инструмента)
    virtual void deactivate() = 0;
};

Q_DECLARE_INTERFACE(IToolPlugin, IToolPlugin_iid);

#endif // ITOOLPLUGIN_H
