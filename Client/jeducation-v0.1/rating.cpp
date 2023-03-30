#include "rating.h"
#include "ui_rating.h"
#include "mainwindow.h"

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

void rating::on_pushButton_clicked()
{
    this->close();
    emit firstWindow1();
}



