#include "Controller/controller.h"
#include "dataTypes.h"
#include <QTcpSocket>
#include <QDataStream>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QBuffer>

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
        clientStream << DataTypes::JSON << QJsonDocument(message).toJson(QJsonDocument::Compact);
    }
}

void Controller::chooseMeeting(const QString &ID)
{
    if (m_clientSocket->state() == QAbstractSocket::ConnectedState) {
        QDataStream clientStream(m_clientSocket);
        clientStream.setVersion(QDataStream::Qt_5_7);
        QJsonObject message;
        message[QStringLiteral("тип")] = QStringLiteral("распределение");
        message[QStringLiteral("ID собрания")] = ID;
        clientStream << DataTypes::JSON << QJsonDocument(message).toJson(QJsonDocument::Compact);
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
    clientStream << DataTypes::JSON << QJsonDocument(message).toJson();
}

void Controller::sendImage(const QImage& image)
{
    QDataStream socketStream(m_clientSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    socketStream << DataTypes::IMAGE << image;
}

void Controller::sendQuestion(const QString &destUser, const QString &text)
{
    if (text.isEmpty())
        return;
    QDataStream clientStream(m_clientSocket);
    clientStream.setVersion(QDataStream::Qt_5_7);
    QJsonObject message;
    message[QStringLiteral("тип")] = QStringLiteral("вопрос");
    message[QStringLiteral("текст")] = text;
    message[QStringLiteral("получатель")] = destUser;
    clientStream << DataTypes::JSON << QJsonDocument(message).toJson();
}

void Controller::sendAnswer(const QString &source, const QString &question, const QString &answer)
{
    if (answer.isEmpty())
        return;
    QDataStream clientStream(m_clientSocket);
    clientStream.setVersion(QDataStream::Qt_5_7);
    QJsonObject message;
    message[QStringLiteral("тип")] = QStringLiteral("ответ");
    message[QStringLiteral("источник")] = source;
    message[QStringLiteral("вопрос")] = question;
    message[QStringLiteral("ответ")] = answer;
    clientStream << DataTypes::JSON << QJsonDocument(message).toJson();
}

void Controller::sendPDF(const QByteArray &data)
{
    QDataStream socketStream(m_clientSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    socketStream << DataTypes::PDF_FILE << data;
}

void Controller::disconnectFromHost()
{
    m_clientSocket->disconnectFromHost();
}

void Controller::jsonReceived(const QJsonObject &docObj)
{
    const QJsonValue typeVal = docObj.value(QStringLiteral("тип"));
    if (typeVal.isNull() || !typeVal.isString())
        return;
    if (typeVal.toString().compare(QStringLiteral("логин"), Qt::CaseInsensitive) == 0) {
        if (m_loggedIn)
            return;
        const QJsonValue resultVal = docObj.value(QStringLiteral("успешно"));
        if (resultVal.isNull() || !resultVal.isBool())
            return;
        const bool loginSuccess = resultVal.toBool();
        if (loginSuccess) {;
            emit loggedIn(docObj.value(QStringLiteral("имя пользователя")).toString());
            return;
        }

        const QJsonValue reasonVal = docObj.value(QStringLiteral("причина"));
        emit loginError(reasonVal.toString());
    } else if (typeVal.toString().compare(QStringLiteral("сообщение"), Qt::CaseInsensitive) == 0) {
        const QJsonValue textVal = docObj.value(QStringLiteral("текст"));
        const QJsonValue senderVal = docObj.value(QStringLiteral("отправитель"));
        if (textVal.isNull() || !textVal.isString())
            return;
        if (senderVal.isNull() || !senderVal.isString())
            return;

        emit messageReceived(senderVal.toString(), textVal.toString());
    } else if (typeVal.toString().compare(QStringLiteral("вопрос"), Qt::CaseInsensitive) == 0) {

        const QJsonValue textVal = docObj.value(QStringLiteral("текст"));
        const QJsonValue senderVal = docObj.value(QStringLiteral("отправитель"));
        if (textVal.isNull() || !textVal.isString())
            return;
        if (senderVal.isNull() || !senderVal.isString())
            return;

        emit questionReceived(senderVal.toString(), textVal.toString());
    } else if (typeVal.toString().compare(QStringLiteral("ответ"), Qt::CaseInsensitive) == 0) {

        const QJsonValue sourceVal = docObj.value(QStringLiteral("источник"));
        const QJsonValue senderVal = docObj.value(QStringLiteral("отправитель"));
        const QJsonValue questionVal = docObj.value(QStringLiteral("вопрос"));
        const QJsonValue answerVal = docObj.value(QStringLiteral("ответ"));

        if (sourceVal.isNull() || !sourceVal.isString()
            || senderVal.isNull() || !senderVal.isString()
            || questionVal.isNull() || !questionVal.isString()
            || answerVal.isNull() || !answerVal.isString())
            return;

        emit answerReceived(sourceVal.toString(), senderVal.toString(), questionVal.toString(), answerVal.toString());
    } else if (typeVal.toString().compare(QStringLiteral("подключение"), Qt::CaseInsensitive) == 0
               || typeVal.toString().compare(QStringLiteral("отсоединение"), Qt::CaseInsensitive) == 0) {

        emit refreshUsersList(docObj.toVariantMap(), typeVal.toString());
    } else if (typeVal.toString().compare(QStringLiteral("новый пользователь"), Qt::CaseInsensitive) == 0) {

        const QJsonValue usernameVal = docObj.value(QStringLiteral("имя пользователя"));
        if (usernameVal.isNull() || !usernameVal.isString())
            return;
        emit userJoined(usernameVal.toString());
    } else if (typeVal.toString().compare(QStringLiteral("пользователь отключился"), Qt::CaseInsensitive) == 0) {
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
    QDataStream socketStream(m_clientSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);

    DataTypes _type = DataTypes::JSON;
    socketStream.startTransaction();
    socketStream >> _type;
    if (socketStream.status() != QDataStream::Ok)
    {
        socketStream.commitTransaction();
        return;
    }

    if (_type == DataTypes::FILE_SEND_CODE)
        emit fileSentOut();
    else if (_type == DataTypes::PDF_FILE)
    {
        QByteArray data;
        socketStream >> data;
        if (socketStream.status() != QDataStream::Ok)
        {
            socketStream.commitTransaction();
            return;
        }
        socketStream.commitTransaction();
        emit receivePDF(data);
    }
    else if (_type == DataTypes::JSON) {
    QByteArray jsonData;
    for (;;) {
        socketStream >> jsonData;
        if (socketStream.commitTransaction()) {
            QJsonParseError parseError;
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                if (jsonDoc.isObject())
                    jsonReceived(jsonDoc.object());
            }
            socketStream.startTransaction();
        } else {
            break;
        }
    }
    }
    else if (_type == DataTypes::IMAGE)
    {
        QImage img;
        QString source;

        socketStream >> img >> source;
        if (!socketStream.commitTransaction())
            return;
        emit receiveImage(img, source);
    }
    else
        socketStream.commitTransaction();
}

void Controller::fileReceived()
{
    QDataStream socketStream(m_clientSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    socketStream << DataTypes::FILE_RECEIVE_CODE;
}
