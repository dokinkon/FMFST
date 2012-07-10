#ifndef NODEITEM_H
#define NODEITEM_H

#include <QGraphicsEllipseItem>

class Node;

class NodeItem : public QGraphicsEllipseItem
{
public:

    typedef enum {
        HighlightOff,
        HighlightAsStartNode,
        HighlightAsFileNode
    } HighlightMode;

    NodeItem(const Node&);
    void setNode(const Node&);
    Node getNode() const;
    int type() const;
    void setHighlightMode(int);
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget* widget = 0);
private:
    struct Private;
    Private* mPrivate;
};

#endif // NODEITEM_H
