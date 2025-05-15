#ifndef DIALOGNETWORK_H
#define DIALOGNETWORK_H

#include <QDialog>

namespace Ui {
class DialogNetwork;
}

class DialogNetwork : public QDialog
{
    Q_OBJECT

public:
    explicit DialogNetwork(QWidget *parent = nullptr);
    ~DialogNetwork();

private:
    Ui::DialogNetwork *ui;
};

#endif // DIALOGNETWORK_H
