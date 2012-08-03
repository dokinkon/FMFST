#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QGraphicsItem>

class Node;

class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:

    typedef enum {
        NodeType        = QGraphicsItem::UserType + 1,
        ConnectionType  = QGraphicsItem::UserType + 2,
    } ItemType;

    explicit GraphicsScene(QObject *parent = 0);
    ~GraphicsScene();
    void serialize();
    void clearHighlight();
    void createNodeItem(const Node&, const QPoint&);
    void destroyNodeItem(int nodeId);
    void destroyNodes(const QVector<Node>& nodes);
    /*!
     *
     */
    void createConnectionItem();
    /*!
     *
     */
    void destroyConnectionItems();
    /*!
     *
     */
    QVector<Node> getSelectedNodes() const;
    /*!
     *
     */
    bool hasSelectedConnections() const;


    void highlightNode(const QString&);
    void highlightStartNode(const QString&);
    void highlightEdge(const QString&, const QString&);
    void updateNodeData(int index, const Node&);
    void updateEdgeWeight(int index1, int index2, float weight);

protected:
    void mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    
signals:
    
public slots:

private:
    struct Private;
    Private* mPrivate;
    
};

GraphicsScene& GetGraphicsScene();
GraphicsScene* GetGraphicsScenePtr();

#endif // GRAPHICSSCENE_H
