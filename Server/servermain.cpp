#include <QApplication>
#include "serverWindow.h"
#include <QtSql>
#include <QSqlDatabase>
#include <QMessageBox>
#include "database.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    database database;
    ServerWindow serverWin;
    serverWin.show();
    return a.exec();
}
