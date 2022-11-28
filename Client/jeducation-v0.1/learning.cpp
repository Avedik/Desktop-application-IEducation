#include "learning.h"
#include "ui_learning.h"

learning::learning(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::learning)
{
    ui->setupUi(this);
}

learning::~learning()
{
    delete ui;
}
