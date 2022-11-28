#include "learning.h"
#include "ui_learning.h"
#include <QtWidgets>

int tic = 0;
int cnt = 0;

learning::learning(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::learning)
{
    ui->setupUi(this);
    time.setHMS(0,0,10);
    cnt_timer= new QTimer(this);
    cnt_timer->setInterval(1000);
    connect(cnt_timer,SIGNAL(timeout()),this,SLOT(countTimer()));

    timer= new QTimer(this);
    timer->setInterval(160);
    connect(timer,SIGNAL(timeout()),this,SLOT(paintingTimer()));
}

learning::~learning()
{
    delete ui;
}

void learning::on_pushButton_clicked()
{
    if (!cnt_timer->isActive())
    {
        cnt_timer->start();
        timer->start();
    }
}

void learning::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
}

void learning::countTimer()
{
    tic++;
    ui->label_6->setText(time.addSecs(-tic).toString("mm:ss"));
    if (ui->label_6->text() == "00:00")
    {
        timer->stop();
        cnt_timer->stop();
    }
}

void learning::paintingTimer()
{
    cnt++;
    repaint();
}
