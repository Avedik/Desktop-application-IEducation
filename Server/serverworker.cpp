#include "serverworker.h"
#include "dataTypes.h"
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QBuffer>
#include <QImage>

ServerWorker::ServerWorker(QObject *parent)
    : QObject(parent),
    m_serverSocket(new QTcpSocket(this)),
    meeting(nullptr)
{
    connect(m_serverSocket, &QTcpSocket::readyRead, this, &ServerWorker::onReadyRead);
    connect(m_serverSocket, &QTcpSocket::disconnected, this, &ServerWorker::disconnectedFromClient);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    connect(m_serverSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &ServerWorker::error);
#else
    connect(m_serverSocket, &QAbstractSocket::errorOccurred, this, &ServerWorker::error);
#endif
}


bool ServerWorker::setSocketDescriptor(qintptr socketDescriptor)
{
    return m_serverSocket->setSocketDescriptor(socketDescriptor);
}

void ServerWorker::setMeeting(Meeting* meeting)
{
    this->meeting = meeting;
}

Meeting* ServerWorker::getMeeting()
{
    return meeting;
}

void ServerWorker::setMode(qint32 mode)
{
    this->mode = mode;
}

qint32 ServerWorker::getMode()
{
    return mode;
}

void ServerWorker::sendJson(const QJsonObject &json)
{
    const QByteArray jsonData = QJsonDocument(json).toJson(QJsonDocument::Compact);
    emit logMessage(QStringLiteral("Отправка ") + userName() + QLatin1String(" - ") + QString::fromUtf8(jsonData));
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    socketStream << DataTypes::JSON << jsonData;
}

void ServerWorker::sendImage(const QImage& image, const QString& source)
{
    emit logMessage(QStringLiteral("Отправка ") + userName() + QLatin1String(" - Image"));
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    socketStream << DataTypes::IMAGE << image << source;
}

void ServerWorker::sendPoint(const QPointF& point, qint32 operationCode)
{
    emit logMessage(QStringLiteral("Отправка ") + userName() + QLatin1String(" - Point"));
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    socketStream << DataTypes::POINT << operationCode << point;
}

void ServerWorker::sendPDF(const QByteArray &data)
{
    emit logMessage(QStringLiteral("Отправка ") + userName() + QLatin1String(" - PDF"));
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    socketStream << DataTypes::PDF_FILE << data;
}

void ServerWorker::sendServiceInfo()
{
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    socketStream << DataTypes::FILE_SEND_CODE;
}

void ServerWorker::disconnectFromClient()
{
    m_serverSocket->abort();
}

QString ServerWorker::userName() const
{
    return m_userName;
}

void ServerWorker::setUserName(const QString &userName)
{
    m_userName = userName;
}

void ServerWorker::receiveJson(QDataStream& socketStream)
{
    QByteArray jsonData;
    socketStream >> jsonData;

    if (socketStream.commitTransaction()) {
        QJsonParseError parseError;
        const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
        if (parseError.error == QJsonParseError::NoError) {
            if (jsonDoc.isObject())
                emit jsonReceived(jsonDoc.object());
            else
                emit logMessage(QStringLiteral("Неверное сообщение: ") + QString::fromUtf8(jsonData));
        } else {
            emit logMessage(QStringLiteral("Неверное сообщение: ") + QString::fromUtf8(jsonData));
        }

        socketStream.startTransaction();
    }

}

void ServerWorker::receiveImage(QDataStream& socketStream)
{
    QImage img;
    socketStream >> img;
    if (socketStream.status() != QDataStream::Ok)
    {
        socketStream.commitTransaction();
        return;
    }
    socketStream.commitTransaction();
    emit imageReceived(img, userName());
}

void ServerWorker::receivePoint(QDataStream& socketStream)
{
    qint32 operationCode;
    QPointF point;
    socketStream >> operationCode >> point;
    if (socketStream.status() != QDataStream::Ok)
    {
        socketStream.commitTransaction();
        return;
    }
    socketStream.commitTransaction();
    emit pointReceived(point, operationCode);
}

void ServerWorker::receivePDF(QDataStream& socketStream)
{
    QByteArray data;
    socketStream >> data;
    if (socketStream.status() != QDataStream::Ok)
    {
        socketStream.commitTransaction();
        return;
    }
    socketStream.commitTransaction();
    emit pdfReceived(data);
}

void ServerWorker::onReadyRead()
{
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);

    DataTypes _type = DataTypes::JSON;
    socketStream.startTransaction();
    socketStream >> _type;
    if (socketStream.status() != QDataStream::Ok)
    {
        socketStream.commitTransaction();
        return;
    }

    if (_type == DataTypes::FILE_RECEIVE_CODE)
        emit userReceiveFile();
    else if (_type == DataTypes::PDF_FILE)
        receivePDF(socketStream);
    else if (_type == DataTypes::JSON)
        receiveJson(socketStream);
    else if (_type == DataTypes::IMAGE)
        receiveImage(socketStream);
    else if (_type == DataTypes::POINT)
        receivePoint(socketStream);
    else
        socketStream.commitTransaction();
}


