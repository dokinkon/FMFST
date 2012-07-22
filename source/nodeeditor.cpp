#include "nodeeditor.h"
#include "node.h"
#include "graphicsscene.h"
#include "ui_nodeeditor.h"

namespace {

const static int RowCount = 13;
const static int ColName  = 1;
const static int ColFA    = 2;
const static int ColPA    = 3;

NodeEditor* SharedInstance = NULL;

}


struct NodeEditor::Private
{
    void initContentWidget(QTableWidget*);
    QVector<Node> mNodes;

    // from ui to data model
    void commitNodeData(QTableWidget*);
    // data model to ui
    void updateNodeData(QTableWidget*);
    void resetNodeData();
    bool serialize();
    bool deserialize();
    QStringList parseFPA(const QString&);
    QString invParseFPA(const QStringList&);
    bool mSerializeEnabled;
};

void NodeEditor::Private::resetNodeData()
{
    mNodes.resize(RowCount);
    for (int i=0;i<RowCount;i++) {
        mNodes[i] = Node();
    }
}

QString NodeEditor::Private::invParseFPA(const QStringList& fpa)
{
    QString s;
    for (int i=0;i<fpa.size();++i) {
        if (i!=0) {
            s.append(",");
        }
        s.append(fpa.at(i));
    }
    return s;
}

QString invParsePA(const QMap<QString, int>& pa)
{
    QString s;
    QMap<QString, int>::ConstIterator it = pa.begin();
    for (;it!=pa.end();++it) {
        s.append(",").append(it.key()).append("(").append(QString::number(it.value())).append(")");
    }
    return s;
}

QStringList NodeEditor::Private::parseFPA(const QString& s)
{
    QStringList fpa;
    QStringList tokens = s.split(",");
    foreach (const QString& token, tokens) {
        fpa.append(token.trimmed());
    }
    return fpa;
}

QMap<QString, int> parsePA(const QString& s)
{
    QMap<QString, int> r;
    QStringList tokens = s.split(",");
    foreach (const QString& token, tokens) {
        if (token.contains("(") && token.contains(")")) {
            //qDebug() << "token = " << token;
            int start = token.indexOf("(");
            int end = token.indexOf(")");
            QString programName = token.left(start);
            //qDebug() << "programName = " << programName;
            QString mid = token.mid(start + 1, end - start - 1);
            //qDebug() << "mid = " << mid;
            int familiar = mid.toInt();
            r[programName] = familiar;
        } else {
            r[token] = 100;
        }
    }
    return r;
}

void NodeEditor::Private::updateNodeData(QTableWidget* table)
{
    if (!table)
        return;

    if (!deserialize())
        return;

    foreach (const Node& node, mNodes) {
        int row = node.getId();

        if (row < 0)
            continue;

        // Set Name
        QTableWidgetItem* item = table->item(row, ColName);
        if (item)
            item->setText(node.getName());

        // Set FA
        item = table->item(row, ColFA);
        if (item)
            item->setText(invParseFPA(node.getFA()));

        // Set PA
        item = table->item(row, ColPA);
        if (item) {
            item->setText(invParsePA(node.getPA()));
        }

    }

    qDebug() << "[NODE-EDITOR]: update node data ok";

}

void NodeEditor::Private::commitNodeData(QTableWidget* table)
{
    if (!table)
        return;

    for (int row=0;row<table->rowCount();++row) {

        Node& node = mNodes[row];
        node.setId(row);

        // Name
        QTableWidgetItem* item = table->item(row, ColName);
        if (item) {
            QString name = item->text();
            if (name.isEmpty()||name.isNull()) {
                node.reset();
            } else {
                node.setName(item->text());
                node.setId(row);
            }
        }

        // FA
        item = table->item(row, ColFA);
        if (item) {
            node.setFA(parseFPA(item->text()));
        }

        // PA
        item = table->item(row, ColPA);
        if (item) {
            node.setPA(parsePA(item->text()));
        }
    }
    serialize();

    if (!GetGraphicsScenePtr())
        return;

    for (int i=0;i<mNodes.size();++i) {
        GetGraphicsScene().updateNodeData(i, mNodes[i]);
    }
    GetGraphicsScene().update();
}

bool NodeEditor::Private::serialize()
{
    if (!mSerializeEnabled)
        return false;

    QFile f("node.dat");
    if (!f.open(QIODevice::WriteOnly)) {
        qDebug() << "FAILED TO OPEN node.dat";
        return false;
    }

    QDataStream ds(&f);
    foreach (const Node& node, mNodes) {
        if (node.getId() < 0)
            continue;

        ds << node.getId();
        ds << node.getName();
        ds << node.getFA();
        ds << node.getPA();
        //qDebug() << "pa = " << node.getPA();
    }
    f.close();
    qDebug() << "[NODE-EDITOR]: serialize node.dat ok";
    return true;
}

bool NodeEditor::Private::deserialize()
{
    QFile f("node.dat");
    //QFile f(":defaults/resource/node.dat.default");
    if (!f.open(QIODevice::ReadOnly)) {
        qDebug() << "FAILED TO OPEN \"node.dat\"";
        return false;
    }

    resetNodeData();
    //mNodes.clear();

    QDataStream ds(&f);
    while (!ds.atEnd()) {
        int id;
        QString name;
        QStringList fa;
        QMap<QString, int> pa;
        ds >> id >> name >> fa >> pa;

        pa.remove("");
        qDebug() << pa;

        if (id>=0 && id<RowCount) {
            mNodes[id].setId(id);
            mNodes[id].setName(name);
            mNodes[id].setFA(fa);
            mNodes[id].setPA(pa);
        }
    }
    qDebug() << "[NODE-EDITOR]: deserialize node.dat ok";
    return true;
}

void NodeEditor::Private::initContentWidget(QTableWidget* w)
{
    if (!w)
        return;

    for (int r=0;r<RowCount;r++) {
        w->setItem(r, ColName, new QTableWidgetItem);
        w->setItem(r, ColFA,   new QTableWidgetItem);
        w->setItem(r, ColPA,   new QTableWidgetItem);
    }

    mNodes.resize(RowCount);
    updateNodeData(w);
}

NodeEditor::NodeEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NodeEditor),
    mPrivate(new Private)
{
    mPrivate->mSerializeEnabled = false;
    ui->setupUi(this);
    connect(ui->pushButtonUpdate, SIGNAL(clicked()), this, SLOT(updateButtonClicked()));
    connect(ui->tableWidgetContent, SIGNAL(cellChanged(int,int)), this, SLOT(slotCellChanged(int,int)));
    mPrivate->initContentWidget(ui->tableWidgetContent);
    SharedInstance = this;
    mPrivate->mSerializeEnabled = true;

    for (int i=0;i<RowCount;++i) {
        QTableWidgetItem* item = ui->tableWidgetContent->item(i, 0);
        item->setCheckState(Qt::Unchecked);
    }

}

NodeEditor::~NodeEditor()
{
    delete ui;
    delete mPrivate;
    SharedInstance = NULL;
}

QVector<Node> NodeEditor::nodes() const
{
    return mPrivate->mNodes;
}

void NodeEditor::closeEvent(QCloseEvent* e)
{
    mPrivate->serialize();
    QWidget::closeEvent(e);
}

void NodeEditor::updateButtonClicked()
{
    mPrivate->commitNodeData(ui->tableWidgetContent);
}

bool NodeEditor::hasProgram(const QString& program) const
{
    foreach (const Node& node, mPrivate->mNodes) {
        QMap<QString, int> pa = node.getPA();
        if (pa.contains(program))
            return true;
    }
    return false;
}

bool NodeEditor::hasFile(const QString& file) const
{
    foreach (const Node& node, mPrivate->mNodes) {
        QStringList fa = node.getFA();
        if (fa.contains(file)) {
            return true;
        }
    }
    return false;
}

QSet<int> NodeEditor::getNecessaryNodes() const
{
    QSet<int> nodes;

    for (int i=0;i<RowCount;++i) {
        QTableWidgetItem* item = ui->tableWidgetContent->item(i, 0);
        if (item->checkState()==Qt::Checked) {
            nodes.insert(i);
        }
    }
    return nodes;
}

void NodeEditor::slotCellChanged(int row, int col)
{
    //qDebug() << "cell changed" << row << ", " << col;
    //if (mPrivate->mNodes.size()==RowCount)
        //mPrivate->commitNodeData(ui->tableWidgetContent);
}

NodeEditor& GetNodeEditor()
{
    return *SharedInstance;
}

NodeEditor* GetNodeEditorPtr()
{
    return SharedInstance;
}
