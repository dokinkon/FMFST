#ifndef CONNECTIONITEM_H
#define CONNECTIONITEM_H

#include <QGraphicsLineItem>

class NodeItem;

class ConnectionItem : public QGraphicsLineItem
{
public:
    enum { Type = UserType + 2 };
    ConnectionItem(NodeItem* t1, NodeItem* t2, float length = 0);
    /*!
     *
     */
    void update();
    /*!
     *
     */
    int type() const { return Type; }
    void setLength(float);
    void setHighlightEnabled(bool);
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget* widget = 0);
    QString getNodeName1() const;
    QString getNodeName2() const;
private:
    struct Private;
    Private* mPrivate;
};

#endif // CONNECTIONITEM_H
