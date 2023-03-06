#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <iostream>
#include "info.h"
#include <QPixmap>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    window = new ChooseMode();
    connect(window, &ChooseMode::firstWindow, this, &MainWindow::show);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    window->show();
    this->close();
}


void MainWindow::on_pushButton_2_clicked()
{
    window1 = new info(this);
    window1 -> show();
}

