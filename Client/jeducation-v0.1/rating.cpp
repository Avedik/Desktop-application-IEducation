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

void rating::refreshRating(const QString &destUser, qint32 score)
{
    auto table = ui->tableWidget;

    bool found = false;
    for(int i = 0; i < table->rowCount(); ++i)
        if(table->item(i, 0)->text() == destUser)
        {
            table->item(i, 1)->setText(QString::number(table->item(i, 1)->text().toInt() + score));
            found = true;
            break;
        }

    if (!found)
    {
        table->insertRow(0);
        QTableWidgetItem *name = new QTableWidgetItem(destUser);
        name->setFlags(name->flags() & (~Qt::ItemIsSelectable) & (~Qt::ItemIsEditable));
        table->setItem(0, 0, name);

        QTableWidgetItem *val = new QTableWidgetItem(QString::number(score));
        val->setFlags(val->flags() & (~Qt::ItemIsSelectable) & (~Qt::ItemIsEditable));
        table->setItem(0, 1, val);
    }
}



