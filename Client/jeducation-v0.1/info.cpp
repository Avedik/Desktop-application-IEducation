#include "info.h"
#include "ui_info.h"

info::info(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::info)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
}

info::~info()
{
    delete ui;
}
