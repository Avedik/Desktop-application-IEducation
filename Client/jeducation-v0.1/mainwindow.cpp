#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <iostream>
#include "info.h"
#include <QPixmap>
#include "rating.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    window2 = new rating(this);
    window = new ChooseMode(window2);
    connect(window, &ChooseMode::firstWindow, this, &MainWindow::show);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    window->show();
    this->hide();
}


void MainWindow::on_pushButton_2_clicked()
{
    window1 = new info(this);
    window1 -> show();
}

void MainWindow::on_pushButton_3_clicked()
{
    window2 -> show();

}

