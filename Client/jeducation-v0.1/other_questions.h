#ifndef OTHER_QUESTIONS_H
#define OTHER_QUESTIONS_H

#include <QDialog>

namespace Ui {
class other_questions;
}

class learning;
class other_questions : public QDialog
{
    Q_OBJECT
    friend learning;

public:
    explicit other_questions(QWidget *parent = nullptr);
    ~other_questions();

private:
    Ui::other_questions *ui;
};

#endif // OTHER_QUESTIONS_H
