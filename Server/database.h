#ifndef DATABASE_H
#define DATABASE_H

#include <QString>

struct UserData {
    QString name;
    int rating;
};

class database
{
    public:
    database();
    UserData readUserDataFromMySQL();
    void sendUserDataToMySQL(const QString& name, int rating);
};

#endif // DATABASE_H
