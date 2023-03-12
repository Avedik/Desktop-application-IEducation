#ifndef LEARNING_H
#define LEARNING_H

#include <QDialog>
#include <QTime>
#include <QWidget>
#include <QAbstractSocket>
#include <QTimer>
#include "dialog.h"
#include "ask.h"
#include "other_questions.h"

namespace Ui {
class learning;
}

class Controller;
class QStandardItemModel;

class learning : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(learning)
    friend ask;
    friend Dialog;

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
    Dialog *answer;
    ask *question;
    other_questions *other_quest;


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
    void questionReceived(const QString &sender, const QString &text);
    void answerReceived(const QString &from, const QString &to, const QString &ques, const QString &ans);
    void refreshUsersList(const QVariantMap& users);
    void sendMessage();
    void disconnectedFromServer();
    void userJoined(const QString &username);
    void userLeft(const QString &username);
    void error(QAbstractSocket::SocketError socketError);
    void on_chooseButton_clicked();
    void on_askButton_clicked();
    void on_answerButton_clicked();
    void on_pushButton_4_clicked();
};

#endif // LEARNING_H
