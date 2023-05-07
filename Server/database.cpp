#include "database.h"
#include "QSqlDatabase"
#include "QDebug"
#include <QtSql>
#include <QMessageBox>
#include <QSqlError>

database::database()
{

}

UserData readUserDataFromMySQL()
{
    UserData userData;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "SQLITE");
    db.setHostName("localhost");
    db.setDatabaseName("a0815686_JEducation");
    db.setUserName("a0815686_JEducation");
    db.setPassword("5S6InVJJ");

    if (db.open()) {
    qDebug() << "Database error:" << db.lastError();
    QSqlQuery query;

    if (query.exec("SELECT name, rating FROM Jeducation WHERE id = 1")) {

        if (query.next()) {
            userData.name = query.value("name").toString();
            userData.rating = query.value("rating").toInt();
            }
        }
        else {
            qDebug() << "Ошибка выполнения SQL-запроса:" << query.lastError().text();
        }

        db.close();}
    else {
    qDebug() << "Ошибка подключения к базе данных:" << db.lastError().text();
    }
    return userData;

}

void sendUserDataToMySQL(const QString& name, int rating)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "SQLITE");
    db.setHostName("localhost");
    db.setDatabaseName("a0815686_JEducation");
    db.setUserName("a0815686_JEducation");
    db.setPassword("5S6InVJJ");

    if (db.open()) {

        QSqlQuery query;

        query.prepare("INSERT INTO Jeducation (name, rating) VALUES (:name, :rating)");
        query.bindValue(":name", name);
        query.bindValue(":rating", rating);

        if (query.exec()) {
        } else {
        qDebug() << "Ошибка выполнения SQL-запроса:" << query.lastError().text();
        }

        db.close();
    } else {

        qDebug() << "Ошибка подключения к базе данных:" << db.lastError().text();
    }
}
