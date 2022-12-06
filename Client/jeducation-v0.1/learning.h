#ifndef LEARNING_H
#define LEARNING_H

#include <QDialog>
#include <QTime>
#include <QWidget>
#include <QAbstractSocket>

namespace Ui {
class learning;
}

class Controller;
class QStandardItemModel;

class learning : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(learning)

public:
    explicit learning(QWidget *parent = nullptr);
    ~learning();

private:
    Ui::learning *ui;
    QTimer *timer;
    QTimer *cnt_timer;
    QTime time;
    Controller *m_Client;
    QStandardItemModel *m_Model;
    QString m_lastUserName;
    bool is_connected;

private slots:
    void on_pushButton_clicked();
    void countTimer();
    void paintingTimer();
    void paintEvent(QPaintEvent *);
    void attemptConnection();
    void connectedToServer();
    void attemptLogin(const QString &userName);
    void loggedIn();
    void loginFailed(const QString &reason);
    void messageReceived(const QString &sender, const QString &text);
    void sendMessage();
    void disconnectedFromServer();
    void userJoined(const QString &username);
    void userLeft(const QString &username);
    void error(QAbstractSocket::SocketError socketError);
};

#endif // LEARNING_H
