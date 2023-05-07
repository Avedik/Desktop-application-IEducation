#include "other_questions.h"
#include "ui_other_questions.h"
#include "learning.h"
#include "Controller/controller.h"
#include <QMessageBox>
#include <QString>

other_questions::other_questions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::other_questions)
{
    ui->setupUi(this);

    ui->answerTable->setColumnCount(4);
    ui->answerTable->setShowGrid(true);

    // Разрешаем выделение только одного элемента
    ui->answerTable->setSelectionMode(QAbstractItemView::SingleSelection);
    // Разрешаем выделение построчно
    ui->answerTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Устанавливаем заголовки колонок
    ui->answerTable->setHorizontalHeaderLabels(QStringList() << "Отправил" << "Ответил" << "Вопрос" << "Ответ");
    // Растягиваем последнюю колонку на всё доступное пространство
    ui->answerTable->horizontalHeader()->setStretchLastSection(true);
}

other_questions::~other_questions()
{
    delete ui;
}

void other_questions::on_trueButton_clicked()
{
    if (ui->answerTable->selectedItems().empty())
    {
        QMessageBox::critical(this,"Ошибка","Выберите участника");
        return;
    }

    auto selectedItem = ui->answerTable->selectedItems().first();
    QString destName = ui->answerTable->item(selectedItem->row(), 1)->text();
    static_cast<learning*>(parentWidget())->m_Client->sendScore(destName, 1);
}

void other_questions::on_falseButton_clicked()
{
    if (ui->answerTable->selectedItems().empty())
    {
        QMessageBox::critical(this,"Ошибка","Выберите участника");
        return;
    }

    auto selectedItem = ui->answerTable->selectedItems().first();
    QString destName = ui->answerTable->item(selectedItem->row(), 1)->text();
    static_cast<learning*>(parentWidget())->m_Client->sendScore(destName, -1);
}

