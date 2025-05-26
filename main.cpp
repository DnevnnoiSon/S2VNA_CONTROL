#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow S2VNA_Control;
    S2VNA_Control.setWindowTitle("S2VNA CONTROL");
    //S2VNA - в демо режиме

    S2VNA_Control.show();
    return a.exec();
}
