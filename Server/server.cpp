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

    QJsonObject messageToAll;
    messageToAll[QStringLiteral("тип")] = QStringLiteral("подключение");
    messageToAll[sender->userName()] = "";
    broadcast(messageToAll, sender);

    QJsonObject messageToSender;
    messageToSender[QStringLiteral("тип")] = QStringLiteral("подключение");
    for (ServerWorker *_worker : m_clients)
        messageToSender[_worker->userName()] = "";
    sendJson(sender, messageToSender);
}

void Server::jsonFromLoggedIn(ServerWorker *sender, const QJsonObject &docObj)
{
    Q_ASSERT(sender);
    const QJsonValue typeVal = docObj.value(QStringLiteral("тип"));
    if (typeVal.isNull() || !typeVal.isString())
        return;
    const QJsonValue receiverVal = docObj.value(QStringLiteral("получатель"));
    if (!receiverVal.isNull() && receiverVal.isString() && sender->userName().compare(receiverVal.toString()) == 0)
        return;

    QJsonObject message;
    auto varMap = docObj.toVariantMap();
    for(QVariantMap::const_iterator iter = varMap.begin(); iter != varMap.end(); ++iter)
        message[iter.key()] = iter.value().toString();

    message[QStringLiteral("отправитель")] = sender->userName();
    if (typeVal.toString().compare(QStringLiteral("вопрос")) == 0)
        for (ServerWorker *worker : m_clients) {
            Q_ASSERT(worker);
            if (worker->userName().compare(receiverVal.toString()) == 0)
            {
                sendJson(worker, message);
                break;
            }
        }
    else
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

void Server::imageReceived(ServerWorker *sender, const QImage &img, const QString& source)
{
    Q_ASSERT(sender);
    emit logMessage(QStringLiteral("Image получен ") );
    if (!sender->userName().isEmpty())
        for (ServerWorker *worker : m_clients) {
            Q_ASSERT(worker);
            worker->sendImage(img, source);
        }
}
void Server::pdfReceived(ServerWorker *sender, const QByteArray &data)
{
    Q_ASSERT(sender);
    emit logMessage(QStringLiteral("PDF получен ") );
    if (!sender->userName().isEmpty())
        for (ServerWorker *worker : m_clients) {
            Q_ASSERT(worker);
            if (worker != sender)
                worker->sendPDF(data);
    }
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

    QJsonObject message;
    message[QStringLiteral("тип")] = QStringLiteral("отсоединение");
    message[sender->userName()] = "";
    broadcast(message, sender);

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
    connect(worker, &ServerWorker::imageReceived, this, std::bind(&Server::imageReceived, this,
                                                                  worker, std::placeholders::_1, std::placeholders::_2));
    connect(worker, &ServerWorker::pdfReceived, this, std::bind(&Server::pdfReceived, this, worker, std::placeholders::_1));
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



