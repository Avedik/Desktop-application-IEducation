#ifndef BRAINSTORM_H
#define BRAINSTORM_H

#include <QDialog>
#include <QGraphicsView>
#include <QPointF>
#include <QAbstractSocket>
#include <painter.h>

namespace Ui {
class brainstorm;
}

class Controller;
class brainstorm : public QDialog
{
    Q_OBJECT

public:
    enum class Instruments { Stylet, Eraser };
    explicit brainstorm(QWidget *parent = nullptr);
    ~brainstorm();
    Instruments getCurrentInstrument();
    void sendPoint(const QPointF& point, qint32 operationCode);

private slots:
    void on_styletButton_clicked();

    void on_eraserButton_clicked();

    void on_clearButton_clicked();
    void receivePoint(const QPointF& point, qint32 operationCode);
    void attemptConnection();
    void connectedToServer();
    void attemptLogin(const QString &userName);
    void loggedIn(const QString& userName);
    void loginFailed(const QString &reason);
    void disconnectedFromServer();
    void userJoined(const QString &username, const QString &meetingID);
    void error(QAbstractSocket::SocketError socketError);

private:
    void switchEnabled(bool is_enabled);

    bool is_connected;
    Instruments currentInstrument;
    Ui::brainstorm *ui;
    Painter *scene;
    Controller *m_Client;
    QString *meetingID = nullptr;
};

#endif // BRAINSTORM_H
