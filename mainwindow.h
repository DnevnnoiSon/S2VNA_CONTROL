#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QChart>
#include <QVector>

#include "MeasurementConfig.h"
#include "icommunication.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_measureButton_clicked();
    void onDeviceStatusChanged(bool isReady);
    void handleDeviceError(const QString& errorMessage);
private:
    Ui::MainWindow *ui;
 //Обьект коммуникации:
    ICommunication* communicator;

    QChart *chart;
 //Валидируемые данные:
    MeasurementConfig measurement_config;
};


#endif // MAINWINDOW_H
