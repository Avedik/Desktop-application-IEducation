#include "meeting.h"
#include "serverworker.h"

Meeting::Meeting(): status(true), deleting(false),
    clients(new QVector<ServerWorker *>())
{
}

Meeting::Meeting(const QVector<ServerWorker *>& meeting): status(true),
    clients(new QVector<ServerWorker *>()) {
    for (ServerWorker *worker : meeting)
        clients->push_back(worker);
}

Meeting::~Meeting()
{
    deleting = true;
    while (clients->size()) {
        ServerWorker* worker = clients->back();
        worker->disconnectFromClient();
    }
    delete clients;
}

qint32 Meeting::size()
{
    return clients->size();
}

void Meeting::addClient(ServerWorker *client)
{
    if (status)
        clients->push_back(client);
}

void Meeting::removeClient(ServerWorker *client)
{
    clients->removeAll(client);
}

void Meeting::setStatus(bool status)
{
    this->status = status;
}

bool Meeting::getStatus()
{
    return status;
}

bool Meeting::isDeleted()
{
    return deleting;
}

void Meeting::increaseCountOfUsersWithFile()
{
    ++numberOfUsersWithFile;
}

qint32 Meeting::getCountOfUsersWithFile()
{
    return numberOfUsersWithFile;
}

QVector<ServerWorker*>::iterator Meeting::begin()
{
    return clients->begin();
}

QVector<ServerWorker*>::iterator Meeting::end()
{
    return clients->end();
}
