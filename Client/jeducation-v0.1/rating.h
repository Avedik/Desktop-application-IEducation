#ifndef RATING_H
#define RATING_H

#include <QDialog>

namespace Ui {
class rating;
}

class rating : public QDialog
{
    Q_OBJECT

public:
    explicit rating(QWidget *parent = nullptr);
    ~rating();

signals:
    void firstWindow1();


private slots:
    void on_pushButton_clicked();

private:


    Ui::rating *ui;
};

#endif // RATING_H
