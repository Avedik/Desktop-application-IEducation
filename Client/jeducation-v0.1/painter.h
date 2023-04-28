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
    ~Painter();
    void addPoint(const QPointF& point, qint32 senderID);
    void addLine(const QPointF& point, qint32 senderID);
    void removePoint(const QPointF& point, qint32 senderID);
    void setMyBrushColor(QColor color);
    QColor getMyBrushColor();
    void setActiveBrushColor(QColor color);
    QColor getActiveBrushColor();
    void addUserPreviousPoint();
    qint32 getPreviousPointsQuantity();

private:
    QPointF myPreviousPoint;
    QVector<QPointF*> *previousPoints;
    brainstorm *wrapper;
    QColor myBrushColor = QColor(Qt::black);
    QColor activeBrushColor = QColor(Qt::black);

private:
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

};

#endif // PAINTER_H
