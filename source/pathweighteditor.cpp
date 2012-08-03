#include "pathweighteditor.h"
#include "nodeeditor.h"
#include "graphicsscene.h"
#include "edge.h"
#include "node.h"
#include "ui_pathweighteditor.h"

#include <QtCore>
#include <QtGui>


namespace {

const static int RowCount = 15;
const static QString FileName = "weight.dat";
const static int TimeWeightCol = 1;
const static int PathRelCol    = 2;


static const int RoleNode1 = Qt::UserRole + 1;
static const int RoleNode2 = Qt::UserRole + 2;

static const int edgeMap[][2] = {
    {0, 1},
    {0, 11},
    {1, 2},
    {2, 3},
    {3, 4},
    {4, 5},
    {5, 6},
    {6, 7},
    {4, 9},
    {8, 9},
    {7, 8},
    {8, 10},
    {10, 11},
    {11, 12},
    {3, 11},
};

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
    bool deserialize();

    int mTimeWeights[RowCount];
    float mReliabilities[RowCount];

    QList<Edge> mEdges;






    bool mSerializeEnabled;
};

void PathWeightEditor::Private::commitData(QTableWidget* w)
{
    if (!w)
        return;

    resetContentData();

    for (int i=0;i<RowCount;i++) {
        QTableWidgetItem* item = w->item(i, TimeWeightCol);
        if (item) {
            mTimeWeights[i] = item->text().toInt();
        }

        item = w->item(i, PathRelCol);
        if (item) {
            mReliabilities[i] = item->text().toFloat();
        }
    }

    serialize();

    // update data to graphics scene
    if (!GetGraphicsScenePtr())
        return;

    for (int i=0;i<RowCount;i++) {
        float weight = mTimeWeights[i] / mReliabilities[i];
        GetGraphicsScene().updateEdgeWeight(edgeMap[i][0], edgeMap[i][1], weight);
    }
    GetGraphicsScene().update();
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
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        w->setItem(i, 0, item);

        // create time weight
        item = new QTableWidgetItem(QString::number(edge.getTimeWeight()));
        w->setItem(i, TimeWeightCol, item);

        item = new QTableWidgetItem(QString::number(edge.getReliability(), 'g', 2));
        w->setItem(i, PathRelCol, item);
    }
}

bool PathWeightEditor::Private::serialize()
{
    if (!mSerializeEnabled)
        return false;

    QFile f(FileName);
    if (!f.open(QIODevice::WriteOnly))
        return false;

    QDataStream ds(&f);
    for (int i=0;i<RowCount;++i) {
        ds << mTimeWeights[i] << mReliabilities[i];
    }

    f.close();
    return true;
}

bool PathWeightEditor::Private::deserialize()
{
    QFile f(FileName);
    if (!f.open(QIODevice::ReadOnly))
        return false;

    resetContentData();

    QDataStream ds(&f);
    int i = 0;
    while (!ds.atEnd()) {
        ds >> mTimeWeights[i];
        ds >> mReliabilities[i];
        ++i;
    }
    f.close();
    return true;
}

void PathWeightEditor::Private::resetContentData()
{
    for (int i=0;i<RowCount;++i) {
        mTimeWeights[i] = 0;
        mReliabilities[i] = 0.0f;
    }
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
    connect(ui->tableWidgetContent, SIGNAL(cellChanged(int,int)), this, SLOT(slotCellChanged(int,int)));

    mPrivate->initContentWidget(ui->tableWidgetContent);
    sharedInstance = this;
    mPrivate->mSerializeEnabled = true;
}

PathWeightEditor::~PathWeightEditor()
{
    delete ui;
    delete mPrivate;
    sharedInstance = NULL;
}

void PathWeightEditor::updateButtonClicked()
{
    mPrivate->commitData(ui->tableWidgetContent);
}

void PathWeightEditor::slotCellChanged(int, int)
{
   // mPrivate->commitData(ui->tableWidgetContent);
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

QVector<float> PathWeightEditor::pathWeights() const
{
    QVector<float> weights;
    weights.resize(RowCount);

    for (int i=0;i<RowCount;i++) {
        weights[i] = (float)mPrivate->mTimeWeights[i] / mPrivate->mReliabilities[i];
    }

    return weights;
}

PathWeightEditor& GetPathWeightEditor()
{
    return *sharedInstance;
}
