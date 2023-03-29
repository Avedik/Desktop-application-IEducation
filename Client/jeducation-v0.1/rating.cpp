#include "rating.h"
#include "ui_rating.h"

rating::rating(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::rating)
{
    ui->setupUi(this);
}

rating::~rating()
{
    delete ui;
}
