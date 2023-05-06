#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class learning;
class Dialog : public QDialog
{
    Q_OBJECT
    friend learning;

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:
    void on_questionTable_cellClicked(int row, int column);

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
