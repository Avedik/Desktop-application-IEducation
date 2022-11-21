#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QWidget>

namespace UI {
  class ServerWindow;
}

class Server;

class ServerWindow : public QWidget
{
    Q_OBJECT

    /// <summary>
    /// Отключает использование конструкторов копии 
    /// и операторов присваивания для ServerWindow
    /// </summary>
    Q_DISABLE_COPY(ServerWindow)

    UI::ServerWindow *ui;
    Server *m_Server;

public:
    explicit ServerWindow(QWidget* parent = nullptr);
    ~ServerWindow();

private slots:
    /// <summary>
    /// Регистрирует сообщение в журнале
    /// </summary>
    /// <param name="msg">сообщение</param>
    void logMessage(const QString& msg);

    void toggleStartServer();
};

#endif // SERVERWINDOW_H
