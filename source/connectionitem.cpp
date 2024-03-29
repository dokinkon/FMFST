#include "connectionitem.h"
#include "nodeitem.h"
#include "node.h"

#include <QtGui>

struct ConnectionItem::Private
{
    NodeItem* mT1;
    NodeItem* mT2;
    float mLength;
    bool mHighlightEnabled;

    Private()
        : mT1(NULL)
        , mT2(NULL)
        , mHighlightEnabled(false)
    {
    }
    void update(ConnectionItem* p)
    {
        if (!p) {
            return;
        }

        if (!mT1 || !mT2)
            return;

        QPointF p1 = mT1->pos();
        QPointF p2 = mT2->pos();

        p->setLine(QLineF(p1, p2));
    }
};

ConnectionItem::ConnectionItem(NodeItem* t1, NodeItem* t2, float length)
    : mPrivate(new Private)
{
    mPrivate->mT1 = t1;
    mPrivate->mT2 = t2;
    mPrivate->mLength = length;
    QPen pen(QColor(128, 128, 128, 128));
    pen.setWidth(5);
    //setPen(QPen(QColor(0, 0, 0)));
    setPen(pen);
    setFlags(QGraphicsItem::ItemIsSelectable);
}

void ConnectionItem::setLength(float length)
{
    mPrivate->mLength = length;
}

void ConnectionItem::update()
{
    mPrivate->update(this);
}

void ConnectionItem::setHighlightEnabled(bool enabled)
{
    mPrivate->mHighlightEnabled = enabled;
}

void ConnectionItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* item, QWidget* widget)
{
    if (!mPrivate->mT1 || !mPrivate->mT2)
        return;

    painter->save();
    if (mPrivate->mHighlightEnabled) {
        QPen pen(QColor(0, 200, 0, 128));
        pen.setWidth(5);
        setPen(pen);

    } else {
        QPen pen(QColor(128, 128, 128, 128));
        pen.setWidth(5);
        setPen(pen);
    }
    QGraphicsLineItem::paint(painter, item, widget);
    painter->restore();
    painter->save();
    painter->setPen(QPen(QColor(0, 0, 0)));
    QPointF p1 = mPrivate->mT1->pos();
    QPointF p2 = mPrivate->mT2->pos();
    QPointF p = (p1 + p2)/2;
    painter->drawText(p, QString::number(mPrivate->mLength));
    painter->restore();
}

QString ConnectionItem::getNodeName1() const
{
    if (!mPrivate->mT1)
        return QString();

    return mPrivate->mT1->getNode().getName();

}
QString ConnectionItem::getNodeName2() const
{
    if (!mPrivate->mT2)
        return QString();

    return mPrivate->mT2->getNode().getName();
}

int ConnectionItem::getNode1Id() const
{
    if (!mPrivate->mT1)
        return -1;
    return mPrivate->mT1->getNode().getId();
}

int ConnectionItem::getNode2Id() const
{
    if (!mPrivate->mT2)
        return -1;
    return mPrivate->mT2->getNode().getId();
}



















