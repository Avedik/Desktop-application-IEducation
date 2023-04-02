#ifndef BRAINSTORM_H
#define BRAINSTORM_H

#include <QDialog>

namespace Ui {
class brainstorm;
}

class brainstorm : public QDialog
{
    Q_OBJECT

public:
    explicit brainstorm(QWidget *parent = nullptr);
    ~brainstorm();

private:
    Ui::brainstorm *ui;
};

#endif // BRAINSTORM_H
