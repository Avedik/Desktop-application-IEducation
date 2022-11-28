#ifndef LEARNING_H
#define LEARNING_H

#include <QDialog>
#include <QTime>

namespace Ui {
class learning;
}

class learning : public QDialog
{
    Q_OBJECT

public:
    explicit learning(QWidget *parent = nullptr);
    ~learning();

private slots:
    void on_pushButton_clicked();
    void countTimer();
    void paintingTimer();
    void paintEvent(QPaintEvent *);

private:
    Ui::learning *ui;
    QTimer *timer;
    QTimer *cnt_timer;
    QTime time;
};

#endif // LEARNING_H
