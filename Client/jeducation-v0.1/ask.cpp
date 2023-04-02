#include "ask.h"
#include "ui_ask.h"
#include "learning.h"
#include "Controller/controller.h"
#include <QMessageBox>

ask::ask(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ask)
{
    ui->setupUi(this);
    ui->usersTable->setColumnCount(2);
    ui->usersTable->setShowGrid(true);

    // Разрешаем выделение только одного элемента
    ui->usersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    // Разрешаем выделение построчно
    ui->usersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Устанавливаем заголовки колонок
    ui->usersTable->setHorizontalHeaderLabels(QStringList() << "Фото" << "Участники");
    // Растягиваем последнюю колонку на всё доступное пространство
    ui->usersTable->horizontalHeader()->setStretchLastSection(true);
}

ask::~ask()
{
    delete ui;
}

void ask::on_buttonBox_accepted()
{
   if (ui->usersTable->selectedItems().empty())
   {
       QMessageBox::critical(this,"Ошибка","Выберите адресата");
       return;
   }

   auto selectedItem = ui->usersTable->selectedItems().first();
   static_cast<learning*>(parentWidget())->m_Client->sendQuestion(selectedItem->text(),
                                                                  ui->textEdit->toPlainText());
   selectedItem->setFlags(selectedItem->flags() & (~Qt::ItemIsSelectable));
   selectedItem->setBackground(QBrush(QColor(Qt::red)));
}

