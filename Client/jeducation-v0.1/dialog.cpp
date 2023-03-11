#include "dialog.h"
#include "ui_dialog.h"
#include "Controller/controller.h"
#include "learning.h"
#include <QInputDialog>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->questionTable->setColumnCount(2);
    ui->questionTable->setShowGrid(true);

    // Разрешаем выделение только одного элемента
    ui->questionTable->setSelectionMode(QAbstractItemView::SingleSelection);
    // Разрешаем выделение построчно
    ui->questionTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Устанавливаем заголовки колонок
    ui->questionTable->setHorizontalHeaderLabels(QStringList() << "Отправитель" << "Вопрос");
    // Растягиваем последнюю колонку на всё доступное пространство
    ui->questionTable->horizontalHeader()->setStretchLastSection(true);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_questionTable_cellClicked(int row, int column)
{
    bool ok = false;
    const QString answer = QInputDialog::getText(
        this
        , tr("Окно ответа")
        , tr("Ответ")
        , QLineEdit::Normal
        , tr(""), &ok
    );

    if (ok)
    {
      static_cast<learning*>(parentWidget())->m_Client->sendAnswer(ui->questionTable->item(row,0)->text(),
                                                                 ui->questionTable->item(row,1)->text(),
                                                                 answer);
      ui->questionTable->removeRow(row);
    }
}

