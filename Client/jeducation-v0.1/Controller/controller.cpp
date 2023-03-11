#include "Controller/controller.h"
#include <QTcpSocket>
#include <QDataStream>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

Controller::Controller(QObject *parent)
    : QObject(parent)
    , m_clientSocket(new QTcpSocket(this))
    , m_loggedIn(false)
{

    connect(m_clientSocket, &QTcpSocket::connected, this, &Controller::connected);
    connect(m_clientSocket, &QTcpSocket::disconnected, this, &Controller::disconnected);
    connect(m_clientSocket, &QTcpSocket::readyRead, this, &Controller::onReadyRead);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    connect(m_clientSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &Controller::error);
#else
    connect(m_clientSocket, &QAbstractSocket::errorOccurred, this, &Controller::error);
#endif

    connect(m_clientSocket, &QTcpSocket::disconnected, this, [this]()->void{m_loggedIn = false;});
}

void Controller::login(const QString &userName)
{
    if (m_clientSocket->state() == QAbstractSocket::ConnectedState) {
        QDataStream clientStream(m_clientSocket);
        clientStream.setVersion(QDataStream::Qt_5_7);
        QJsonObject message;
        message[QStringLiteral("тип")] = QStringLiteral("логин");
        message[QStringLiteral("имя пользователя")] = userName;
        clientStream << QJsonDocument(message).toJson(QJsonDocument::Compact);
    }
}

void Controller::sendMessage(const QString &text)
{
    if (text.isEmpty())
        return;
    QDataStream clientStream(m_clientSocket);
    clientStream.setVersion(QDataStream::Qt_5_7);
    QJsonObject message;
    message[QStringLiteral("тип")] = QStringLiteral("сообщение");
    message[QStringLiteral("текст")] = text;
    clientStream << QJsonDocument(message).toJson();
}

void Controller::sendQuestion(const QString &text)
{
    if (text.isEmpty())
        return;
    QDataStream clientStream(m_clientSocket);
    clientStream.setVersion(QDataStream::Qt_5_7);
    QJsonObject message;
    message[QStringLiteral("тип")] = QStringLiteral("вопрос");
    message[QStringLiteral("текст")] = text;
    clientStream << QJsonDocument(message).toJson();
}

void Controller::sendAnswer(const QString &source, const QString &question, const QString &answer)
{
    if (answer.isEmpty())
        return;
    QDataStream clientStream(m_clientSocket);
    clientStream.setVersion(QDataStream::Qt_5_7);
    QJsonObject message;
    message[QStringLiteral("тип")] = QStringLiteral("ответ");
    message[QStringLiteral("вопрос")] = question;
    message[QStringLiteral("ответ")] = answer;
    clientStream << QJsonDocument(message).toJson();
}

void Controller::disconnectFromHost()
{
    m_clientSocket->disconnectFromHost();
}

void Controller::jsonReceived(const QJsonObject &docObj)
{
    const QJsonValue типVal = docObj.value(QStringLiteral("тип"));
    if (типVal.isNull() || !типVal.isString())
        return;
    if (типVal.toString().compare(QStringLiteral("логин"), Qt::CaseInsensitive) == 0) {
        if (m_loggedIn)
            return;
        const QJsonValue resultVal = docObj.value(QStringLiteral("успешно"));
        if (resultVal.isNull() || !resultVal.isBool())
            return;
        const bool loginSuccess = resultVal.toBool();
        if (loginSuccess) {
            emit loggedIn();
            return;
        }

        const QJsonValue reasonVal = docObj.value(QStringLiteral("причина"));
        emit loginError(reasonVal.toString());
    } else if (типVal.toString().compare(QStringLiteral("сообщение"), Qt::CaseInsensitive) == 0) {
        const QJsonValue textVal = docObj.value(QStringLiteral("текст"));
        const QJsonValue senderVal = docObj.value(QStringLiteral("отправитель"));
        if (textVal.isNull() || !textVal.isString())
            return;
        if (senderVal.isNull() || !senderVal.isString())
            return;

        emit messageReceived(senderVal.toString(), textVal.toString());
    } else if (типVal.toString().compare(QStringLiteral("вопрос"), Qt::CaseInsensitive) == 0) {

        const QJsonValue textVal = docObj.value(QStringLiteral("текст"));
        const QJsonValue senderVal = docObj.value(QStringLiteral("отправитель"));
        if (textVal.isNull() || !textVal.isString())
            return;
        if (senderVal.isNull() || !senderVal.isString())
            return;

        emit questionReceived(senderVal.toString(), textVal.toString());
    } else if (типVal.toString().compare(QStringLiteral("новый пользователь"), Qt::CaseInsensitive) == 0) {

        const QJsonValue usernameVal = docObj.value(QStringLiteral("имя пользователя"));
        if (usernameVal.isNull() || !usernameVal.isString())
            return;
        emit userJoined(usernameVal.toString());
    } else if (типVal.toString().compare(QStringLiteral("пользователь отключился"), Qt::CaseInsensitive) == 0) {
        const QJsonValue usernameVal = docObj.value(QStringLiteral("имя пользователя"));
        if (usernameVal.isNull() || !usernameVal.isString())
            return;
        emit userLeft(usernameVal.toString());
    }
}

void Controller::connectToServer(const QHostAddress &address, quint16 port)
{
    m_clientSocket->connectToHost(address, port);
}

void Controller::onReadyRead()
{
    QByteArray jsonData;
    QDataStream socketStream(m_clientSocket);

    socketStream.setVersion(QDataStream::Qt_5_7);
    for (;;) {
        socketStream.startTransaction();
        socketStream >> jsonData;
        if (socketStream.commitTransaction()) {
            QJsonParseError parseError;
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                if (jsonDoc.isObject())
                    jsonReceived(jsonDoc.object());
            }
        } else {
            break;
        }
    }
}
