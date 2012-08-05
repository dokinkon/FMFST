#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QGraphicsItem>

class Node;
class Edge;

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
    /*!
     * must be called after nodeeditor and edgeeditor initialized
     */
    void initialize();
    void serialize();
    void clearHighlight();
    void createNodeItem(const Node&, const QPoint&);
    void destroyNodeItem(int nodeId);
    void destroyNodes(const QVector<Node>& nodes);
    /*!
     *
     */
    void syncAndUpdate();
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
    void destroyConnectionItem(int, int);
    /*!
     *
     */
    void destroyConnectionItemsRefToNode(int nodeId);
    /*!
     *
     */
    QVector<Node> getSelectedNodes() const;
    /*!
     *
     */
    QVector<Edge> getSelectedEdges() const;
    /*!
     *
     */
    bool hasSelectedConnections() const;


    void highlightNode(int nodeId);
    void highlightStartNode(int nodeId);
    void highlightEdge(const Edge&);
    void updateNodeData();
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
