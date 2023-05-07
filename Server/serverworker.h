#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include <QObject>
#include <QTcpSocket>
#include <QImage>
#include <QColor>
#include <QPointF>
#include "dataTypes.h"

class QJsonObject;
class Meeting;
class ServerWorker : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ServerWorker)

    //Метод по чтению входящих сообщений из сети и отправке на главный серверный объект
    bool receiveJson(QDataStream& socketStream);
    bool receiveImage(QDataStream& socketStream);
    bool receivePoint(QDataStream& socketStream);
    bool receiveFile(DataTypes dataType, QDataStream& socketStream);
    bool receiveColor(QDataStream& socketStream);
public:
    explicit ServerWorker(QObject *parent = nullptr);
    virtual bool setSocketDescriptor(qintptr socketDescriptor);
    QString userName() const;
    //Способ получения и установки имени пользователя
    void setUserName(const QString &userName);
    void sendJson(const QJsonObject &jsonData);
    void sendImage(const QImage& image, const QString& source);
    void sendPoint(const QPointF& point, qint32 operationCode, qint32 senderID);
    void sendColor(const QColor& color);
    void sendFile(DataTypes dataType, const QByteArray &data);
    void setMeeting(Meeting* meeting);
    Meeting* getMeeting();
    void setMode(qint32 mode);
    qint32 getMode();
signals:
    //Используется для отправки на центральный сервер полученного сообщения
    void jsonReceived(const QJsonObject &jsonDoc);
    void imageReceived(const QImage &image, const QString& source);
    void pointReceived(const QPointF& point, qint32 operationCode);
    void colorReceived(const QColor& color);
    void fileReceived(DataTypes dataType, const QByteArray &data);
    //Выдается, когда клиент закрывает соединение
    void disconnectedFromClient();
    //Используется для уведомления об ошибке
    void error();
    //Используется для уведомления сервера о внутренних событиях
    void logMessage(const QString &msg);
public slots:
    //Метод, закрывающий соединение с клиентом
    void disconnectFromClient();
private slots:

    void onReadyRead();
private:
    //Содержит сокет, взаимодействующий с клиентом
    QTcpSocket *m_serverSocket;
    //Имя пользователя подключенного к этому объекту
    QString m_userName;
    Meeting* meeting;
    qint32 mode;
};

#endif // SERVERWORKER_H
