#include "nodeeditor.h"
#include "node.h"
#include "graphicsscene.h"
#include "ui_nodeeditor.h"
#include <cassert>

namespace {

const static int ColName  = 1;
const static int ColFA    = 2;
const static int ColPA    = 3;

NodeEditor* SharedInstance = NULL;

static const int RoleNodeId = Qt::UserRole;


}


struct NodeEditor::Private
{
    void initContentWidget(QTableWidget*);
    QList<Node> mNodes;

    // from ui to data model
    void commitNodeData(QTableWidget*);
    // data model to ui
    void updateNodeData(QTableWidget*, bool needDeserialize = true);
    void resetNodeData();
    bool serialize();
    bool deserialize();
    int generateId() const;

    QStringList parseFPA(const QString&);
    QString invParseFPA(const QStringList&);
    bool mSerializeEnabled;
};

int NodeEditor::Private::generateId() const
{
    int id = -1;

    bool found;
    do {
        id++;
        found = true;
        foreach (const Node& node, mNodes) {
            if (node.getId()==id) {
                found = false;
                break;
            }
        }
    } while (!found);

    return id;
}

void NodeEditor::Private::resetNodeData()
{
    mNodes.clear();
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

// from data model to ui
void NodeEditor::Private::updateNodeData(QTableWidget* table, bool needDeserialize)
{
    if (!table)
        return;

    if (needDeserialize) {
        if (!deserialize())
            return;
    }

    table->clearContents();
    table->setRowCount(mNodes.size());

    for (int i=0;i<mNodes.size();++i) {
        const Node& node = mNodes.at(i);

        // Set Necessary
        QTableWidgetItem* item = table->item(i, 0);
        if (!item) {
            item = new QTableWidgetItem("");
            table->setItem(i, 0, item);
        }
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);

        // Set Name & Id...
        item = table->item(i, ColName);
        if (!item) {
            item = new QTableWidgetItem(node.getName());
            table->setItem(i, ColName, item);
        }
        item->setText(node.getName());
        item->setData(RoleNodeId, node.getId());

        // Set FA
        item = table->item(i, ColFA);
        if (!item) {
            item = new QTableWidgetItem("");
            table->setItem(i, ColFA, item);
        }
        item->setText(invParseFPA(node.getFA()));

        // Set PA
        item = table->item(i, ColPA);
        if (!item) {
            item = new QTableWidgetItem("");
            table->setItem(i, ColPA, item);
        }
        item->setText(invParsePA(node.getPA()));
    }

    qDebug() << "[NODE-EDITOR]: update node data ok";
}

// from ui to data model
void NodeEditor::Private::commitNodeData(QTableWidget* table)
{
    if (!table)
        return;

    mNodes.clear();

    for (int row=0;row<table->rowCount();++row) {


        // Name & Id
        QTableWidgetItem* item = table->item(row, ColName);
        if (!item)
            continue;

        Node node;
        node.setName(item->text());
        node.setId(row);

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

        mNodes.append(node);
    }
    serialize();

    if (!GetGraphicsScenePtr())
        return;

    for (int i=0;i<mNodes.size();++i) {
        GetGraphicsScene().updateNodeData(i, mNodes.at(i));
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

    mNodes.clear();

    QDataStream ds(&f);
    while (!ds.atEnd()) {
        int id;
        QString name;
        QStringList fa;
        QMap<QString, int> pa;
        ds >> id >> name >> fa >> pa;

        pa.remove("");
        Node node;
        node.setId(id);
        node.setName(name);
        node.setFA(fa);
        node.setPA(pa);

        mNodes.append(node);
    }
    qDebug() << "[NODE-EDITOR]: deserialize node.dat ok";
    return true;
}

void NodeEditor::Private::initContentWidget(QTableWidget* w)
{
    if (!w)
        return;
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
}

NodeEditor::~NodeEditor()
{
    delete ui;
    delete mPrivate;
    SharedInstance = NULL;
}

Node NodeEditor::getNode(int nodeId) const
{
    foreach (const Node& node, mPrivate->mNodes) {
        if (node.getId()==nodeId)
            return node;
    }
    return Node();
}

Node NodeEditor::createNode(const QString& name, const QString& fa, const QString& pa)
{
    Node node;
    node.setId(mPrivate->generateId());
    node.setName(name);
    node.setFA(mPrivate->parseFPA(fa));
    node.setPA(parsePA(pa));
    mPrivate->mNodes.append(node);
    mPrivate->updateNodeData(ui->tableWidgetContent, false);
    mPrivate->serialize();
    return node;
}

void NodeEditor::destroyNodes(const QVector<Node>& nodes)
{
    foreach (const Node& node, nodes) {
        QList<Node>::Iterator it = mPrivate->mNodes.begin();
        while (it!=mPrivate->mNodes.end()) {
            if (node.getId()==it->getId()) {
                it = mPrivate->mNodes.erase(it);
            } else {
                it++;
            }
        }
    }
    mPrivate->updateNodeData(ui->tableWidgetContent, false);
}

QVector<Node> NodeEditor::nodes() const
{
    return mPrivate->mNodes.toVector();
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

    for (int i=0;i<ui->tableWidgetContent->rowCount();++i) {
        QTableWidgetItem* item = ui->tableWidgetContent->item(i, 0);
        if (item->checkState()==Qt::Checked) {
            QTableWidgetItem* nameItem = ui->tableWidgetContent->item(i, 1);
            assert(nameItem);
            nodes.insert(nameItem->data(RoleNodeId).toInt());
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
