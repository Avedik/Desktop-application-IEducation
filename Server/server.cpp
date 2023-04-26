#include "server.h"
#include "serverworker.h"
#include "meeting.h"
#include <QThread>
#include <functional>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QTimer>

Server::Server(QObject *parent): QTcpServer(parent){
    for (int i = 0; i < 2; ++i)
        m_modes.push_back(new QVector<Meeting*>());
}

void Server::allocateUser(ServerWorker *sender, const QString& ID)
{
    if (ID.size() <= 2 || (!ID.startsWith("0:") && !ID.startsWith("1:")))
    {
        QJsonObject messageToSender;
        messageToSender[QStringLiteral("тип")] = QStringLiteral("логин");
        messageToSender[QStringLiteral("успешно")] = false;
        messageToSender[QStringLiteral("причина")] = QStringLiteral("Неверный ID");
        sendJson(sender, messageToSender);
        return;
    }

    sender->setMode(ID.mid(0,1).toInt());
    QString meetingID = ID.mid(2, ID.length() - 2);
    int id = 0;
    bool isGoodID = true;

    if (meetingID != QStringLiteral("Новое собрание"))
    {
        id = meetingID.toInt(&isGoodID);
        if (isGoodID && 0 <= id && id < m_modes.at(sender->getMode())->size())
        {
            if (m_modes.at(sender->getMode())->at(id)->getStatus()) {
                waitingUsers.removeAll(sender);
                Meeting* meeting = m_modes.at(sender->getMode())->at(id);
                meeting->addClient(sender);
                sender->setMeeting(meeting);
            } else
            {
                QJsonObject messageToSender;
                messageToSender[QStringLiteral("тип")] = QStringLiteral("логин");
                messageToSender[QStringLiteral("успешно")] = false;
                messageToSender[QStringLiteral("причина")] = QStringLiteral("К этому собранию уже нельзя присоединиться");
                sendJson(sender, messageToSender);
                return;
            }
        }
        else
        {
            QJsonObject messageToSender;
            messageToSender[QStringLiteral("тип")] = QStringLiteral("логин");
            messageToSender[QStringLiteral("успешно")] = false;
            messageToSender[QStringLiteral("причина")] = QStringLiteral("Собрания с таким ID не существует");
            sendJson(sender, messageToSender);
            return;
        }
    }
    else
    {
        Meeting* newMeeting = new Meeting ({sender});
        m_modes.at(sender->getMode())->push_back(newMeeting);
        id = m_modes.at(sender->getMode())->size() - 1;
        sender->setMeeting(newMeeting);
        waitingUsers.removeAll(sender);
    }
    QJsonObject connectedMessage;
    connectedMessage[QStringLiteral("тип")] = QStringLiteral("новый пользователь");
    connectedMessage[QStringLiteral("имя пользователя")] = QStringLiteral("");
    connectedMessage[QStringLiteral("ID собрания")] = QString::number(id);
    sendJson(sender, connectedMessage);
}

void Server::jsonFromLoggedOut(ServerWorker *sender, const QJsonObject &docObj)
{
    Q_ASSERT(sender);
    const QJsonValue typeVal = docObj.value(QStringLiteral("тип"));
    if (typeVal.isNull() || !typeVal.isString())
        return;
    if (typeVal.toString().compare(QStringLiteral("распределение"), Qt::CaseInsensitive) == 0)
    {
        const QJsonValue ID = docObj.value(QStringLiteral("ID собрания"));
        if (ID.isNull() || !ID.isString())
            return;
        allocateUser(sender, ID.toString());
        return;
    }

    if (sender->getMeeting() == nullptr)
    {
        QJsonObject messageToSender;
        messageToSender[QStringLiteral("тип")] = QStringLiteral("логин");
        messageToSender[QStringLiteral("успешно")] = false;
        messageToSender[QStringLiteral("причина")] = QStringLiteral("Не задан ID");
        sendJson(sender, messageToSender);
        return;
    }

    if (typeVal.toString().compare(QStringLiteral("логин"), Qt::CaseInsensitive) != 0)
        return;

    const QJsonValue usernameVal = docObj.value(QStringLiteral("имя пользователя"));
    if (usernameVal.isNull() || !usernameVal.isString())
        return;
    const QString newUserName = usernameVal.toString().simplified();
    if (newUserName.isEmpty())
        return;

    for (ServerWorker *worker : *sender->getMeeting()) {
        if (worker == sender)
            continue;
        if (worker->userName().compare(newUserName, Qt::CaseInsensitive) == 0) {
            QJsonObject message;
            message[QStringLiteral("тип")] = QStringLiteral("логин");
            message[QStringLiteral("успешно")] = false;
            message[QStringLiteral("причина")] = QStringLiteral("Имя пользователя уже существует");
            sendJson(sender, message);
            return;
        }
    }
    sender->setUserName(newUserName);
    QJsonObject successMessage;
    successMessage[QStringLiteral("тип")] = QStringLiteral("логин");
    successMessage[QStringLiteral("успешно")] = true;
    successMessage[QStringLiteral("имя пользователя")] = newUserName;
    sendJson(sender, successMessage);

    QJsonObject connectedMessage;
    connectedMessage[QStringLiteral("тип")] = QStringLiteral("новый пользователь");
    connectedMessage[QStringLiteral("имя пользователя")] = newUserName;
    connectedMessage[QStringLiteral("ID собрания")] = QStringLiteral("");
    broadcast(connectedMessage, sender);

    QJsonObject messageToAll;
    messageToAll[QStringLiteral("тип")] = QStringLiteral("подключение");
    messageToAll[sender->userName()] = "";
    broadcast(messageToAll, sender);

    QJsonObject messageToSender;
    messageToSender[QStringLiteral("тип")] = QStringLiteral("подключение");
    for (ServerWorker *_worker : *sender->getMeeting())
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
        for (ServerWorker *worker : *sender->getMeeting()) {
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
    for (ServerWorker *worker : *exclude->getMeeting()) {
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
        for (ServerWorker *worker : *sender->getMeeting()) {
            Q_ASSERT(worker);
            worker->sendImage(img, source);
        }
}

void Server::pointReceived(ServerWorker *sender, const QPointF &point, qint32 operationCode)
{
    Q_ASSERT(sender);
    emit logMessage(QStringLiteral("Point получен ") );
    if (!sender->userName().isEmpty())
        for (ServerWorker *worker : *sender->getMeeting()) {
            Q_ASSERT(worker);
            if (sender == worker)
                continue;
            worker->sendPoint(point, operationCode);
        }
}

void Server::colorReceived(ServerWorker *sender, const QColor& color)
{
    Q_ASSERT(sender);
    emit logMessage(QStringLiteral("Color получен ") );
    if (!sender->userName().isEmpty())
        for (ServerWorker *worker : *sender->getMeeting()) {
            Q_ASSERT(worker);
            if (sender == worker)
                continue;
            worker->sendColor(color);
        }
}

void Server::pdfReceived(ServerWorker *sender, const QByteArray &data)
{
    Q_ASSERT(sender);
    emit logMessage(QStringLiteral("PDF получен ") );
    if (!sender->userName().isEmpty())
        for (ServerWorker *worker : *sender->getMeeting()) {
            Q_ASSERT(worker);
            worker->sendPDF(data);
        }
}

void Server::userDisconnected(ServerWorker *sender)
{
    const QString userName = sender->userName();
    if (!userName.isEmpty()) {
        QJsonObject disconnectedMessage;
        disconnectedMessage[QStringLiteral("тип")] = QStringLiteral("пользователь отключился");
        disconnectedMessage[QStringLiteral("имя пользователя")] = userName;
        broadcast(disconnectedMessage, sender);
        emit logMessage(userName + QStringLiteral(" отключен"));
    }

    if (sender->getMeeting() != nullptr)
    {
        QJsonObject message;
        message[QStringLiteral("тип")] = QStringLiteral("отсоединение");
        message[sender->userName()] = "";
        broadcast(message, sender);
    }
    sender->deleteLater();

    if (waitingUsers.contains(sender))
        waitingUsers.removeAll(sender);
    else {
        Meeting* meeting = sender->getMeeting();
        meeting->removeClient(sender);
        if (meeting->size() == 0)
        {
            m_modes.at(sender->getMode())->removeAll(meeting);
            if (!meeting->isDeleted())
                delete meeting;
        }
    }
}

void Server::userError(ServerWorker *sender)
{
    Q_UNUSED(sender)
    emit logMessage(QStringLiteral("Ошибка от ") + sender->userName());
}

void Server::userReceiveFile(ServerWorker *sender)
{
    Meeting *meeting = sender->getMeeting();
    meeting->increaseCountOfUsersWithFile();
    if (meeting->getCountOfUsersWithFile() == meeting->size())
    {
        meeting->setStatus(false);
        for (ServerWorker *worker : *meeting)
            worker->sendServiceInfo();
    }
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
    connect(worker, &ServerWorker::pointReceived, this, std::bind(&Server::pointReceived, this,
                                                                  worker, std::placeholders::_1, std::placeholders::_2));
    connect(worker, &ServerWorker::colorReceived, this, std::bind(&Server::colorReceived, this, worker, std::placeholders::_1));
    connect(worker, &ServerWorker::pdfReceived, this, std::bind(&Server::pdfReceived, this, worker, std::placeholders::_1));
    connect(worker, &ServerWorker::logMessage, this, &Server::logMessage);
    connect(worker, &ServerWorker::userReceiveFile, this, std::bind(&Server::userReceiveFile, this, worker));

    waitingUsers.push_back(worker);
    emit logMessage(QStringLiteral("Новый клиент подключился"));
}

void Server::stopServer()
{
    while (waitingUsers.size())
        waitingUsers.back()->disconnectFromClient();

    for (QVector<Meeting*>* mode : m_modes) {
        while (mode->size()) {
            Meeting* meeting = mode->back();
            delete meeting;
            mode->removeAll(meeting);
        }
    }
    close();
}

Server::~Server()
{
    stopServer();
    while (m_modes.size()) {
        QVector<Meeting*>* mode = m_modes.back();
        m_modes.removeAll(mode);
        delete mode;
    }
}



