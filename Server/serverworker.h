#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include <QObject>
#include <QTcpSocket>
class QJsonObject;
class ServerWorker : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ServerWorker)
public:
    explicit ServerWorker(QObject *parent = nullptr);
    virtual bool setSocketDescriptor(qintptr socketDescriptor);
    QString userName() const;
    //Способ получения и установки имени пользователя
    void setUserName(const QString &userName);
    void sendJson(const QJsonObject &jsonData);
signals:
    //Используется для отправки на центральный сервер полученного сообщения
    void jsonReceived(const QJsonObject &jsonDoc);
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
    //Метод по чтению входящих сообщений из сети и отправке на главный серверный объект
    void receiveJson();
private:
    //Содержит сокет, взаимодействующий с клиентом
    QTcpSocket *m_serverSocket;
    //Имя пользователя подключенного к этому объекту
    QString m_userName;
};

#endif // SERVERWORKER_H
