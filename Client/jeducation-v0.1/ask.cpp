#include "ask.h"
#include "ui_ask.h"
#include "learning.h"
#include "Controller/controller.h"

ask::ask(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ask)
{
    ui->setupUi(this);
}

ask::~ask()
{
    delete ui;
}

void ask::on_buttonBox_accepted()
{
   static_cast<learning*>(parentWidget())->m_Client->sendQuestion(ui->textEdit->toPlainText());
}


void ask::on_ask_rejected()
{
    hide();
}

