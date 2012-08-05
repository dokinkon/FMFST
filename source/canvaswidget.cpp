#include "canvaswidget.h"
#include "graphicsscene.h"
#include "createnodedialog.h"
#include "nodeeditor.h"
#include "edge.h"
#include "pathweighteditor.h"

#include <QtGui>
#include <QtCore>


struct CanvasWidget::Private
{
    GraphicsScene* mGraphicsScene;
    QAction* mCreateNodeAction;
    QPoint mLastLocation;
    QAction* mCreateConnectionAction;

    void deleteSelectedItems();
    void deleteSelectedNodes(const QVector<Node>& nodes);
    void deleteSelectedEdges(const QVector<Edge>& edges);
};

void CanvasWidget::Private::deleteSelectedEdges(const QVector<Edge>& edges)
{
    foreach (const Edge& edge, edges) {
        GetPathWeightEditor().destroyConnection(edge.v1(), edge.v2());
        GetGraphicsScene().destroyConnectionItem(edge.v1(), edge.v2());
    }
    mGraphicsScene->update();
}

void CanvasWidget::Private::deleteSelectedNodes(const QVector<Node>& nodes)
{
    foreach (const Node& node, nodes) {
        GetPathWeightEditor().destroyConnectionRefToNode(node.getId());
        GetGraphicsScene().destroyConnectionItemsRefToNode(node.getId());
    }
    GetNodeEditor().destroyNodes(nodes);
    mGraphicsScene->destroyNodes(nodes);
    mGraphicsScene->update();
}

void CanvasWidget::Private::deleteSelectedItems()
{
    QVector<Edge> edges = mGraphicsScene->getSelectedEdges();

    // Delete selected nodes
    QVector<Node> nodes = mGraphicsScene->getSelectedNodes();
    if (!nodes.isEmpty()) {
        deleteSelectedNodes(nodes);
    }

    if (!edges.isEmpty()) {
        deleteSelectedEdges(edges);
    }
}

CanvasWidget::CanvasWidget(QWidget *parent) :
    QGraphicsView(parent)
  , mPrivate(new Private)
{
    mPrivate->mGraphicsScene = new GraphicsScene;
    mPrivate->mCreateNodeAction = new QAction("Create Node", this);
    mPrivate->mCreateConnectionAction = new QAction("Create Connection", this);

    connect(mPrivate->mCreateNodeAction, SIGNAL(triggered()), this, SLOT(slotCreateNode()));
    connect(mPrivate->mCreateConnectionAction, SIGNAL(triggered()), this, SLOT(slotCreateConnection()));
    QGraphicsView::setScene(mPrivate->mGraphicsScene);
}

void CanvasWidget::slotCreateNode()
{
    CreateNodeDialog dialog(this);
    if (QDialog::Accepted==dialog.exec()) {
        QString name = dialog.getNodeName();
        QString fa = dialog.getFA();
        QString pa = dialog.getPA();

        Node node = GetNodeEditor().createNode(name, fa, pa);
        mPrivate->mGraphicsScene->createNodeItem(node, mPrivate->mLastLocation);
    }
}

/*
void CanvasWidget::slotDestroyNodes()
{
    QVector<Node> nodes = mPrivate->mGraphicsScene->getSelectedNodes();
    if (nodes.isEmpty())
        return;

    QString message("Are you sure deleting following nodes?\n");
    foreach (const Node& node, nodes) {
        message.append(node.getName()).append(", ");
    }

    if (QMessageBox::Yes==QMessageBox::question(this, "Are you sure?", message, QMessageBox::Yes, QMessageBox::No)) {
        mPrivate->mGraphicsScene->destroyNodes(nodes);
        GetNodeEditor().destroyNodes(nodes);
        // TODO
    }
}
*/

void CanvasWidget::slotCreateConnection()
{
    QVector<Node> nodes = mPrivate->mGraphicsScene->getSelectedNodes();
    if (nodes.size()!=2) {
        QMessageBox::information(this, "Need 2 nodes", "Please select 2 nodes first.");
        return;
    }

    if (GetPathWeightEditor().hasEdge(nodes[0].getId(), nodes[1].getId())) {
        QMessageBox::information(this, "Connected", "These two nodes have been connected already");
        return;
    }

    mPrivate->mGraphicsScene->createConnectionItem();
    GetPathWeightEditor().createEdge(nodes[0].getId(), nodes[1].getId());
}

/*
void CanvasWidget::slotDestroyConnections()
{
    if (!mPrivate->mGraphicsScene->hasSelectedConnections())
        return;

    int r = QMessageBox::question(this, "Are you sure", "Are you sure to delete selected connections?", QMessageBox::Yes, QMessageBox::No);
    if (r==QMessageBox::No)
        return;

    mPrivate->mGraphicsScene->destroyConnectionItems();
}
*/

CanvasWidget::~CanvasWidget()
{
    delete mPrivate;
}

void CanvasWidget::contextMenuEvent(QContextMenuEvent *event)
{
    mPrivate->mLastLocation = event->pos();
    QMenu menu(this);
    menu.addAction(mPrivate->mCreateNodeAction);
    menu.addAction(mPrivate->mCreateConnectionAction);
    menu.exec(event->globalPos());
}

void CanvasWidget::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key()==Qt::Key_Backspace || event->key()==Qt::Key_Delete) {
        qDebug() << "Delete something...";

        if (mPrivate->mGraphicsScene->selectedItems().size()>0) {
            int r = QMessageBox::question(this, "Are you sure", "Are you sure delete selected items?", QMessageBox::Yes, QMessageBox::No);
            if (QMessageBox::Yes==r) {
                mPrivate->deleteSelectedItems();
                // TODO
            }
        }
    }
}















