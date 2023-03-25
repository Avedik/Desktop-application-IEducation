#ifndef TEXTVIEWER_H
#define TEXTVIEWER_H

#include <QMainWindow>
#include <QAxWidget>

namespace Ui {
class TextViewer;
}

class TextViewer : public QMainWindow
{
    Q_OBJECT

public:
    explicit TextViewer(QWidget *parent = nullptr);
    ~TextViewer();
    void openExcel(QString &filename);
    void openWord(QString &filename);
    void openPdf(QString &filename);
    void closeOffice();

private:
    Ui::TextViewer *ui;
    QAxWidget *m_content = nullptr;

};

#endif // TEXTVIEWER_H
