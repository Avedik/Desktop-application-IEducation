#include "server.h"
#include "serverWindow.h"
#include "ui_window.h"
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
            QMessageBox::critical(this, tr("Ошибка"), tr("Невозможно запустить сервер"));
            fatal = true;
        }

        if (!fatal) {
            logMessage(QStringLiteral("Сервер запущен"));
            ui->startStopButton->setText(tr("Остановить сервер"));
        }
    }
    else {
        m_Server->stopServer();
        ui->startStopButton->setText(tr("Запустить сервер"));
        logMessage(QStringLiteral("Сервер остановлен"));
    }
}

void ServerWindow::logMessage(const QString& message)
{
    ui->logEditor->appendPlainText(message + QLatin1Char('\n'));
}
