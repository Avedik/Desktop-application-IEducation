#include "painter.h"
#include "brainstorm.h"

Painter::Painter(brainstorm *wrapper, QObject *parent) : QGraphicsScene(parent)
{
    this->wrapper = wrapper;
    previousPoints = new QVector<QPointF*>();
}

Painter::~Painter()
{
    for (int i = 0; i < previousPoints->size(); ++i)
        delete (*previousPoints)[i];
    previousPoints->clear();
    delete previousPoints;
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
                   QBrush(myBrushColor));
        myPreviousPoint = point;
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
        QGraphicsScene::addLine(myPreviousPoint.x(),
                myPreviousPoint.y(),
                point.x(),
                point.y(),
                QPen(myBrushColor, 5, Qt::SolidLine, Qt::RoundCap));

        myPreviousPoint = point;
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

void Painter::addPoint(const QPointF& point, qint32 senderID)
{
    if (previousPoints->at(senderID) == nullptr)
        (*previousPoints)[senderID] = new QPointF();

    previousPoints->at(senderID)->setX(point.x());
    previousPoints->at(senderID)->setY(point.y());
    addEllipse(point.x(),
               point.y(),
               5, 5,
               QPen(Qt::NoPen),
               QBrush(activeBrushColor));
}

void Painter::addLine(const QPointF& point, qint32 senderID)
{
    if (previousPoints->at(senderID) == nullptr)
    {
        addPoint(point, senderID);
        return;
    }

    QGraphicsScene::addLine(previousPoints->at(senderID)->x(),
            previousPoints->at(senderID)->y(),
            point.x(),
            point.y(),
            QPen(activeBrushColor, 5, Qt::SolidLine, Qt::RoundCap));

    previousPoints->at(senderID)->setX(point.x());
    previousPoints->at(senderID)->setY(point.y());
}

void Painter::removePoint(const QPointF& point, qint32 senderID)
{
    if (previousPoints->at(senderID)->x() == point.x() && previousPoints->at(senderID)->y() == point.y())
    {
        delete previousPoints->at(senderID);
        (*previousPoints)[senderID] = nullptr;
    }

    QGraphicsItem* item = itemAt(point, QTransform());
    if (item)
        removeItem(item);
}

void Painter::setMyBrushColor(QColor color)
{
    myBrushColor = color;
}

QColor Painter::getMyBrushColor()
{
    return myBrushColor;
}

void Painter::setActiveBrushColor(QColor color)
{
    activeBrushColor = color;
}

QColor Painter::getActiveBrushColor()
{
    return activeBrushColor;
}

void Painter::addUserPreviousPoint()
{
    previousPoints->push_back(new QPointF());
}

qint32 Painter::getPreviousPointsQuantity()
{
    return previousPoints->size();
}

