#ifndef ASK_H
#define ASK_H

#include <QDialog>

namespace Ui {
class ask;
}

class learning;
class ask : public QDialog
{
    Q_OBJECT
    friend learning;

public:
    explicit ask(QWidget *parent = nullptr);
    ~ask();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::ask *ui;
};

#endif // ASK_H
