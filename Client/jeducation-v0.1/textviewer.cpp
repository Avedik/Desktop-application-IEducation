#include "textviewer.h"
#include "ui_textviewer.h"
#include <QMessageBox>

TextViewer::TextViewer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TextViewer)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
}

TextViewer::~TextViewer()
{
    delete ui;
}

void TextViewer::openWord(QString &filename)
{
    closeOffice();
    m_content = new QAxWidget("Word.Application", this);
    m_content->dynamicCall("SetVisible (bool Visible)","false");
    m_content->setProperty("DisplayAlerts", false);

    ui->gridLayout->addWidget(m_content);
    m_content->setControl(filename);
    m_content->show();
}

void TextViewer::openPdf(QString &filename)
{
    closeOffice();
    m_content = new QAxWidget(this);
    if(!m_content->setControl("Adobe PDF Reader"))
        QMessageBox::critical(this, "Error", "PDF не установлен!");
    ui->gridLayout->addWidget(m_content);
    m_content->dynamicCall(
                "LoadFile(const QString&)",
                filename);
}

void TextViewer::closeOffice()
{
    if(m_content)
    {
        m_content->close();
        m_content->clear();
        delete m_content;
        m_content = nullptr;
    }
}
