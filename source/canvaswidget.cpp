#include "canvaswidget.h"
#include "graphicsscene.h"
#include "createnodedialog.h"
#include "nodeeditor.h"
#include "pathweighteditor.h"

#include <QtGui>
#include <QtCore>


struct CanvasWidget::Private
{
    GraphicsScene* mGraphicsScene;
    QAction* mCreateNodeAction;
    QAction* mDestroyNodeAction;
    QPoint mLastLocation;
    QAction* mCreateConnectionAction;
    QAction* mDestroyConnectionAction;
};


CanvasWidget::CanvasWidget(QWidget *parent) :
    QGraphicsView(parent)
  , mPrivate(new Private)
{
    mPrivate->mGraphicsScene = new GraphicsScene;
    mPrivate->mCreateNodeAction = new QAction("Create Node", this);
    mPrivate->mDestroyNodeAction = new QAction("Destroy Nodes", this);
    mPrivate->mCreateConnectionAction = new QAction("Create Connection", this);
    mPrivate->mDestroyConnectionAction = new QAction("Destroy Connections", this);

    connect(mPrivate->mCreateNodeAction, SIGNAL(triggered()), this, SLOT(slotCreateNode()));
    connect(mPrivate->mDestroyNodeAction, SIGNAL(triggered()), this, SLOT(slotDestroyNodes()));
    connect(mPrivate->mCreateConnectionAction, SIGNAL(triggered()), this, SLOT(slotCreateConnection()));
    connect(mPrivate->mDestroyConnectionAction, SIGNAL(triggered()), this, SLOT(slotDestroyConnections()));
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

void CanvasWidget::slotDestroyConnections()
{
    if (!mPrivate->mGraphicsScene->hasSelectedConnections())
        return;

    int r = QMessageBox::question(this, "Are you sure", "Are you sure to delete selected connections?", QMessageBox::Yes, QMessageBox::No);
    if (r==QMessageBox::No)
        return;

    mPrivate->mGraphicsScene->destroyConnectionItems();
}

CanvasWidget::~CanvasWidget()
{
    delete mPrivate;
}

void CanvasWidget::contextMenuEvent(QContextMenuEvent *event)
{
    mPrivate->mLastLocation = event->pos();
    QMenu menu(this);
    menu.addAction(mPrivate->mCreateNodeAction);
    //menu.addAction(mPrivate->mDestroyNodeAction);
    menu.addAction(mPrivate->mCreateConnectionAction);
    //menu.addAction(mPrivate->mDestroyConnectionAction);
    menu.exec(event->globalPos());
}

void CanvasWidget::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key()==Qt::Key_Backspace || event->key()==Qt::Key_Delete) {
        qDebug() << "Delete something...";

        if (mPrivate->mGraphicsScene->selectedItems().size()>0) {
            int r = QMessageBox::question(this, "Are you sure", "Are you sure delete selected items?", QMessageBox::Yes, QMessageBox::No);
            if (QMessageBox::Yes==r) {
                // TODO
            }
        }
    }
}















