#include "pathweighteditor.h"
#include "nodeeditor.h"
#include "graphicsscene.h"
#include "edge.h"
#include "node.h"
#include "ui_pathweighteditor.h"

#include <QtCore>
#include <QtGui>


namespace {

const static QString FileName = "edges.dat";
const static int TimeWeightCol = 1;
const static int PathRelCol    = 2;


static const int RoleNode1 = Qt::UserRole + 1;
static const int RoleNode2 = Qt::UserRole + 2;

static PathWeightEditor* sharedInstance = NULL;

}

struct PathWeightEditor::Private
{
    void initContentWidget(QTableWidget*);
    void resetContentData();

    // from ui to data model
    void commitData(QTableWidget*);
    // from data model to ui
    void updateData(QTableWidget*, bool needDeserialize = true);

    bool serialize();
    bool serializeToText();
    bool deserialize();
    bool deserializeFromText();

    QList<Edge> mEdges;

    bool mSerializeEnabled;
};

// from ui to data model
void PathWeightEditor::Private::commitData(QTableWidget* w)
{
    if (!w)
        return;

    mEdges.clear();

    for (int i=0;i<w->rowCount();i++) {
        float timeWeight = 1.0f;
        float reliability = 1.0f;
        QTableWidgetItem* item = w->item(i, TimeWeightCol);
        if (item) {
            timeWeight = item->text().toFloat();
        }

        item = w->item(i, PathRelCol);
        if (item) {
            reliability = item->text().toFloat();
        }

        // get name item
        item = w->item(i, 0);
        int node1Id = item->data(RoleNode1).toInt();
        int node2Id = item->data(RoleNode2).toInt();

        mEdges.append(Edge(node1Id, node2Id, timeWeight, reliability));
    }

    serialize();

    // update data to graphics scene
    if (!GetGraphicsScenePtr())
        return;

    GetGraphicsScene().syncAndUpdate();
}

void PathWeightEditor::Private::updateData(QTableWidget* w, bool needDeserialize)
{
    if (!w)
        return;

    if (needDeserialize) {
        if (!deserialize())
            return;
    }

    w->clearContents();
    w->setRowCount(mEdges.size());
    w->setColumnCount(3);

    for (int i=0;i<mEdges.size();++i) {

        const Edge& edge = mEdges.at(i);
        Node node1 = GetNodeEditor().getNode(edge.v1());
        Node node2 = GetNodeEditor().getNode(edge.v2());

        // create name item
        QString name = QString("%1, %2").arg(node1.getName()).arg(node2.getName());
        QTableWidgetItem* item = new QTableWidgetItem(name);
        item->setData(RoleNode1, node1.getId());
        item->setData(RoleNode2, node2.getId());
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        w->setItem(i, 0, item);

        // create time weight
        item = new QTableWidgetItem(QString::number(edge.getTimeWeight()));
        w->setItem(i, TimeWeightCol, item);

        item = new QTableWidgetItem(QString::number(edge.getReliability(), 'g', 2));
        w->setItem(i, PathRelCol, item);
    }
}

bool PathWeightEditor::Private::serializeToText()
{
    if (!mSerializeEnabled) {
        qDebug() << "[WARNING][EDGE-EDITOR]:Failed to serialize";
        return false;
    }
    QFile f(FileName);
    if (!f.open(QIODevice::WriteOnly))
        return false;

    QTextStream ts(&f);

    foreach (const Edge& edge, mEdges) {
        ts << edge.v1() << ";" << edge.v2() << ";" << edge.getTimeWeight() << ";" << edge.getReliability() << "\n";
    }

    //qDebug() << "[EDGE-EDITOR]:serialize ok (" << mEdges.size() << ")";
    f.close();
    return true;
}

bool PathWeightEditor::Private::deserializeFromText()
{
    QFile f(FileName);
    if (!f.open(QIODevice::ReadOnly))
        return false;

    mEdges.clear();

    QTextStream ts(&f);
    while (!ts.atEnd()) {
        QString line = ts.readLine();
        int node1Id = line.section(";", 0, 0).toInt();
        int node2Id = line.section(";", 1, 1).toInt();
        float timeWeight = line.section(";", 2, 2).toFloat();
        float reliability = line.section(";", 3, 3).toFloat();
        mEdges.append(Edge(node1Id, node2Id, timeWeight, reliability));
    }
    f.close();
    qDebug() << "[EDGE-MANAGER]: deserialize ok (" << mEdges.size() << ")";
    return true;
}

bool PathWeightEditor::Private::serialize()
{
    if (!mSerializeEnabled) {
        qDebug() << "[WARNING][EDGE-EDITOR]:Failed to serialize";
        return false;
    }

    QFile f(FileName);
    if (!f.open(QIODevice::WriteOnly))
        return false;

    QDataStream ds(&f);

    foreach (const Edge& edge, mEdges) {
        ds << edge.v1() << edge.v2() << edge.getTimeWeight() << edge.getReliability();
    }

    qDebug() << "[EDGE-EDITOR]:serialize ok (" << mEdges.size() << ")";

    f.close();
    return true;
}

bool PathWeightEditor::Private::deserialize()
{
    QFile f(FileName);
    if (!f.open(QIODevice::ReadOnly))
        return false;

    mEdges.clear();

    QDataStream ds(&f);
    while (!ds.atEnd()) {
        int node1Id, node2Id;
        float timeWeight, reliability;
        ds >> node1Id >> node2Id >> timeWeight >> reliability;
        mEdges.append(Edge(node1Id, node2Id, timeWeight, reliability));
    }
    f.close();
    qDebug() << "[EDGE-MANAGER]: deserialize ok (" << mEdges.size() << ")";
    return true;
}

void PathWeightEditor::Private::resetContentData()
{
    mEdges.clear();
}

void PathWeightEditor::Private::initContentWidget(QTableWidget *w)
{
    if (!w)
        return;
    updateData(w);
}

PathWeightEditor::PathWeightEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PathWeightEditor),
    mPrivate(new Private)
{
    mPrivate->mSerializeEnabled = false;
    ui->setupUi(this);
    connect(ui->pushButtonUpdate, SIGNAL(clicked()), this, SLOT(updateButtonClicked()));
    sharedInstance = this;
    mPrivate->mSerializeEnabled = true;
}

PathWeightEditor::~PathWeightEditor()
{
    delete ui;
    delete mPrivate;
    sharedInstance = NULL;
}

void PathWeightEditor::initialize()
{
    mPrivate->initContentWidget(ui->tableWidgetContent);
}

void PathWeightEditor::updateButtonClicked()
{
    mPrivate->commitData(ui->tableWidgetContent);
}

bool PathWeightEditor::hasEdge(int v1, int v2) const
{
    foreach (const Edge& edge, mPrivate->mEdges) {
        if (edge.v1()==v1 && edge.v2()==v2)
            return true;

        if (edge.v1()==v2 && edge.v2()==v1)
            return true;
    }
    return false;
}

void PathWeightEditor::destroyConnection(int node1Id, int node2Id)
{
    QList<Edge>::Iterator it = mPrivate->mEdges.begin();
    while (it!=mPrivate->mEdges.end()) {
        const Edge& edge = *it;
        if (edge.v1()==node1Id && edge.v2()==node2Id) {
            it = mPrivate->mEdges.erase(it);
        } else if (edge.v1()==node2Id && edge.v2()==node1Id) {
            it = mPrivate->mEdges.erase(it);
        } else {
            ++it;
        }
    }
    mPrivate->updateData(ui->tableWidgetContent, false);

}
void PathWeightEditor::destroyConnectionRefToNode(int nodeId)
{
    QList<Edge>::Iterator it = mPrivate->mEdges.begin();
    while (it!=mPrivate->mEdges.end()) {
        const Edge& edge = *it;
        if (edge.v1()==nodeId || edge.v2()==nodeId) {
            it = mPrivate->mEdges.erase(it);
        } else {
            ++it;
        }
    }
    mPrivate->updateData(ui->tableWidgetContent, false);
}

void PathWeightEditor::createEdge(int v1, int v2)
{
    if (hasEdge(v1, v2))
        return;

    Node node1 = GetNodeEditor().getNode(v1);
    Node node2 = GetNodeEditor().getNode(v2);

    if (!node1.isValid() || !node2.isValid()) {
        qDebug() << "Failed to create edge.";
        return;
    }

    mPrivate->mEdges.append(Edge(v1, v2, 1.0f, 1.0f));
    mPrivate->updateData(ui->tableWidgetContent, false);
}

QVector<Edge> PathWeightEditor::getEdges() const
{
    return mPrivate->mEdges.toVector();
}

QVector<float> PathWeightEditor::pathWeights() const
{
    QVector<float> weights;


    foreach (const Edge& edge, mPrivate->mEdges) {
        weights.append(edge.length());
    }

    return weights;
}

void PathWeightEditor::updateNodeData()
{
    mPrivate->updateData(ui->tableWidgetContent, false);
}

void PathWeightEditor::serialize()
{
    mPrivate->serialize();
}

void PathWeightEditor::closeEvent(QCloseEvent* e)
{
    mPrivate->serialize();
    QWidget::closeEvent(e);
}

PathWeightEditor& GetPathWeightEditor()
{
    return *sharedInstance;
}
