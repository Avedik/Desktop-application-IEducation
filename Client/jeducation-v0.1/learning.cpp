#include "learning.h"
#include "ui_learning.h"
#include "ui_dialog.h"
#include "ui_ask.h"
#include "ui_other_questions.h"
#include <QtWidgets>
#include "Controller/controller.h"
#include <QStandardItemModel>
#include <QInputDialog>
#include <QMessageBox>
#include <QHostAddress>

learning::learning(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::learning)
  , m_Client(new Controller(this))
  , m_Model(new QStandardItemModel(this))
{
    answer = new Dialog(this);
    other_quest = new other_questions(this);
    question = new ask(this);

    ui->setupUi(this);
    setStyleSheet("QPushButton { background-color: rgb(100,100,100); }");
    setStyleSheet(styleSheet() + "QLabel, QLineEdit, QPushButton { color: white; }");

    m_Model->insertColumn(0);
    is_connected = false;
    ui->chatView->setModel(m_Model);

    // Задаём обработчик нажатия на Enter
    keyEnterReceiver* key = new keyEnterReceiver(this);
    installEventFilter(key);

    connect(m_Client, &Controller::connected, this, &learning::connectedToServer);
    connect(m_Client, &Controller::loggedIn, this, &learning::loggedIn);
    connect(m_Client, &Controller::loginError, this, &learning::loginFailed);
    connect(m_Client, &Controller::messageReceived, this, &learning::messageReceived);
    connect(m_Client, &Controller::questionReceived, this, &learning::questionReceived);
    connect(m_Client, &Controller::answerReceived, this, &learning::answerReceived);
    connect(m_Client, &Controller::refreshUsersList, this, &learning::refreshUsersList);
    connect(m_Client, &Controller::disconnected, this, &learning::disconnectedFromServer);
    connect(m_Client, &Controller::error, this, &learning::error);
    connect(m_Client, &Controller::userJoined, this, &learning::userJoined);
    connect(m_Client, &Controller::userLeft, this, &learning::userLeft);
    connect(m_Client, &Controller::receiveImage, this, &learning::receiveImage);

    connect(ui->connectButton, &QPushButton::clicked, this, &learning::attemptConnection);
    connect(ui->sendButton, &QPushButton::clicked, this, &learning::sendMessage);
    connect(ui->messageEdit, &QLineEdit::returnPressed, this, &learning::sendMessage);

    time.setHMS(0,0,10);
    cnt_timer= new QTimer(this);
    cnt_timer->setInterval(1000);
    connect(cnt_timer,SIGNAL(timeout()),this,SLOT(countTimer()));

    timer= new QTimer(this);
    timer->setInterval(160);
    connect(timer,SIGNAL(timeout()),this,SLOT(paintingTimer()));
    ui->chatView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    setAttribute(Qt::WA_DeleteOnClose);
}

bool learning::keyEnterReceiver::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type()==QEvent::KeyPress) {
            QKeyEvent* key = static_cast<QKeyEvent*>(event);
            if ( (key->key()==Qt::Key_Enter) || (key->key()==Qt::Key_Return) ) {
                if (dest->time_is_changed)
                    dest->time_is_changed = false;
                else if (dest->is_connected)
                    dest->sendMessage();
                else
                    dest->attemptConnection();
            } else {
                return QObject::eventFilter(obj, event);
            }
            return true;
    } else {
        return QObject::eventFilter(obj, event);
    }
    return false;
}

learning::~learning()
{
    if (engine)
    {
        QMetaObject::invokeMethod(engine->rootObjects().first(), "close");
        delete engine;
    }
    if (is_connected)
        m_Client->disconnectFromHost();
    delete ui;
}


void learning::on_timerEdit_textChanged(const QString &arg1)
{
    time_is_changed = true;
}

void learning::on_timerEdit_editingFinished()
{
    QStringList timeElems = ui->timerEdit->text().split(":");
    if (timeElems.size() != 2)
    {
        QMessageBox::critical(this,"Ошибка","Введите корректное время в формате **:**");
        return;
    }

    bool is_min = true;
    bool is_sec = true;
    int mins = timeElems.first().toInt(&is_min);
    int seconds = timeElems.last().toInt(&is_sec);

    if (!is_min || mins >= 100)
    {
        QMessageBox::critical(this,"Ошибка","Введите корректное число минут");
        return;
    }
    if (!is_sec || seconds >= 60)
    {
        QMessageBox::critical(this,"Ошибка","Введите корректное число секунд");
        return;
    }
    if ((mins == 0 && seconds == 0) || mins < 0 || seconds < 0)
    {
        QMessageBox::critical(this,"Ошибка","Задайте корректное время");
        return;
    }
    time.setHMS(mins/60, mins == 0 ? 0 : mins%60, seconds);
    ui->pushButton->setEnabled(true);
    m_Client->sendMessage(QString("timerValue") + ui->timerEdit->text());
}

void learning::on_pushButton_clicked()
{
    if (ui->importPdfButton->isEnabled())
    {
        QMessageBox::warning(this,"Предупреждение","Загрузите материал для изучения");
        return;
    }

    if (!cnt_timer->isActive())
    {
        cnt_timer->start();
        timer->start();
        ui->pushButton->setEnabled(false);
    }
    m_Client->sendMessage(QString("startTimer"));
    ui->timerEdit->setReadOnly(true);
}

void learning::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
}

void learning::countTimer()
{
    tic++;
    ui->timerEdit->setText(time.addSecs(-tic).toString("mm:ss"));
    if (ui->timerEdit->text() == "00:00")
    {
        timer->stop();
        cnt_timer->stop();
        tic = cnt = 0;
        ui->timerEdit->setReadOnly(false);

        if (engine)
        {
            on_askButton_clicked();
            ui->importPdfButton->setEnabled(false);

            QMetaObject::invokeMethod(engine->rootObjects().first(), "close");
            delete engine;
            engine = nullptr;

            QMessageBox::information(this, "Сообщение",
                                     "Задайте время в таймере для составления вопросов и запустите его.");
        }
        else if (ui->askButton->isEnabled())
        {
            question->close();
            on_answerButton_clicked();
            ui->askButton->setEnabled(false);
            ui->answerButton->setEnabled(true);

            QMessageBox::information(this, "Сообщение",
                                     "Задайте время в таймере для ответов на вопросы и запустите его.");
        }
        else if (ui->answerButton->isEnabled())
        {
            answer->close();
            on_allAnswersButton_clicked();
            ui->answerButton->setEnabled(false);
            ui->allAnswersButton->setEnabled(true);

            QMessageBox::information(this, "Сообщение",
                                     "Задайте время в таймере для оценки ответов других участников и запустите его.");
        }
        else if (ui->allAnswersButton->isEnabled())
        {
            other_quest->close();
            on_allAnswersButton_clicked();
            ui->allAnswersButton->setEnabled(false);
            QMessageBox::information(this, "Сообщение", "Вы прошли все этапы!");
        }
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

    switchEnabled(true);
    attemptLogin(newUsername);
}

void learning::switchEnabled(bool is_enabled)
{
    ui->sendButton->setEnabled(is_enabled);
    ui->messageEdit->setEnabled(is_enabled);
    ui->chatView->setEnabled(is_enabled);
    ui->chooseButton->setEnabled(is_enabled);

    ui->pushButton->setEnabled(is_enabled);
    ui->timerEdit->setEnabled(is_enabled);
    ui->askButton->setEnabled(is_enabled);
    ui->importPdfButton->setEnabled(is_enabled);
}

void learning::attemptLogin(const QString &userName)
{
    m_Client->login(userName);
}

void learning::loggedIn()
{
    m_lastUserName.clear();
}

void learning::loginFailed(const QString &reason)
{
    QMessageBox::critical(this, tr("Ошибка"), reason);
    connectedToServer();
}

void learning::messageReceived(const QString &sender, const QString &text)
{
    if (text == QString("startTimer"))
    {
        on_pushButton_clicked();
        return;
    }
    else if (text.startsWith(QString("timerValue")))
    {
        ui->timerEdit->setText(text.mid(10, 5));
        on_timerEdit_editingFinished();
        return;
    }

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

   int count = 0;
   for (auto s : {sender, text})
   {
       QTableWidgetItem *item = new QTableWidgetItem(s);
       item->setFlags(item->flags() | Qt::ItemIsEditable);
       table->setItem(0, count++, item);
   }
}

void learning::answerReceived(const QString &from, const QString &to, const QString &ques, const QString &ans)
{
    auto table = other_quest->ui->answerTable;
    table->insertRow(0);

    int count = 0;
    for (auto s : {from, to, ques, ans})
    {
        QTableWidgetItem *item = new QTableWidgetItem(s);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        table->setItem(0, count++, item);
    }
}

void learning::refreshUsersList(const QVariantMap& users)
{
    auto table = question->ui->usersTable;
    table->clearContents();
    table->setRowCount(0);

    for (QVariantMap::const_iterator iter = users.begin(); iter != users.end(); ++iter)
    {
        if (iter.key().compare(QStringLiteral("тип")) == 0)
            continue;
        table->insertRow(0);

        QTableWidgetItem *item = new QTableWidgetItem(iter.key());
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        table->setItem(0, 1, item);
    }
    m_Client->sendImage(QImage(picturePath));
}

void learning::sendMessage()
{
    if (ui->messageEdit->text().trimmed().isEmpty())
        return;
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

    switchEnabled(false);
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

    switchEnabled(false);
    m_lastUserName.clear();
}

void learning::on_chooseButton_clicked()
{
    picturePath = QFileDialog::getOpenFileName(this, "Выбор фотографии", "../", "*.png *.jpg");

    QPixmap img(picturePath);
    QSize sz(300,300);
    img = img.scaled(sz, Qt::KeepAspectRatio);
    ui->label_2->setPixmap(img);

    m_Client->sendImage(QImage(picturePath));
}

void learning::on_askButton_clicked()
{
    question->ui->textEdit->clear();
    question->show();
}

void learning::on_answerButton_clicked()
{
    answer->show();
}

void learning::on_allAnswersButton_clicked()
{
    other_quest->show();
}

void learning::receiveImage(const QImage& image, const QString& source)
{
    QPixmap img = QPixmap::fromImage(image);
    QSize sz(50,50);
    img = img.scaled(sz, Qt::KeepAspectRatio);

    QLabel *_label = new QLabel();
    _label->setPixmap(img);

    auto table = question->ui->usersTable;
    QTableWidgetItem *item = table->findItems(source, Qt::MatchContains).last();

    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    table->setCellWidget(item->row(), 0, _label);
}

void learning::on_importPdfButton_clicked()
{
    QString docPath = QFileDialog::getOpenFileName(this, "Импортирование материала", "../", "*.pdf");
    if (docPath.isEmpty())
        return;
    ui->importPdfButton->setEnabled(false);

    if (engine)
    {
        QMetaObject::invokeMethod(engine->rootObjects().first(), "close");
        delete engine;
        engine = nullptr;
    }
    engine = new QQmlApplicationEngine();
    engine->load(QUrl(QStringLiteral("qrc:///pdfviewer/viewer.qml")));
    engine->rootObjects().constFirst()->setProperty("source", QUrl::fromUserInput(docPath));

    QMessageBox::information(this, "Сообщение",
                             "Настройте и запустите таймер. Начните изучать материал!");
}

