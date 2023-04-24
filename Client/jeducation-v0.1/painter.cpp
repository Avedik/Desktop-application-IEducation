#include "painter.h"
#include "brainstorm.h"

Painter::Painter(brainstorm *wrapper, QObject *parent) : QGraphicsScene(parent)
{
    this->wrapper = wrapper;
}

void Painter::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;

    if (wrapper->getCurrentInstrument() == brainstorm::Instruments::Stylet)
    {
        QPointF point(event->scenePos().x() - 2, event->scenePos().y() - 2);
        addEllipse(point.x(),
                   point.y(),
                   5, 5,
                   QPen(Qt::NoPen),
                   QBrush(brushColor));
        previousPoint = point;
        wrapper->sendPoint(point, 0);
    }
    else if (wrapper->getCurrentInstrument() == brainstorm::Instruments::Eraser)
    {
        QPointF point(event->scenePos());
        QGraphicsItem* item = itemAt(point, QTransform());
        if (item)
        {
            removeItem(item);
            wrapper->sendPoint(point, 1);
        }
    }
}

void Painter::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;

    if (wrapper->getCurrentInstrument() == brainstorm::Instruments::Stylet)
    {
        QPointF point(event->scenePos());
        QGraphicsScene::addLine(previousPoint.x(),
                previousPoint.y(),
                point.x(),
                point.y(),
                QPen(brushColor, 5, Qt::SolidLine, Qt::RoundCap));

        previousPoint = point;
        wrapper->sendPoint(point, 2);
    }
    else if (wrapper->getCurrentInstrument() == brainstorm::Instruments::Eraser)
    {
        QPointF point(event->scenePos());
        QGraphicsItem* item = itemAt(point, QTransform());
        if (item)
        {
            removeItem(item);
            wrapper->sendPoint(point, 1);
        }
    }
}

void Painter::addPoint(const QPointF& point)
{
    addEllipse(point.x(),
               point.y(),
               5, 5,
               QPen(Qt::NoPen),
               QBrush(brushColor));
    previousPoint = point;
}

void Painter::addLine(const QPointF& point)
{
    QGraphicsScene::addLine(previousPoint.x(),
            previousPoint.y(),
            point.x(),
            point.y(),
            QPen(brushColor, 5, Qt::SolidLine, Qt::RoundCap));

    previousPoint = point;
}

void Painter::removePoint(const QPointF& point)
{
    QGraphicsItem* item = itemAt(point, QTransform());
    if (item)
        removeItem(item);
}

void Painter::setBrushColor(QColor color)
{
    brushColor = color;
}

QColor Painter::getBrushColor()
{
    return brushColor;
}
