#ifndef MEETING_H
#define MEETING_H

#include <QObject>
#include <QVector>

class ServerWorker;
class Meeting : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Meeting)
public:
    Meeting();
    Meeting(const QVector<ServerWorker *>& meeting);
    ~Meeting();
    void addClient(ServerWorker *client);
    void removeClient(ServerWorker *client);
    void setStatus(bool status);
    bool getStatus();
    bool isDeleted();
    qint32 size();

    QVector<ServerWorker*>::iterator begin();
    QVector<ServerWorker*>::iterator end();

private:
    bool status;
    bool deleting;
    QVector<ServerWorker *>* clients;
};

#endif // MEETING_H
