#include "other_questions.h"
#include "ui_other_questions.h"

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


