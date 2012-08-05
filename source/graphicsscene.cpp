#include "graphicsscene.h"
#include "nodeitem.h"
#include "edge.h"
#include "connectionitem.h"
#include "nodeeditor.h"
#include "pathweighteditor.h"
#include <QtCore>
#include <QtGui>

namespace {

const static QString FileName = "scene.dat";
GraphicsScene* SharedGraphicsScene = NULL;

}

struct GraphicsScene::Private
{
    void init(QGraphicsScene*);
    bool serialize(QGraphicsScene*);
    bool deserialize(QGraphicsScene*);
    void update(QGraphicsScene*);

    NodeItem* findNodeItem(QGraphicsScene*, int nodeId) const;
    ConnectionItem* findConnectionItem(QGraphicsScene*, const Edge& edge);
    QList<ConnectionItem*> getConnectionItems(QGraphicsScene* scene);
};

QList<ConnectionItem*> GraphicsScene::Private::getConnectionItems(QGraphicsScene* scene)
{
    QList<ConnectionItem*> connectionItems;
    if (!scene) {
        return connectionItems;
    }

    foreach (QGraphicsItem* item, scene->items()) {
        if (item->type()!=GraphicsScene::ConnectionType)
            continue;

        ConnectionItem* connectionItem = static_cast<ConnectionItem*>(item);
        connectionItems.append(connectionItem);
    }
    return connectionItems;
}

ConnectionItem* GraphicsScene::Private::findConnectionItem(QGraphicsScene* scene, const Edge& edge)
{
    foreach (ConnectionItem* connectionItem, getConnectionItems(scene)) {
        if ( ( connectionItem->getNode1Id()==edge.v1() && connectionItem->getNode2Id()==edge.v2() ) ||
             ( connectionItem->getNode2Id()==edge.v1() && connectionItem->getNode1Id()==edge.v2())) {
            return connectionItem;
        }
    }
    return NULL;
}

NodeItem* GraphicsScene::Private::findNodeItem(QGraphicsScene* scene, int nodeId) const
{
    if (!scene)
        return NULL;

    foreach (QGraphicsItem* item, scene->items()) {
        if (item->type()!=GraphicsScene::NodeType)
            continue;

        NodeItem* nodeItem = static_cast<NodeItem*>(item);
        if (nodeItem->getNode().getId()==nodeId) {
            return nodeItem;
        }
    }

    return NULL;
}

void GraphicsScene::Private::init(QGraphicsScene* s)
{
    if (!s)
        return;
    s->setSceneRect(0, 0, 800, 600);
}

void GraphicsScene::Private::update(QGraphicsScene* scene)
{
    if (!scene)
        return;


    foreach (const Edge& edge, GetPathWeightEditor().getEdges()) {
        ConnectionItem* connectionItem = findConnectionItem(scene, edge);
        if (!connectionItem)
            continue;

        connectionItem->setLength(edge.length());
    }

    QRectF rectF(0, 0, 800, 600);
    foreach (QGraphicsItem* item, scene->items()) {
        rectF = rectF.united(item->boundingRect());
        if (item->type()!=ConnectionType)
            continue;

        ConnectionItem* connectionItem = static_cast<ConnectionItem*>(item);
        connectionItem->update();
    }

    rectF.adjust(-100, -100, 100, 100);
    scene->setSceneRect(rectF);
    scene->update();
}

bool GraphicsScene::Private::serialize(QGraphicsScene* scene)
{
    QFile f(FileName);
    if (!f.open(QIODevice::WriteOnly)) {
        return false;
    }

    QDataStream ds(&f);

    foreach (QGraphicsItem* item, scene->items()) {
        if (item->type()!=GraphicsScene::NodeType)
            continue;

        NodeItem* nodeItem = static_cast<NodeItem*>(item);
        ds << nodeItem->getNode().getId() << item->pos();
    }

    f.close();
    qDebug() << "[GRAPHICS-SCENE]: serialize scene.dat ok";
    return true;
}

bool GraphicsScene::Private::deserialize(QGraphicsScene* scene)
{
    QFile f(FileName);
    if (!f.open(QIODevice::ReadOnly)) {
        return false;
    }

    QDataStream ds(&f);
    while (!ds.atEnd()) {
        QPointF pos;
        int nodeId;
        ds >> nodeId >> pos;

        NodeItem* nodeItem = findNodeItem(scene, nodeId);
        if (nodeItem) {
            nodeItem->setPos(pos);
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
    mPrivate->serialize(this);
    delete mPrivate;
    SharedGraphicsScene = NULL;
}

void GraphicsScene::initialize()
{
    QVector<Node> nodes = GetNodeEditor().nodes();
    for (int i=0;i<nodes.size();i++) {
        NodeItem* item = new NodeItem(nodes[i]);
        QGraphicsScene::addItem(item);
    }

    QVector<Edge> edges = GetPathWeightEditor().getEdges();
    foreach (const Edge& edge, edges) {
        NodeItem* nodeItem1 = mPrivate->findNodeItem(this, edge.v1());
        NodeItem* nodeItem2 = mPrivate->findNodeItem(this, edge.v2());
        if (!nodeItem1 || !nodeItem2)
            continue;
        ConnectionItem* connectionItem = new ConnectionItem(nodeItem1, nodeItem2, edge.length());
        QGraphicsScene::addItem(connectionItem);
    }

    mPrivate->deserialize(this);
    mPrivate->update(this);
}


QVector<Node> GraphicsScene::getSelectedNodes() const
{
    QVector<Node> nodes;
    QList<QGraphicsItem*> items = QGraphicsScene::selectedItems();
    foreach (QGraphicsItem* item, items) {
        if (item->type()==GraphicsScene::NodeType) {
            NodeItem* nodeItem = static_cast<NodeItem*>(item);
            nodes.append(nodeItem->getNode());
        }
    }
    return nodes;
}

QVector<Edge> GraphicsScene::getSelectedEdges() const
{
    QVector<Edge> edges;
    QList<QGraphicsItem*> items = QGraphicsScene::selectedItems();
    foreach (QGraphicsItem* item, items) {
        if (item->type()!=GraphicsScene::ConnectionType)
            continue;
        ConnectionItem* connectionItem = static_cast<ConnectionItem*>(item);
        int n1 = connectionItem->getNode1Id();
        int n2 = connectionItem->getNode2Id();
        edges.append(Edge(n1, n2, 1, 1));
    }
    return edges;
}

bool GraphicsScene::hasSelectedConnections() const
{
    QList<QGraphicsItem*> items = QGraphicsScene::selectedItems();
    foreach (QGraphicsItem* item, items) {
        if (item->type()==GraphicsScene::ConnectionType) {
            return true;
        }
    }
    return false;
}

void GraphicsScene::createConnectionItem()
{
    QVector<Node> nodes = getSelectedNodes();
    if (nodes.size()!=2)
        return;

    NodeItem* n0 = mPrivate->findNodeItem(this, nodes[0].getId());
    NodeItem* n1 = mPrivate->findNodeItem(this, nodes[1].getId());

    if (!n0 || !n1)
        return;

    ConnectionItem* connectionItem = new ConnectionItem(n0, n1);
    addItem(connectionItem);
    connectionItem->update();
    mPrivate->update(this);
    clearSelection();
}

void GraphicsScene::destroyConnectionItems()
{
    QList<QGraphicsItem*> items = QGraphicsScene::selectedItems();
    foreach (QGraphicsItem* item, items) {
        if (item->type()!=GraphicsScene::ConnectionType)
            continue;

        ConnectionItem* connectionItem = static_cast<ConnectionItem*>(item);

        QGraphicsScene::removeItem(connectionItem);
        delete connectionItem;
    }
    update();
}

void GraphicsScene::destroyConnectionItem(int node1Id, int node2Id)
{
    QList<QGraphicsItem*> items = QGraphicsScene::items();
    foreach (QGraphicsItem* item, items) {
        if (item->type()!=GraphicsScene::ConnectionType)
            continue;

        ConnectionItem* connectionItem = static_cast<ConnectionItem*>(item);
        if (connectionItem->getNode1Id()==node1Id && connectionItem->getNode2Id()==node2Id) {
            QGraphicsScene::removeItem(connectionItem);
            delete connectionItem;
        } else if (connectionItem->getNode1Id()==node2Id && connectionItem->getNode2Id()==node1Id) {
            QGraphicsScene::removeItem(connectionItem);
            delete connectionItem;
        }
    }
}

void GraphicsScene::destroyConnectionItemsRefToNode(int nodeId)
{
    QList<QGraphicsItem*> items = QGraphicsScene::items();
    foreach (QGraphicsItem* item, items) {
        if (item->type()!=GraphicsScene::ConnectionType)
            continue;

        ConnectionItem* connectionItem = static_cast<ConnectionItem*>(item);
        if (connectionItem->getNode1Id()==nodeId || connectionItem->getNode2Id()==nodeId) {
            QGraphicsScene::removeItem(connectionItem);
            delete connectionItem;
        }
    }
}

void GraphicsScene::destroyNodes(const QVector<Node>& nodes)
{
    foreach (const Node& node, nodes) {
        NodeItem* nodeItem = mPrivate->findNodeItem(this, node.getId());
        if (!nodeItem)
            continue;

        QGraphicsScene::removeItem(nodeItem);
        delete nodeItem;
    }
    update();
}

void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    QGraphicsScene::mouseMoveEvent(mouseEvent);
    mPrivate->update(this);
}

void GraphicsScene::clearHighlight()
{
    foreach (QGraphicsItem* item, QGraphicsScene::items()) {
        if (item->type()==GraphicsScene::NodeType) {
            NodeItem* nodeItem = static_cast<NodeItem*>(item);
            nodeItem->setHighlightMode(NodeItem::HighlightOff);
        } else if (item->type()==GraphicsScene::ConnectionType) {
            ConnectionItem* connectionItem = static_cast<ConnectionItem*>(item);
            connectionItem->setHighlightEnabled(false);
        }
    }
    update();
}

void GraphicsScene::createNodeItem(const Node& node, const QPoint& location)
{
    NodeItem* item = new NodeItem(node);
    item->setPos(location);
    addItem(item);
}

void GraphicsScene::destroyNodeItem(int nodeId)
{
    NodeItem* item = mPrivate->findNodeItem(this, nodeId);
    if (!item)
        return;
    removeItem(item);
    delete item;
}

void GraphicsScene::highlightNode(int nodeId)
{
    NodeItem* item = mPrivate->findNodeItem(this, nodeId);
    if (!item)
        return;
    item->setHighlightMode(NodeItem::HighlightAsFileNode);
}

void GraphicsScene::updateEdgeWeight(int index1, int index2, float weight)
{
    /*
    ConnectionItem* connectionItem = mPrivate->findConnectionItem(
                mPrivate->mNodeItems[index1]->getNode().getName(),
                mPrivate->mNodeItems[index2]->getNode().getName());
    if (!connectionItem) {
        //qDebug() << "CAN'T FIND EDGE" << index1 << "-" << index2;
        return;
    }

    connectionItem->setLength(weight);
    */
}

void GraphicsScene::updateNodeData()
{
    foreach (QGraphicsItem* item, QGraphicsScene::items()) {
        if (item->type()!=GraphicsScene::NodeType) 
            continue;

        NodeItem* nodeItem = static_cast<NodeItem*>(item);
        int nodeId = nodeItem->getNode().getId();
        Node node = GetNodeEditor().getNode(nodeId);
        if (node.isValid()) {
            nodeItem->setNode(node);
        }
    }
}

void GraphicsScene::highlightEdge(const Edge& edge)
{
    ConnectionItem* connectionItem = mPrivate->findConnectionItem(this, edge);
    if (!connectionItem)
        return;
    connectionItem->setHighlightEnabled(true);
}

void GraphicsScene::syncAndUpdate()
{
    mPrivate->update(this);
    QGraphicsScene::update();
}

void GraphicsScene::serialize()
{
    mPrivate->serialize(this);
}

GraphicsScene& GetGraphicsScene()
{
    return *SharedGraphicsScene;
}

GraphicsScene* GetGraphicsScenePtr()
{
    return SharedGraphicsScene;
}
