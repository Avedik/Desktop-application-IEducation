#include "choosemode.h"
#include "ui_choosemode.h"
#include "learning.h"
#include "mainwindow.h"
#include "brainstorm.h"

ChooseMode::ChooseMode(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChooseMode)
{
    ui->setupUi(this);
}

ChooseMode::~ChooseMode()
{
    delete ui;
}

void ChooseMode::on_pushButton_clicked()
{
    mode1 = new learning(this);
    mode1 -> show();
}

void ChooseMode::on_pushButton_2_clicked()
{
    this->close();
    emit firstWindow();
}

void ChooseMode::on_pushButton_3_clicked()
{
    mode2 = new brainstorm(this);
    mode2 -> show();
}

