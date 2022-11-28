#ifndef LEARNING_H
#define LEARNING_H

#include <QDialog>

namespace Ui {
class learning;
}

class learning : public QDialog
{
    Q_OBJECT

public:
    explicit learning(QWidget *parent = nullptr);
    ~learning();

private:
    Ui::learning *ui;
};

#endif // LEARNING_H
