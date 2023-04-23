#ifndef BRAINSTORM_H
#define BRAINSTORM_H

#include <QDialog>
#include <QGraphicsView>
#include <painter.h>

namespace Ui {
class brainstorm;
}

class brainstorm : public QDialog
{
    Q_OBJECT

public:
    enum class Instruments { Stylet, Eraser };
    explicit brainstorm(QWidget *parent = nullptr);
    ~brainstorm();
    Instruments getCurrentInstrument();

private slots:
    void on_styletButton_clicked();

    void on_eraserButton_clicked();

    void on_clearButton_clicked();

private:
    Instruments currentInstrument;
    Ui::brainstorm *ui;
    Painter *scene;
};

#endif // BRAINSTORM_H
