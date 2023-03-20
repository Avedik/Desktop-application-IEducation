#include "brainstorm.h"
#include "ui_brainstorm.h"

brainstorm::brainstorm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::brainstorm)
{
    ui->setupUi(this);
}

brainstorm::~brainstorm()
{
    delete ui;
}
