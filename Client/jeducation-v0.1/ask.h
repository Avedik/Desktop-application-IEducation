#ifndef ASK_H
#define ASK_H

#include <QDialog>

namespace Ui {
class ask;
}

class ask : public QDialog
{
    Q_OBJECT

public:
    explicit ask(QWidget *parent = nullptr);
    ~ask();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::ask *ui;
};

#endif // ASK_H
