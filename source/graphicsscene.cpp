#include "graphicsscene.h"
#include "nodeitem.h"
#include "connectionitem.h"
#include "nodeeditor.h"
#include <QtCore>
#include <QtGui>

namespace {

const static QString FileName = "scene.dat";
const static int NodeCount = 13;
GraphicsScene* SharedGraphicsScene = NULL;

}

struct GraphicsScene::Private
{
    void init(QGraphicsScene*);
    bool serialize();
    bool deserialize();
    void update(QGraphicsScene*);
    NodeItem* X(int);

    QVector<NodeItem*> mNodeItems;
    QVector<ConnectionItem*> mConnectionItems;

    NodeItem* findNodeItem(const QString&) const;
    ConnectionItem* findConnectionItem(const QString&, const QString&);
};

ConnectionItem* GraphicsScene::Private::findConnectionItem(const QString& v1, const QString& v2)
{
    foreach (ConnectionItem* connectionItem, mConnectionItems) {
        if ( ( connectionItem->getNodeName1()==v1 && connectionItem->getNodeName2()==v2 ) ||
             ( connectionItem->getNodeName2()==v1 && connectionItem->getNodeName1()==v2)) {
            return connectionItem;
        }
    }
    return NULL;
}

NodeItem* GraphicsScene::Private::findNodeItem(const QString& name) const
{
    foreach (NodeItem* item, mNodeItems) {
        Node node = item->getNode();
        if (node.getName()==name) {
            return item;
        }
    }

    return NULL;
}

NodeItem* GraphicsScene::Private::X(int idx)
{
    return mNodeItems[idx - 1];
}

void GraphicsScene::Private::init(QGraphicsScene* s)
{
    if (!s)
        return;
    s->setSceneRect(0, 0, 800, 600);

    QVector<Node> nodes = GetNodeEditor().nodes();
    mNodeItems.resize(NodeCount);
    for (int i=0;i<NodeCount;i++) {
        NodeItem* item = new NodeItem(nodes[i]);
        mNodeItems[i] = item;
        s->addItem(item);
    }

    // create connections
    ConnectionItem* connectionItem = NULL;
    connectionItem = new ConnectionItem(X(1), X(2));
    mConnectionItems.append(connectionItem);
    s->addItem(connectionItem);

    connectionItem = new ConnectionItem(X(2), X(3));
    mConnectionItems.append(connectionItem);
    s->addItem(connectionItem);

    connectionItem = (new ConnectionItem(X(3), X(4)));
    mConnectionItems.append(connectionItem);
    s->addItem(connectionItem);

    connectionItem = (new ConnectionItem(X(4), X(5)));
    mConnectionItems.append(connectionItem);
    s->addItem(connectionItem);

    connectionItem = (new ConnectionItem(X(5), X(6)));
    mConnectionItems.append(connectionItem);
    s->addItem(connectionItem);

    connectionItem = (new ConnectionItem(X(6), X(7)));
    mConnectionItems.append(connectionItem);
    s->addItem(connectionItem);

    connectionItem = (new ConnectionItem(X(7), X(8)));
    mConnectionItems.append(connectionItem);
    s->addItem(connectionItem);

    connectionItem = (new ConnectionItem(X(8), X(9)));
    mConnectionItems.append(connectionItem);
    s->addItem(connectionItem);

    connectionItem = (new ConnectionItem(X(9), X(10)));
    mConnectionItems.append(connectionItem);
    s->addItem(connectionItem);

    connectionItem = (new ConnectionItem(X(10), X(5)));
    mConnectionItems.append(connectionItem);
    s->addItem(connectionItem);

    connectionItem = (new ConnectionItem(X(9), X(11)));
    mConnectionItems.append(connectionItem);
    s->addItem(connectionItem);

    connectionItem = (new ConnectionItem(X(11), X(12)));
    mConnectionItems.append(connectionItem);
    s->addItem(connectionItem);

    connectionItem = (new ConnectionItem(X(12), X(4)));
    mConnectionItems.append(connectionItem);
    s->addItem(connectionItem);

    connectionItem = (new ConnectionItem(X(12), X(13)));
    mConnectionItems.append(connectionItem);
    s->addItem(connectionItem);

    connectionItem = new ConnectionItem(X(12), X(1));
    mConnectionItems.append(connectionItem);
    s->addItem(connectionItem);

    deserialize();
}

void GraphicsScene::Private::update(QGraphicsScene* scene)
{
    if (!scene)
        return;

    foreach (QGraphicsItem* item, scene->items()) {
        if (item->type()!=ConnectionType)
            continue;

        ConnectionItem* connectionItem = static_cast<ConnectionItem*>(item);
        connectionItem->update();
    }
    scene->update();
}

bool GraphicsScene::Private::serialize()
{
    QFile f(FileName);
    if (!f.open(QIODevice::WriteOnly)) {
        return false;
    }

    QDataStream ds(&f);
    foreach (const NodeItem* item, mNodeItems) {
        ds << item->pos();
    }

    f.close();
    qDebug() << "[GRAPHICS-SCENE]: serialize scene.dat ok";
    return true;
}

bool GraphicsScene::Private::deserialize()
{
    QFile f(FileName);
    if (!f.open(QIODevice::ReadOnly)) {
        return false;
    }

    QDataStream ds(&f);
    int i=0;
    while (!ds.atEnd()) {
        QPointF pos;
        ds >> pos;

        if (i<NodeCount) {
            mNodeItems[i]->setPos(pos);
            i++;
        }
    }
    f.close();
    qDebug() << "[GRAPHICS-SCENE]: deserialize scene.dat ok";
    return true;
}

GraphicsScene::GraphicsScene(QObject *parent) :
    QGraphicsScene(parent),
    mPrivate(new Private)
{
    mPrivate->init(this);
    SharedGraphicsScene = this;
    mPrivate->update(this);
}

GraphicsScene::~GraphicsScene()
{
    mPrivate->serialize();
    delete mPrivate;
    SharedGraphicsScene = NULL;
}

void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    QGraphicsScene::mouseMoveEvent(mouseEvent);
    mPrivate->update(this);
}

void GraphicsScene::clearHighlight()
{
    foreach (NodeItem* item, mPrivate->mNodeItems) {
        item->setHighlightMode(NodeItem::HighlightOff);
    }
    update();
}

void GraphicsScene::highlightNode(const QString& nodeName)
{
    NodeItem* item = mPrivate->findNodeItem(nodeName);
    if (!item)
        return;
    item->setHighlightMode(NodeItem::HighlightAsFileNode);
}

void GraphicsScene::updateEdgeWeight(int index1, int index2, float weight)
{
    ConnectionItem* connectionItem = mPrivate->findConnectionItem(
                mPrivate->mNodeItems[index1]->getNode().getName(),
                mPrivate->mNodeItems[index2]->getNode().getName());
    if (!connectionItem) {
        //qDebug() << "CAN'T FIND EDGE" << index1 << "-" << index2;
        return;
    }

    connectionItem->setLength(weight);
}

void GraphicsScene::updateNodeData(int index, const Node& node)
{
    mPrivate->mNodeItems[index]->setNode(node);
}

void GraphicsScene::highlightEdge(const QString& nodeName1, const QString& nodeName2)
{
    ConnectionItem* connectionItem = mPrivate->findConnectionItem(nodeName1, nodeName2);
    if (!connectionItem)
        return;
    connectionItem->setHighlightEnabled(true);
}

void GraphicsScene::serialize()
{
    mPrivate->serialize();
}

GraphicsScene& GetGraphicsScene()
{
    return *SharedGraphicsScene;
}

GraphicsScene* GetGraphicsScenePtr()
{
    return SharedGraphicsScene;
}
