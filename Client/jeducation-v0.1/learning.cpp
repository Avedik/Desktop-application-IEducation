#include "learning.h"
#include "ui_learning.h"
#include "ui_dialog.h"
#include <QtWidgets>
#include "Controller/controller.h"
#include <QStandardItemModel>
#include <QInputDialog>
#include <QMessageBox>
#include <QHostAddress>

int tic = 0;
int cnt = 0;
// от сглаза
learning::learning(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::learning)
  , m_Client(new Controller(this))
  , m_Model(new QStandardItemModel(this))
{
    answer = new Dialog(this);
    other_quest = new other_questions(this);

    ui->setupUi(this);
    ui->sendButton->setEnabled(false);
    ui->messageEdit->setEnabled(false);
    ui->chatView->setEnabled(false);
    setStyleSheet("QPushButton { background-color: rgb(100,100,100); }");
    setStyleSheet(styleSheet() + "QLabel, QLineEdit, QPushButton { color: white; }");

    m_Model->insertColumn(0);
    is_connected = false;
    ui->chatView->setModel(m_Model);

    connect(m_Client, &Controller::connected, this, &learning::connectedToServer);
    connect(m_Client, &Controller::loggedIn, this, &learning::loggedIn);
    connect(m_Client, &Controller::loginError, this, &learning::loginFailed);
    connect(m_Client, &Controller::messageReceived, this, &learning::messageReceived);
    connect(m_Client, &Controller::questionReceived, this, &learning::questionReceived);
    connect(m_Client, &Controller::disconnected, this, &learning::disconnectedFromServer);
    connect(m_Client, &Controller::error, this, &learning::error);
    connect(m_Client, &Controller::userJoined, this, &learning::userJoined);
    connect(m_Client, &Controller::userLeft, this, &learning::userLeft);
    connect(ui->connectButton, &QPushButton::clicked, this, &learning::attemptConnection);
    connect(ui->sendButton, &QPushButton::clicked, this, &learning::sendMessage);
    connect(ui->messageEdit, &QLineEdit::returnPressed, this, &learning::sendMessage);

    time.setHMS(0,10,0);
    cnt_timer= new QTimer(this);
    cnt_timer->setInterval(1000);
    connect(cnt_timer,SIGNAL(timeout()),this,SLOT(countTimer()));

    timer= new QTimer(this);
    timer->setInterval(160);
    connect(timer,SIGNAL(timeout()),this,SLOT(paintingTimer()));

}

learning::~learning()
{
    delete ui;
}

void learning::on_pushButton_clicked()
{
    if (!cnt_timer->isActive())
    {
        cnt_timer->start();
        timer->start();
    }
}

void learning::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
}

void learning::countTimer()
{
    tic++;
    ui->label_6->setText(time.addSecs(-tic).toString("mm:ss"));
    if (ui->label_6->text() == "00:00")
    {
        timer->stop();
        cnt_timer->stop();
    }
}

void learning::paintingTimer()
{
    cnt++;
    repaint();
}

void learning::attemptConnection()
{
    if (is_connected)
    {
      is_connected = false;
      return m_Client->disconnectFromHost();
    }

    const QString hostAddress = QInputDialog::getText(
        this
        , tr("Выбор сервера")
        , tr("Адрес сервера")
        , QLineEdit::Normal
        , QStringLiteral("46.29.115.42")
    );
    if (hostAddress.isEmpty())
      return;

    m_Client->connectToServer(QHostAddress(hostAddress), 1967);
}

void learning::connectedToServer()
{
    const QString newUsername = QInputDialog::getText(this, tr("Выбор пользователя"), tr("Имя пользователя"));
    if (newUsername.isEmpty()){
        is_connected = false;
        return m_Client->disconnectFromHost();
    }
    
    is_connected = true;
    ui->connectButton->setText(tr("Отключиться"));
    setStyleSheet(styleSheet() + "QPushButton { background-color: rgb(20,20,20); }");
    ui->label_3->setText(newUsername);
    attemptLogin(newUsername);
}

void learning::attemptLogin(const QString &userName)
{
    m_Client->login(userName);
}

void learning::loggedIn()
{
    ui->sendButton->setEnabled(true);
    ui->messageEdit->setEnabled(true);
    ui->chatView->setEnabled(true);

    m_lastUserName.clear();
}

void learning::loginFailed(const QString &reason)
{
    QMessageBox::critical(this, tr("Ошибка"), reason);
    connectedToServer();
}

void learning::messageReceived(const QString &sender, const QString &text)
{
    int newRow = m_Model->rowCount();

    if (m_lastUserName != sender) {

        m_lastUserName = sender;

        QFont boldFont;
        boldFont.setBold(true);

        m_Model->insertRows(newRow, 2);

        m_Model->setData(m_Model->index(newRow, 0), sender + QLatin1Char(':'));

        m_Model->setData(m_Model->index(newRow, 0), int(Qt::AlignLeft | Qt::AlignVCenter), Qt::TextAlignmentRole);

        m_Model->setData(m_Model->index(newRow, 0), boldFont, Qt::FontRole);
        ++newRow;
    } else {

        m_Model->insertRow(newRow);
    }
    m_Model->setData(m_Model->index(newRow, 0), text);
    m_Model->setData(m_Model->index(newRow, 0), int(Qt::AlignLeft | Qt::AlignVCenter), Qt::TextAlignmentRole);
    ui->chatView->scrollToBottom();
}

void learning::questionReceived(const QString &sender, const QString &text)
{
   auto table = answer->ui->questionTable;
   table->insertRow(0);
   table->setItem(0, 0, new QTableWidgetItem(sender));
   table->setItem(0, 1, new QTableWidgetItem(text));
}

void learning::sendMessage()
{
    m_Client->sendMessage(ui->messageEdit->text());

    const int newRow = m_Model->rowCount();

    m_Model->insertRow(newRow);
    m_Model->setData(m_Model->index(newRow, 0), ui->messageEdit->text());

    m_Model->setData(m_Model->index(newRow, 0), int(Qt::AlignRight | Qt::AlignVCenter), Qt::TextAlignmentRole);
    ui->messageEdit->clear();
    ui->chatView->scrollToBottom();
    m_lastUserName.clear();
}

void learning::disconnectedFromServer()
{
    QMessageBox::warning(this, tr("Отсоединено"), tr("Хост прервал соединение"));

    ui->connectButton->setText(tr("Подключиться"));
    setStyleSheet(styleSheet() + "QPushButton { background-color: rgb(100,100,100); }");
    ui->sendButton->setEnabled(false);
    ui->messageEdit->setEnabled(false);
    ui->chatView->setEnabled(false);
    ui->connectButton->setEnabled(true);
    m_lastUserName.clear();
    hide();
}

void learning::userJoined(const QString &username)
{
    const int newRow = m_Model->rowCount();
    m_Model->insertRow(newRow);

    m_Model->setData(m_Model->index(newRow, 0), tr("%1 Присоединился к чату").arg(username));
    m_Model->setData(m_Model->index(newRow, 0), Qt::AlignCenter, Qt::TextAlignmentRole);
    m_Model->setData(m_Model->index(newRow, 0), QBrush(Qt::blue), Qt::ForegroundRole);
    ui->chatView->scrollToBottom();
    m_lastUserName.clear();
}
void learning::userLeft(const QString &username)
{

    const int newRow = m_Model->rowCount();

    m_Model->insertRow(newRow);
    m_Model->setData(m_Model->index(newRow, 0), tr("%1 Покинул чат").arg(username));
    m_Model->setData(m_Model->index(newRow, 0), Qt::AlignCenter, Qt::TextAlignmentRole);

    m_Model->setData(m_Model->index(newRow, 0), QBrush(Qt::red), Qt::ForegroundRole);
    ui->chatView->scrollToBottom();
    m_lastUserName.clear();
}

void learning::error(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
    case QAbstractSocket::ProxyConnectionClosedError:
        return;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::critical(this, tr("Ошибка"), tr("Хост отказался от подключения"));
        break;
    case QAbstractSocket::ProxyConnectionRefusedError:
        QMessageBox::critical(this, tr("Ошибка"), tr("Прокси-сервер отказал в подключении"));
        break;
    case QAbstractSocket::ProxyNotFoundError:
        QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось найти прокси-сервер"));
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось найти сервер"));
        break;
    case QAbstractSocket::SocketAccessError:
        QMessageBox::critical(this, tr("Ошибка"), tr("У вас нет разрешений на выполнение этой операции"));
        break;
    case QAbstractSocket::SocketResourceError:
        QMessageBox::critical(this, tr("Ошибка"), tr("Открыто слишком много соединений"));
        break;
    case QAbstractSocket::SocketTimeoutError:
        QMessageBox::warning(this, tr("Ошибка"), tr("Время ожидания операции истекло"));
        return;
    case QAbstractSocket::ProxyConnectionTimeoutError:
        QMessageBox::critical(this, tr("Ошибка"), tr("Время ожидания прокси истекло"));
        break;
    case QAbstractSocket::NetworkError:
        QMessageBox::critical(this, tr("Ошибка"), tr("Не удается подключиться к сети"));
        break;
    case QAbstractSocket::UnknownSocketError:
        QMessageBox::critical(this, tr("Ошибка"), tr("Произошла неизвестная ошибка"));
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        QMessageBox::critical(this, tr("Ошибка"), tr("Операция не поддерживается"));
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        QMessageBox::critical(this, tr("Ошибка"), tr("Вашему прокси-серверу требуется аутентификация"));
        break;
    case QAbstractSocket::ProxyProtocolError:
        QMessageBox::critical(this, tr("Ошибка"), tr("Сбой связи через прокси-сервер"));
        break;
    case QAbstractSocket::TemporaryError:
    case QAbstractSocket::OperationError:
        QMessageBox::warning(this, tr("Ошибка"), tr("Операция не совершена, повторите попытку"));
        return;
    default:
        Q_UNREACHABLE();
    }

    ui->connectButton->setEnabled(true);

    ui->sendButton->setEnabled(false);
    ui->messageEdit->setEnabled(false);
    ui->chatView->setEnabled(false);

    m_lastUserName.clear();
}

void learning::on_chooseButton_clicked()
{
    QString url = QFileDialog::getOpenFileName(this, "Выбор фотографии", "../", "*.png *.jpg");

    QPixmap img(url);
    QSize sz(150,150);
    img = img.scaled(sz, Qt::KeepAspectRatio);
    ui->label_2->setPixmap(img);
}

void learning::on_askButton_clicked()
{
    delete question;
    question = new ask(this);
    question->show();
}

void learning::on_answerButton_clicked()
{
    answer->show();
}

void learning::on_pushButton_4_clicked()
{
    other_quest->show();
}

