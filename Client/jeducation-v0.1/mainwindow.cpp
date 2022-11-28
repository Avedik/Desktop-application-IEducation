#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "choosemode.h"
#include <QMessageBox>
#include <iostream>
#include "info.h"
#include <QPixmap>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    //this->close();
    window = new ChooseMode(this);
    window -> show();
}


void MainWindow::on_pushButton_2_clicked()
{
    window1 = new info(this);
    window1 -> show();
}

