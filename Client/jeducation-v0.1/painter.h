#ifndef PAINTER_H
#define PAINTER_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QTimer>
#include <QDebug>

class brainstorm;
class Painter : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit Painter(brainstorm *wrapper, QObject *parent = 0);

private:
    QPointF previousPoint;
    brainstorm *wrapper;

private:
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

};

#endif // PAINTER_H
