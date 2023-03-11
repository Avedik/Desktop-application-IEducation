#include "server.h"
#include "serverworker.h"
#include <QThread>
#include <functional>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QTimer>

Server::Server(QObject *parent): QTcpServer(parent){}

void Server::jsonFromLoggedOut(ServerWorker *sender, const QJsonObject &docObj)
{
    Q_ASSERT(sender);
    const QJsonValue typeVal = docObj.value(QStringLiteral("тип"));
    if (typeVal.isNull() || !typeVal.isString())
        return;
    if (typeVal.toString().compare(QStringLiteral("логин"), Qt::CaseInsensitive) != 0)
        return;
    const QJsonValue usernameVal = docObj.value(QStringLiteral("имя пользователя"));
    if (usernameVal.isNull() || !usernameVal.isString())
        return;
    const QString newUserName = usernameVal.toString().simplified();
    if (newUserName.isEmpty())
        return;
    for (ServerWorker *worker : qAsConst(m_clients)) {
        if (worker == sender)
            continue;
        if (worker->userName().compare(newUserName, Qt::CaseInsensitive) == 0) {
            QJsonObject message;
            message[QStringLiteral("тип")] = QStringLiteral("логин");
            message[QStringLiteral("успешно")] = false;
            message[QStringLiteral("причина")] = QStringLiteral("имя пользователя уже существует");
            sendJson(sender, message);
            return;
        }
    }
    sender->setUserName(newUserName);
    QJsonObject successMessage;
    successMessage[QStringLiteral("тип")] = QStringLiteral("логин");
    successMessage[QStringLiteral("успешно")] = true;
    sendJson(sender, successMessage);
    QJsonObject connectedMessage;
    connectedMessage[QStringLiteral("тип")] = QStringLiteral("новый пользователь");
    connectedMessage[QStringLiteral("новый пользователь")] = newUserName;
    broadcast(connectedMessage, sender);
}

void Server::jsonFromLoggedIn(ServerWorker *sender, const QJsonObject &docObj)
{
    Q_ASSERT(sender);
    const QJsonValue typeVal = docObj.value(QStringLiteral("тип"));
    if (typeVal.isNull() || !typeVal.isString())
        return;
    const QJsonValue textVal = docObj.value(QStringLiteral("текст"));
    if (textVal.isNull() || !textVal.isString())
        return;
    const QString text = textVal.toString().trimmed();
    if (text.isEmpty())
        return;
    QJsonObject message;
    message[QStringLiteral("тип")] =typeVal.toString();
    message[QStringLiteral("текст")] = text;
    message[QStringLiteral("отправитель")] = sender->userName();
    broadcast(message, sender);
}

void Server::sendJson(ServerWorker *destination, const QJsonObject &message)
{
    Q_ASSERT(destination);
    destination->sendJson(message);
}

void Server::broadcast(const QJsonObject &message, ServerWorker *exclude)
{
    for (ServerWorker *worker : m_clients) {
        Q_ASSERT(worker);
        if (worker == exclude)
            continue;
        sendJson(worker, message);
    }
}

void Server::jsonReceived(ServerWorker *sender, const QJsonObject &doc)
{
    Q_ASSERT(sender);
    emit logMessage(QStringLiteral("JSON получены ") + QString::fromUtf8(QJsonDocument(doc).toJson()));
    if (sender->userName().isEmpty())
        return jsonFromLoggedOut(sender, doc);
    jsonFromLoggedIn(sender, doc);
}

void Server::userDisconnected(ServerWorker *sender)
{
    m_clients.removeAll(sender);
    const QString userName = sender->userName();
    if (!userName.isEmpty()) {
        QJsonObject disconnectedMessage;
        disconnectedMessage[QStringLiteral("тип")] = QStringLiteral("пользователь отключился");
        disconnectedMessage[QStringLiteral("новый пользователь")] = userName;
        broadcast(disconnectedMessage, nullptr);
        emit logMessage(userName + QStringLiteral(" отключен"));
    }
    sender->deleteLater();
}

void Server::userError(ServerWorker *sender)
{
    Q_UNUSED(sender)
    emit logMessage(QStringLiteral("Ошибка от ") + sender->userName());
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    ServerWorker *worker = new ServerWorker(this);
    if (!worker->setSocketDescriptor(socketDescriptor)) {
        worker->deleteLater();
        return;
    }
    connect(worker, &ServerWorker::disconnectedFromClient, this, std::bind(&Server::userDisconnected, this, worker));
    connect(worker, &ServerWorker::error, this, std::bind(&Server::userError, this, worker));
    connect(worker, &ServerWorker::jsonReceived, this, std::bind(&Server::jsonReceived, this, worker, std::placeholders::_1));
    connect(worker, &ServerWorker::logMessage, this, &Server::logMessage);
    m_clients.append(worker);
    emit logMessage(QStringLiteral("Новый клиент подключился"));
}

void Server::stopServer()
{
    for (ServerWorker *worker : m_clients) {
        worker->disconnectFromClient();
    }
    close();
}



