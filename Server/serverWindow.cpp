#include "server.h"
#include "serverWindow.h"
#include "ui_serverwindow.h"
#include <QMessageBox>

ServerWindow::ServerWindow(QWidget* parent) : QWidget(parent)
, ui(new Ui::ServerWindow)
, m_Server(new Server(this))
{
    ui->setupUi(this);

    connect(m_Server, &Server::logMessage, this, &ServerWindow::logMessage);
    connect(ui->startStopButton, &QPushButton::clicked, this, &ServerWindow::StartServer);
}

ServerWindow::~ServerWindow()
{
    delete ui;
}

void ServerWindow::StartServer()
{
    if (!m_Server->isListening()) {
        bool fatal = false;
        if (!m_Server->listen(QHostAddress::Any, 1967)) {
            QMessageBox::critical(this, tr("������"), tr("���������� ��������� ������"));
            fatal = true;
        }

        if (!fatal) {
            logMessage(QStringLiteral("������ �������"));
            ui->startStopButton->setText(tr("���������� ������"));
        }
    }
    else {
        m_Server->stopServer();
        ui->startStopButton->setText(tr("��������� ������"));
        logMessage(QStringLiteral("������ ����������"));
    }
}

void ServerWindow::logMessage(const QString& message)
{
    ui->logEditor->appendPlainText(message + QLatin1Char('\n'));
}
