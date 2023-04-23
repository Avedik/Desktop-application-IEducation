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
        addEllipse(event->scenePos().x() - 2,
                   event->scenePos().y() - 2,
                   5, 5,
                   QPen(Qt::NoPen),
                   QBrush(Qt::black));
        previousPoint = event->scenePos();
    }
    else if (wrapper->getCurrentInstrument() == brainstorm::Instruments::Eraser)
    {
        QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
        if (item)
            removeItem(item);
    }
}

void Painter::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;

    if (wrapper->getCurrentInstrument() == brainstorm::Instruments::Stylet)
    {
        addLine(previousPoint.x(),
                previousPoint.y(),
                event->scenePos().x(),
                event->scenePos().y(),
                QPen(Qt::black, 5, Qt::SolidLine, Qt::RoundCap));

        previousPoint = event->scenePos();
    }
    else if (wrapper->getCurrentInstrument() == brainstorm::Instruments::Eraser)
    {
        QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
        if (item)
            removeItem(item);
    }
}
