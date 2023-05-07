#include <QInputDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QString>
#include <QMessageBox>
#include <QMediaFormat>
#include <QColorDialog>
#include <QFileDialog>
#include <QAudioDevice>
#include <QMediaDevices>
#include <QDesktopServices>
#include "brainstorm.h"
#include "ui_brainstorm.h"
#include "Controller/controller.h"

brainstorm::brainstorm(QWidget *parent) :
    QDialog(parent),
    currentInstrument(Instruments::Stylet),
    ui(new Ui::brainstorm),
    m_Client(new Controller(this)),
    audioFilesCount(0)
{
    ui->setupUi(this);
    is_connected = false;
    switchEnabled(false);

    ui->audioFilesBox->setColumnCount(1);
    ui->audioFilesBox->setShowGrid(true);

    ui->audioFilesBox->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->audioFilesBox->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->audioFilesBox->setHorizontalHeaderLabels(QStringList() << "Принятые аудио");
    ui->audioFilesBox->horizontalHeader()->setStretchLastSection(true);

    connect(m_Client, &Controller::receivePoint, this, &brainstorm::receivePoint);
    connect(m_Client, &Controller::receiveColor, this, &brainstorm::receiveColor);
    connect(m_Client, &Controller::receiveFile, this, &brainstorm::receiveFile);
    connect(m_Client, &Controller::connected, this, &brainstorm::connectedToServer);
    connect(m_Client, &Controller::loggedIn, this, &brainstorm::loggedIn);
    connect(m_Client, &Controller::loginError, this, &brainstorm::loginFailed);
    connect(m_Client, &Controller::disconnected, this, &brainstorm::disconnectedFromServer);
    connect(m_Client, &Controller::error, this, &brainstorm::error);
    connect(m_Client, &Controller::userJoined, this, &brainstorm::userJoined);
    connect(ui->joinButton, &QPushButton::clicked, this, &brainstorm::attemptConnection);
    connect(m_Client, &Controller::refreshUsersList, this, &brainstorm::refreshUsersQuantity);

    scene = new Painter(this);
    ui->table->setScene(scene);
    ui->table->setBackgroundBrush(QColor("white"));
    setAttribute(Qt::WA_DeleteOnClose);

    audioSession = new QMediaCaptureSession();
    audioInput = new QAudioInput(this);
    audioSession->setAudioInput(audioInput);

    recorder = new QMediaRecorder(this);
    audioSession->setRecorder(recorder);
    recorder->setQuality(QMediaRecorder::HighQuality);

    QMediaFormat format;
    format.setAudioCodec(QMediaFormat::AudioCodec::MP3);
    recorder->setMediaFormat(format);

    QString diskName = QApplication::applicationDirPath().split(QRegularExpression(":")).first();
    temporaryDir = new QTemporaryDir(diskName + ":/");
    recorder->setOutputLocation(QUrl::fromLocalFile(temporaryDir->path() + "/audio"));

    for (auto input : QMediaDevices::audioInputs()) {
        auto name = input.description();
        ui->audioInputBox->addItem(name, QVariant::fromValue(input));
    }

    player = new QMediaPlayer;
    audioOutput = new QAudioOutput;
    player->setAudioOutput(audioOutput);
}

brainstorm::~brainstorm()
{
    delete audioSession;
    delete audioInput;
    delete recorder;

    player->setSource(QUrl());
    delete player;
    delete audioOutput;
    delete temporaryDir;

    if (is_connected)
        m_Client->disconnectFromHost();
    delete ui;
    delete scene;
    if (meetingID)
        delete meetingID;
}

brainstorm::Instruments brainstorm::getCurrentInstrument()
{
    return currentInstrument;
}

void brainstorm::on_styletButton_clicked()
{
    currentInstrument = Instruments::Stylet;
}

void brainstorm::on_eraserButton_clicked()
{
    currentInstrument = Instruments::Eraser;
}

void brainstorm::on_clearButton_clicked()
{
    scene->clear();
    m_Client->sendPoint(QPointF(), -1);
}

void brainstorm::attemptConnection()
{
    if (is_connected)
    {
      is_connected = false;
      return m_Client->disconnectFromHost();
    }

    m_Client->connectToServer(QHostAddress(QString("46.29.115.42")), 1967);
}

void brainstorm::connectedToServer()
{
    if (!meetingID) {
        do {
            bool ok;
            meetingID = new QString(QInputDialog::getText(
                                        this
                                        , tr("Выбор собрания")
                                        , tr("ID собрания")
                                        , QLineEdit::Normal
                                        , QStringLiteral("Новое собрание")
                                        , &ok
                                        ));
            bool isNum = true;
            int ID = meetingID->toInt(&isNum);

            if (ok && *meetingID != QString("Новое собрание") && (meetingID->isEmpty() || !isNum || ID < 0))
            {
                QMessageBox::critical(this,"Ошибка","Введите корректное ID");
                continue;
            } else if (!ok)
                return;
            break;
        } while (true);
        m_Client->chooseMeeting("1:" + *meetingID);
    }

    is_connected = true;
    ui->joinButton->setText(tr("Отключиться"));
    setStyleSheet(styleSheet() + "QPushButton { background-color: rgb(20,20,20); }");
}

void brainstorm::attemptLogin(const QString &userName)
{
    m_Client->login(userName);
}

void brainstorm::loggedIn(const QString& userName)
{
    ui->label_3->setText(ui->label_3->text() + QString(", Имя: ") + userName);
    switchEnabled(true);
}

void brainstorm::loginFailed(const QString &reason)
{
    QMessageBox::critical(this, tr("Ошибка"), reason);
    if (reason == QString("Собрания с таким ID не существует") || reason == QString("К этому собранию уже нельзя присоединиться"))
    {
        meetingID = nullptr;
        connectedToServer();
    } else if (reason == QString("Имя пользователя уже существует"))
        userJoined("", "");
}

void brainstorm::disconnectedFromServer()
{
    QMessageBox::warning(this, tr("Отсоединено"), tr("Соединение прервано"));

    ui->joinButton->setText(tr("Присоединиться к команде"));
    setStyleSheet(styleSheet() + "QPushButton { background-color: rgb(100,100,100); }");

    switchEnabled(false);
    hide();
}

void brainstorm::userJoined(const QString &username, const QString &meetingID)
{
    if (!username.isEmpty() && !meetingID.isEmpty())
    {
        ui->label_3->setText(QStringLiteral("ID собрания: ") + meetingID + QString(", Имя: ") + username);
        return;
    }
    else if (username.isEmpty()) {
        if (!meetingID.isEmpty())
            ui->label_3->setText(QStringLiteral("ID собрания: ") + meetingID);
        const QString newUsername = QInputDialog::getText(this, tr("Выбор пользователя"), tr("Имя пользователя"));
        if (newUsername.isEmpty()){
            is_connected = false;
            return m_Client->disconnectFromHost();
        }
        attemptLogin(newUsername);
        return;
    }

    scene->addUserPreviousPoint();
}

void brainstorm::refreshUsersQuantity(const QVariantMap& users, const QString& type)
{
    if (scene->getPreviousPointsQuantity() == 0)
        for (QVariantMap::const_iterator iter = users.begin(); iter != users.end(); ++iter)
            scene->addUserPreviousPoint();
}

void brainstorm::switchEnabled(bool is_enabled)
{
    ui->table->setEnabled(is_enabled);

    for (QPushButton *but : {ui->clearButton,
         ui->eraserButton,
         ui->styletButton,
         ui->recordButton,
         ui->sendButton
})
        but->setEnabled(is_enabled);
}

void brainstorm::receivePoint(const QPointF& point, qint32 operationCode, qint32 senderID)
{
    switch (operationCode)
    {
    case -1:
        scene->clear();
        break;
    case 0:
        scene->addPoint(point, senderID);
        break;
    case 1:
        scene->removePoint(point, senderID);
        break;
    case 2:
        scene->addLine(point, senderID);
        break;
    }
}

void brainstorm::receiveColor(const QColor& color)
{
    scene->setActiveBrushColor(color);
}

void brainstorm::sendPoint(const QPointF& point, qint32 operationCode)
{
    m_Client->sendColor(scene->getMyBrushColor());
    m_Client->sendPoint(point, operationCode);
}

void brainstorm::error(QAbstractSocket::SocketError socketError)
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
}

void brainstorm::on_changeColorButton_clicked()
{
    QColor newColor = QColorDialog::getColor(scene->getMyBrushColor(), this);
    scene->setMyBrushColor(newColor);
    m_Client->sendColor(newColor);
}

void brainstorm::on_recordButton_clicked()
{
    if (recorder->recorderState() == QMediaRecorder::StoppedState) {
        ui->recordButton->setText(QString("Остановить\nзапись"));

        int index = ui->audioInputBox->currentIndex();
        audioSession->audioInput()->setDevice((index == -1 ?
                                                   QVariant() : ui->audioInputBox->itemData(index)).value<QAudioDevice>());

        recorder->record();
        ui->sendButton->setEnabled(false);
    } else {
        ui->recordButton->setText(QString("Записать\nголосовое\nсообщение"));
        ui->sendButton->setEnabled(true);
        recorder->stop();
    }
}

void brainstorm::on_sendButton_clicked()
{
    QFile file(recorder->outputLocation().toString().remove("file:///") + ".m4a");
    if (!file.open(QFile::ReadWrite))
    {
        QMessageBox::critical(this,"Ошибка","Не удалось открыть файл");
        return;
    }

    QByteArray data;
    data = file.readAll();
    file.close();

    m_Client->sendFile(DataTypes::AUDIO_FILE, data);
}

void brainstorm::receiveFile(DataTypes dataType, const QByteArray &data)
{
    if (dataType == DataTypes::AUDIO_FILE)
    {
        QString fileName = "audio_" + QString::number(++audioFilesCount) + ".m4a";
        QFile file(temporaryDir->path() + "/" + fileName);
        if (!file.open(QFile::ReadWrite))
        {
            QMessageBox::critical(this,"Ошибка","Не удалось открыть файл");
            return;
        }

        file.write(data);
        file.close();

        auto table = ui->audioFilesBox;
        table->insertRow(0);

        QTableWidgetItem *item = new QTableWidgetItem(fileName);
        item->setFlags(item->flags() & (~Qt::ItemIsEditable));
        table->setItem(0, 0, item);

        player->setSource(QUrl::fromLocalFile(temporaryDir->path() + "/" + fileName));
        audioOutput->setVolume(50);
        player->play();
    }
}

void brainstorm::on_audioFilesBox_cellPressed(int row, int column)
{
    player->setSource(QUrl::fromLocalFile(temporaryDir->path() + "/" + ui->audioFilesBox->item(row, column)->text()));
    audioOutput->setVolume(50);
    player->play();
}

