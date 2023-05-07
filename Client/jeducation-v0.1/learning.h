#ifndef LEARNING_H
#define LEARNING_H

#include <QDialog>
#include <QTime>
#include <QWidget>
#include <QAbstractSocket>
#include <QTimer>
#include <QQmlApplicationEngine>
#include "dialog.h"
#include "ask.h"
#include "other_questions.h"
#include "dataTypes.h"

namespace Ui {
class learning;
}

class Controller;
class QStandardItemModel;
class rating;

class learning : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(learning)
    friend ask;
    friend other_questions;
    friend Dialog;

public:
    explicit learning(rating *table, QWidget *parent = nullptr);
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
    QString picturePath;
    QQmlApplicationEngine *engine = nullptr;
    QString *meetingID = nullptr;
    rating *ratingTable;

    int tic = 0;
    int cnt = 0;
private:
    void startTimer();
    void switchEnabled(bool is_enabled);
    void switchButtonEnabled(QPushButton* button, bool is_enabled);
private slots:
    void on_pushButton_clicked();
    void countTimer();
    void paintingTimer();
    void paintEvent(QPaintEvent *);
    void attemptConnection();
    void connectedToServer();
    void attemptLogin(const QString &userName);
    void loggedIn(const QString& userName);
    void loginFailed(const QString &reason);
    void messageReceived(const QString &sender, const QString &text);
    void scoreReceived(const QString &destUser, qint32 score);
    void questionReceived(const QString &sender, const QString &text);
    void answerReceived(const QString &from, const QString &to, const QString &ques, const QString &ans);

    void refreshUsersList(const QVariantMap& users, const QString& type);
    void sendMessage();
    void disconnectedFromServer();
    void userJoined(const QString &username, const QString &meetingID);
    void userLeft(const QString &username);
    void error(QAbstractSocket::SocketError socketError);
    void on_chooseButton_clicked();
    void on_askButton_clicked();
    void on_answerButton_clicked();
    void on_allAnswersButton_clicked();
    void receiveImage(const QImage& image, const QString& source);
    void receiveFile(DataTypes dataType, const QByteArray &data);
    void on_importPdfButton_clicked();

    bool timerEditFinished();
    void openPDFViewer(const QString& docPath);

    void on_deletePhoto_clicked();

    void on_newMeetingButton_clicked();

    void on_connectButton_clicked();

private:
    class keyEnterReceiver : public QObject
    {
        learning* dest;
    public:
        keyEnterReceiver(learning* _dest): dest(_dest) {}
    protected:
        bool eventFilter(QObject* obj, QEvent* event);
    };
};

#endif // LEARNING_H
