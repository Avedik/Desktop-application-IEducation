#ifndef CHOOSEMODE_H
#define CHOOSEMODE_H

#include <QDialog>
#include "learning.h"

namespace Ui {
class ChooseMode;
}

class ChooseMode : public QDialog
{
    Q_OBJECT

public:
    explicit ChooseMode(QWidget *parent = nullptr);
    ~ChooseMode();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::ChooseMode *ui;
    learning *mode1;
};

#endif // CHOOSEMODE_H
