#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QVector>
#include <QQueue>
#include <QList>
#include <QPointF>
#include <QColor>
#include "serverworker.h"

//Класс для управления потоками
class QThread;
class ServerWorker;
class Meeting;
class Server : public QTcpServer
{
private:
    Q_OBJECT  //Выполняет некоторые функции в QT. Если убрать, сервер может работать некорректно
    Q_DISABLE_COPY(Server) //Запрещает использовать копирующие конструкторы и операторы присвоения

    ///Формирование json запроса о человеке, который вышел
    void jsonFromLoggedOut(ServerWorker *sender, const QJsonObject &doc);
    ///Формирование json запроса о человеке, который зашел
    void jsonFromLoggedIn(ServerWorker *sender, const QJsonObject &doc);
    ///Отправка json запроса
    void sendJson(ServerWorker *destination, const QJsonObject &message);
    void allocateUser(ServerWorker *sender, const QString& ID);
    //Вектор для хранения пользователей
    QVector<QVector<Meeting*>*> m_modes;
    QQueue<ServerWorker *> waitingUsers;

private slots:
    ///Проверяет статус подключенных пользователей и логирует информацию об этом
    void broadcast(const QJsonObject &message, ServerWorker *exclude);
    void jsonReceived(ServerWorker *sender, const QJsonObject &doc);
    void imageReceived(ServerWorker *sender, const QImage &img, const QString& source);
    void pointReceived(ServerWorker *sender, const QPointF &point, qint32 operationCode);
    void colorReceived(ServerWorker *sender, const QColor &color);
    void fileReceived(ServerWorker *sender, DataTypes dataType, const QByteArray &data);
    void ratingReceived(ServerWorker *sender, const QJsonObject &rating);

    ///Функция для обработки выхода пользователя
    void userDisconnected(ServerWorker *sender);
    ///Функция для обработки ошибки пользователя
    void userError(ServerWorker *sender);

public:
    ///Конструктор Server
    explicit Server(QObject *parent = nullptr);
    ~Server();

public slots:
    ///Функция для остановки сервера
    void stopServer();

protected:
    ///Функция для обработки нового пользователя
    void incomingConnection(qintptr socketDescriptor) override;

signals:
    ///Функция для логирования сообщений
    void logMessage(const QString &msg);


};

#endif
