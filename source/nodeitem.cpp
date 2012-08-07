#include "nodeitem.h"
#include "node.h"
#include "graphicsscene.h"
#include <QtCore>
#include <QtGui>

namespace {

static const float Radius = 90;

}

struct NodeItem::Private
{
    Node mNode;
    int mHighlightMode;
};

NodeItem::NodeItem(const Node& node)
    : mPrivate(new Private)
{
    mPrivate->mNode = node;
    mPrivate->mHighlightMode = NodeItem::HighlightOff;
    setRect(-Radius/2, -Radius/2, Radius, Radius);
    setBrush(QBrush(QColor(230, 230, 230, 168)));
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    setZValue(10);
}

void NodeItem::setNode(const Node& node)
{
    mPrivate->mNode = node;
}

Node NodeItem::getNode() const
{
    return mPrivate->mNode;
}

int NodeItem::type() const
{
    return GraphicsScene::NodeType;
}

void NodeItem::setHighlightMode(int mode)
{
    mPrivate->mHighlightMode = mode;
}

void NodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* item, QWidget* widget)
{
    painter->save();
    QPen pen;
    switch (mPrivate->mHighlightMode) {
        case NodeItem::HighlightOff:
        {
            pen.setWidth(5);
            pen.setColor(QColor(128, 128, 128, 50));
        }
        break;
    case NodeItem::HighlightAsStartNode:
        pen.setWidth(2);
        pen.setColor(QColor(255, 0, 0));
        break;
    case NodeItem::HighlightAsFileNode:
        pen.setWidth(5);
        pen.setColor(QColor(0, 200, 0, 128));
        break;
    }
    setPen(pen);

    QGraphicsEllipseItem::paint(painter, item, widget);
    painter->restore();

    painter->save();
    painter->setPen(QColor(0, 0, 0));

    // Draw Name
    QString name = mPrivate->mNode.getName();//QString("Name:%1").arg(mPrivate->mNode.getName());
    painter->drawText(QRectF(-Radius/2, -Radius/2, Radius, Radius/3), Qt::AlignCenter, name);
    // Draw FA
    painter->drawText(QRectF(-Radius/2, Radius/3-Radius/2, Radius, Radius/3), Qt::AlignCenter, mPrivate->mNode.getFA().join(","));
    // Draw PA

    QStringList pa = mPrivate->mNode.getPA().keys();
    painter->drawText(QRectF(-Radius/2, Radius/3*2-Radius/2, Radius, Radius/3), Qt::AlignCenter, pa.join(","));
    painter->restore();
}















