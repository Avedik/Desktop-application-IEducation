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
    /// ��������� ������������� ������������� ����� 
    /// � ���������� ������������ ��� ServerWindow
    /// </summary>
    Q_DISABLE_COPY(ServerWindow)

    UI::ServerWindow *ui;
    Server *m_Server;

public:
    explicit ServerWindow(QWidget* parent = nullptr);
    ~ServerWindow();

private slots:
    /// <summary>
    /// ������������ ��������� � �������
    /// </summary>
    /// <param name="msg">���������</param>
    void logMessage(const QString& msg);

    void toggleStartServer();
};

#endif // SERVERWINDOW_H
