#include "nodeeditor.h"
#include "node.h"
#include "graphicsscene.h"
#include "pathweighteditor.h"
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
    bool serializeToText();
    bool deserializeFromText();
    bool deserialize();
    int generateId() const;

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

QString invParseFPA(const QMap<QString, float>& pa)
{
    QString s;
    QMap<QString, float>::ConstIterator it = pa.begin();
    for (;it!=pa.end();++it) {
        s.append(",").append(it.key()).append("(").append(QString::number(it.value(), 'f', 1)).append(")");
    }
    return s;
}

QMap<QString, float> parsePA(const QString& s)
{
    QMap<QString, float> r;
    QStringList tokens = s.split(",");
    foreach (const QString& token, tokens) {
        if (token.isNull() || token.isEmpty())
            continue;
        if (token.contains("(") && token.contains(")")) {
            int start = token.indexOf("(");
            int end = token.indexOf(")");
            QString programName = token.left(start);
            QString mid = token.mid(start + 1, end - start - 1);
            float familiar = mid.toFloat();
            r[programName] = familiar;
        } else {
            r[token] = 1.0f;
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
        item->setText(invParseFPA(node.getPA()));
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
            node.setFA(parsePA(item->text()));
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
}

bool NodeEditor::Private::serializeToText()
{
    if (!mSerializeEnabled)
        return false;

    QFile f("node.dat");
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "FAILED TO OPEN node.dat";
        return false;
    }

    QTextStream ts(&f);
    
    foreach (const Node& node, mNodes) {

        QMap<QString, float> fa = node.getFA();
        QMap<QString, float> pa = node.getPA();

        ts << "NODE.ID=" << node.getId() << "\n";
        ts << "NODE.NAME=" << node.getName() << "\n";
        ts << "NODE.FA.COUNT=" << fa.size() << "\n";
        QMap<QString, float>::ConstIterator it = fa.begin();
        for (;it!=fa.end();++it) {
            ts << it.key() << "=" << it.value() << "\n";
        }

        ts << "NODE.PA.COUNT=" << pa.size() << "\n";
        it = pa.begin();
        for (;it!=pa.end();++it) {
            ts << it.key() << "=" << it.value() << "\n";
        }
    }

    return true;
}
bool NodeEditor::Private::deserializeFromText()
{
    QFile f("node.dat");
    if (!f.open(QIODevice::ReadOnly)) {
        qDebug() << "FAILED TO OPEN \"node.dat\"";
        return false;
    }

    mNodes.clear();

    QTextStream ts(&f);
    while (!ts.atEnd()) {
        QString line = ts.readLine();
        QString nodeId = line.replace("NODE.ID=", "");

        line = ts.readLine();
        QString nodeName = line.replace("NODE.NAME=","");

        line = ts.readLine();
        int faCount = line.replace("NODE.FA.COUNT", "").toInt();

        QMap<QString, float> fa, pa;
        for (int i=0;i<faCount;i++) {
            line = ts.readLine();
            QString faName = line.section("=", 0, 0);
            float faValue  = line.section("=", 1, 1).toFloat();
            fa[faName] = faValue;
        }

        line = ts.readLine();
        int paCount = line.replace("NODE.PA.COUNT", "").toInt();
        for (int i=0;i<paCount;i++) {
            line = ts.readLine();
            QString paName = line.section("=", 0, 0);
            float paValue  = line.section("=", 1, 1);
            pa[paName] = paValue;
        }

        Node node;
        node.setId(nodeId);
        node.setName(nodeName);
        node.setFA(fa);
        node.setPA(pa);
        mNodes.append(node);
    }
    qDebug() << "[NODE-EDITOR]: deserialize node.dat ok";
    return true;
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
    }
    f.close();
    qDebug() << "[NODE-EDITOR]: serialize node.dat ok";
    return true;
}



bool NodeEditor::Private::deserialize()
{
    QFile f("node.dat");
    if (!f.open(QIODevice::ReadOnly)) {
        qDebug() << "FAILED TO OPEN \"node.dat\"";
        return false;
    }

    mNodes.clear();

    QDataStream ds(&f);
    while (!ds.atEnd()) {
        int id;
        QString name;
        QMap<QString, float> fa;
        QMap<QString, float> pa;
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
    SharedInstance = this;
    mPrivate->mSerializeEnabled = true;
}

NodeEditor::~NodeEditor()
{
    delete ui;
    delete mPrivate;
    SharedInstance = NULL;
}

void NodeEditor::initialize()
{
    mPrivate->initContentWidget(ui->tableWidgetContent);
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
    node.setFA(parsePA(fa));
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
    GetPathWeightEditor().updateNodeData();
    GetGraphicsScene().updateNodeData();
    GetGraphicsScene().update();
}

bool NodeEditor::hasProgram(const QString& program) const
{
    foreach (const Node& node, mPrivate->mNodes) {
        QMap<QString, float> pa = node.getPA();
        if (pa.contains(program))
            return true;
    }
    return false;
}

bool NodeEditor::hasProgram(const QSet<QString>& programs) const
{
    foreach (const Node& node, mPrivate->mNodes) {
        QMap<QString, float> pa = node.getPA();

        bool found = true;
        foreach (const QString& program, programs) {
            if (!pa.contains(program)) {
                found = false;
                break;
            }
        }

        if (found)
            return true;
    }
    return false;
}

bool NodeEditor::hasFile(const QString& file) const
{
    foreach (const Node& node, mPrivate->mNodes) {
        QMap<QString, float> fa = node.getFA();
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

NodeEditor& GetNodeEditor()
{
    return *SharedInstance;
}

NodeEditor* GetNodeEditorPtr()
{
    return SharedInstance;
}
