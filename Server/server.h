#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QVector>

//Класс для управления потоками
class QThread;
class ServerWorker;
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
    //Вектор для хранения пользователей
    QVector<ServerWorker *> m_clients;
    bool _state = true;
    qint32 numberOfUsersWithFile = 0;

private slots:
    ///Проверяет статус подключенных пользователей и логирует информацию об этом
    void broadcast(const QJsonObject &message, ServerWorker *exclude);
    void jsonReceived(ServerWorker *sender, const QJsonObject &doc);
    void imageReceived(ServerWorker *sender, const QImage &img, const QString& source);
    void pdfReceived(ServerWorker *sender, const QByteArray &data);
    ///Функция для обработки выхода пользователя
    void userDisconnected(ServerWorker *sender);
    ///Функция для обработки ошибки пользователя
    void userError(ServerWorker *sender);
    void userReceiveFile();

public:
    ///Конструктор Server
    explicit Server(QObject *parent = nullptr);

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
