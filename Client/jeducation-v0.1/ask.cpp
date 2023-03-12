#include "ask.h"
#include "ui_ask.h"
#include "learning.h"
#include "Controller/controller.h"

ask::ask(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ask)
{
    ui->setupUi(this);
    ui->usersTable->setColumnCount(1);
    ui->usersTable->setShowGrid(true);

    // Разрешаем выделение только одного элемента
    ui->usersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    // Разрешаем выделение построчно
    ui->usersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Устанавливаем заголовки колонок
    ui->usersTable->setHorizontalHeaderLabels(QStringList() << "Участники");
    // Растягиваем последнюю колонку на всё доступное пространство
    ui->usersTable->horizontalHeader()->setStretchLastSection(true);
}

ask::~ask()
{
    delete ui;
}

void ask::on_buttonBox_accepted()
{
   static_cast<learning*>(parentWidget())->m_Client->sendQuestion(ui->usersTable->selectedItems().first()->text(),
                                                                  ui->textEdit->toPlainText());
}

