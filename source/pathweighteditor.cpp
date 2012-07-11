#include "pathweighteditor.h"
#include "graphicsscene.h"
#include "ui_pathweighteditor.h"

#include <QtCore>
#include <QtGui>


namespace {

const static int RowCount = 15;
const static QString FileName = "weight.dat";
const static int TimeWeightCol = 1;
const static int PathRelCol    = 2;

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
    void updateData(QTableWidget*);

    bool serialize();
    bool deserialize();

    int mTimeWeights[RowCount];
    float mReliabilities[RowCount];

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

void PathWeightEditor::Private::updateData(QTableWidget* w)
{
    if (!w)
        return;

    if (!deserialize())
        return;

    for (int i=0;i<RowCount;++i) {

        QTableWidgetItem* item = new QTableWidgetItem(QString::number(mTimeWeights[i]));
        w->setItem(i, TimeWeightCol, item);

        item = new QTableWidgetItem(QString::number(mReliabilities[i], 'g', 2));
        w->setItem(i, PathRelCol, item);

        item = w->item(i, 0);
        if (item) {
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        }
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
