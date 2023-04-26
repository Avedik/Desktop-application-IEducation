#ifndef PAINTER_H
#define PAINTER_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QTimer>
#include <QDebug>
#include <QPointF>
#include <QColor>

class brainstorm;
class Painter : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit Painter(brainstorm *wrapper, QObject *parent = 0);
    void addPoint(const QPointF& point);
    void addLine(const QPointF& point);
    void removePoint(const QPointF& point);
    void setMyBrushColor(QColor color);
    QColor getMyBrushColor();
    void setActiveBrushColor(QColor color);
    QColor getActiveBrushColor();

private:
    QPointF previousPoint;
    brainstorm *wrapper;
    QColor myBrushColor = QColor(Qt::black);
    QColor activeBrushColor = QColor(Qt::black);

private:
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

};

#endif // PAINTER_H
