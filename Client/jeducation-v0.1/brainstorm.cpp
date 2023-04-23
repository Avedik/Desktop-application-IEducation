#include "brainstorm.h"
#include "ui_brainstorm.h"

brainstorm::brainstorm(QWidget *parent) :
    QDialog(parent),
    currentInstrument(Instruments::Stylet),
    ui(new Ui::brainstorm)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    scene = new Painter(this);
    ui->table->setScene(scene);
    ui->table->setBackgroundBrush(QColor("white"));
}

brainstorm::~brainstorm()
{
    delete ui;
    delete scene;
}

brainstorm::Instruments brainstorm::getCurrentInstrument()
{
    return currentInstrument;
}


void brainstorm::on_styletButton_clicked()
{
    currentInstrument = Instruments::Stylet;
}


void brainstorm::on_eraserButton_clicked()
{
    currentInstrument = Instruments::Eraser;
}


void brainstorm::on_clearButton_clicked()
{
    scene->clear();
}

