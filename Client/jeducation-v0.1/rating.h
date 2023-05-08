#ifndef RATING_H
#define RATING_H

#include <QDialog>
#include <QString>

namespace Ui {
class rating;
}

class rating : public QDialog
{
    Q_OBJECT

public:
    explicit rating(QWidget *parent = nullptr);
    ~rating();
    void refreshRating(const QString &destUser, qint32 score);

signals:
    void firstWindow1();


private slots:
    void on_pushButton_clicked();

    void on_rating_rejected();

private:


    Ui::rating *ui;
};

#endif // RATING_H
