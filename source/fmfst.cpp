#include "fmfst.h"
#include "node.h"
#include "pathweighteditor.h"
#include "nodeeditor.h"
#include "graphicsscene.h"

namespace {
// The Path Weight Sequence Is:
// 0:  X1,  X2
// 1:  X1,  X12
// 2:  X2,  X3
// 3:  X3,  X4
// 4:  X4,  X5
// 5:  X5,  X6
// 6:  X6,  X7
// 7:  X7,  X8
// 8:  X5,  X10
// 9:  X9,  X10
// 10: X8,  X9
// 11: X9,  X11
// 12: X11, X12
// 13: X12, X13
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
    {3, 11}
};

}

bool FMFST::isMFST(const Graph& fst)
{
    if (mFounds.isEmpty()) {
        return true;
    }

    foreach (const Graph& tree, mFounds) {
        if (fst.contains(tree)) {
            qDebug() << "=======================";
            qDebug() << "NOT MFST:";
            dumpGraph(fst);
            qDebug() << "=======================";
            return false;
        }
    }

    foreach (const Graph& tree, mFounds) {
        if (tree.contains(fst)) {
            mFounds.removeAll(tree);
            qDebug() << "=======================";
            qDebug() << "IS MFST:";
            dumpGraph(fst);
            qDebug() << "=======================";
            return true;
        }
    }
    return true;
}

QSet<Edge> FMFST::getSpanningEdgesFromFST(const Graph& fst)
{
    //qDebug() << "FIND SPANNING EDGES:";
    //qDebug() << "FST:" << fst.toString();
    //qDebug() << "G:" << G.toString();


    QSet<Edge> edges;
    foreach (const Edge& e, G.edges()) {
        int v1 = e.v1();
        int v2 = e.v2();

        if ( (fst.containsNode(v1) && !fst.containsNode(v2)) ||
             (fst.containsNode(v2) && !fst.containsNode(v1))) {
            edges.insert(e);
        }
    }
    //qDebug() << "spanningEdges:" << edges.size();
    return edges;
}

QSet<int> FMFST::findNodesWithProgram(const QString& program)
{
    QSet<int> nodes;
    foreach (const Node& n, mNodeData) {
        if (n.getPA().contains(program)) {
            nodes.insert(n.getId());
        }
    }
    return nodes;
}

void FMFST::findMFST(int nodeIndex, const QSet<QString>& fn, const QSet<int>& necessaryNodes)
{
    Graph fst;
    fst.insertNode(nodeIndex);
    findMFST(fst, fn, necessaryNodes);
}

void FMFST::findMFST(const Graph& fst, const QSet<QString>& fn, const QSet<int>& necessaryNodes)
{
    qDebug() << "-----------------------------------------------";
    qDebug() << "findMFST:" << ++mIterations; 
    dumpGraph(fst);
    qDebug() << "FN:" << fn;
    qDebug() << "-----------------------------------------------";

    if (fn.isEmpty() && necessaryNodes.isEmpty()) {
        if (isMFST(fst)) {
            mFounds.append(fst);
            qDebug() << "Add MFST:";
            dumpGraph(fst);
        }
        return;
    }

    QSet<Edge> T;
    QSet<Edge> spanningEdges = getSpanningEdgesFromFST(fst);

	// dump spanning edges
	qDebug() << "SPANNING EDGES:";
	foreach (const Edge& e, spanningEdges) {
        qDebug() << "X" << e.v1() + 1 << ", X" << e.v2() + 1;
	}

    foreach (const Edge& a, spanningEdges) {
        qDebug() << "CHECK EDGE: X" << a.v1() + 1 << ", X" << a.v2() +1;
        G.removeEdge(a);
        qDebug() << "G IS:";
        dumpGraph(G);


        int y;

        if (!fst.containsNode(a.v1())) {
            y = a.v1();
        } else {
            y = a.v2();
        }

        Graph extendFST(fst);
        extendFST.insertNode(y);
        extendFST.insertEdge(a);

        QSet<QString> newFn = fn;
        newFn -= mNodeData[y].getFA().toSet();

        qDebug() << "FA on node X" << y + 1 << " are" << mNodeData[y].getFA().toSet();

        // update necessayNodes
        QSet<int> nn = necessaryNodes;
        if (nn.contains(y)) {
            nn.remove(y);
        }

        findMFST(extendFST, newFn, nn);
        T.insert(a);
    }

    G.insertEdge(T);
}

void FMFST::dumpGraph(const Graph& g)
{
    QSet<int> nodes = g.nodes();
    qDebug() << "GRAPH\n";
    qDebug() << "V = {";
    foreach (int node, nodes) {
        qDebug() << mNodeData[node].getName();
    }
    qDebug() << "}\n";

    QSet<Edge> edges = g.edges();
    qDebug() << "E = {";
    foreach (const Edge& edge, edges) {
        qDebug() << mNodeData[edge.v1()].getName() << ", " << mNodeData[edge.v2()].getName();
    }
    qDebug() << "}\n";
}

float FMFST::getMinimumEdgeLength(const QString& program, const Graph& g)
{
    foreach (int n, findNodesWithProgram(program)) {
        if (g.containsNode(n)) {
            int familiar = mNodeData[n].getPA()[program];
            return (float)g.getTotalEdgeLength() / familiar;
        }
    }
    // something wrong...
    return g.getTotalEdgeLength();
}

static QString FormatResult(const QList<Graph>& trees, const QVector<Node>& nodes, const QSet<int>& starts, const QSet<int>& necessaryNodes)
{
    QString result;
    foreach (const Graph& tree, trees) {
        result.append("\n");

        int start = -1;
        foreach (int i, starts) {
            if (tree.containsNode(i)) {
                start = i;
                break;
            }
        }

        QSet<int> dumpped;

        if (start==-1) {
            result.append("起點:有誤\n");
        } else {
            result.append(QString::fromUtf8("起點：")).append(nodes[start].getName()).append("\n");
            dumpped.insert(start);
        }

        result.append(QString::fromUtf8("連接必經點:"));
        foreach (int i, necessaryNodes) {

            if (tree.containsNode(i)) {
                result.append(nodes[i].getName()).append(" ");
                dumpped.insert(i);
            }
        }
        result.append("\n");

        QSet<int> ns = tree.nodes();
        foreach (int i, dumpped) {
            ns.remove(i);
        }

        result.append(QString::fromUtf8("經過節點:"));
        foreach (int i, ns) {
            result.append(nodes[i].getName()).append(" ");
        }
        result.append("\n");
        result.append("=================\n");
    }

    return result;
}

QString FMFST::run(const QString& program, const QSet<QString>& fn, const QSet<int>& necessaryNodes)
{
    G = mDCSGraph;
    //qDebug() << "--------------------------------------------------";
    //qDebug() << "RUN!";
    //qDebug() << "G:" << G.toString();
    //qDebug() << "FN:" << FN;
    //qDebug() << "--------------------------------------------------";
	mIterations = 0;

    // remove the necessaryNodes that does not contain fn

    // 1. Get nodes that contain the program P
    QSet<int> starts = findNodesWithProgram(program);
    //qDebug() << "INIT NODES:" << nodes;
    foreach (int node, starts) {
        QSet<QString> fa = mNodeData[node].getFA().toSet();
        QSet<QString> newFn = fn;
        findMFST(node, newFn.subtract(fa), necessaryNodes);
        G.removeNode(node);
        qDebug() << "REMOVE NODE X" << node;
    }

    qDebug() << "RESULTS:";
    // Get Mimimum File Spanning Tree As Solution:
    Graph solution;
    float minLength = 100000000;
    foreach (const Graph& fst, mFounds) {
        dumpGraph(fst);
        qDebug() << "EDGE LENGTH:" << fst.getTotalEdgeLength();

        float len = getMinimumEdgeLength(program, fst);

        if (len<minLength) {
            minLength = len;
            solution = fst;
        }
    }
    // Update The Solution To GraphicsScene
    GetGraphicsScene().clearHighlight();
    foreach (int node, solution.nodes()) {
        GetGraphicsScene().highlightNode(mNodeData[node].getName());
    }
    foreach (const Edge& edge, solution.edges()) {
        GetGraphicsScene().highlightEdge(mNodeData[edge.v1()].getName(), mNodeData[edge.v2()].getName());
    }
    GetGraphicsScene().update();

    return FormatResult(mFounds, mNodeData, starts, necessaryNodes);
}

//---------------------------------------------
// The Path Weight Sequence Is:
// 0:  X1,  X2
// 1:  X1,  X12
// 2:  X2,  X3
// 3:  X3,  X4
// 4:  X4,  X5
// 5:  X5,  X6
// 6:  X6,  X7
// 7:  X7,  X8
// 8:  X5,  X10
// 9:  X9,  X10
// 10: X8,  X9
// 11: X9,  X11
// 12: X11, X12
// 13: X12, X13
void FMFST::init(const QVector<Node>& nodeData)
{
    mNodeData = nodeData;
    mDCSGraph.clear();
    mFounds.clear();
    QVector<float> lengths = GetPathWeightEditor().pathWeights();
    QVector<Node> nodes = GetNodeEditor().nodes();

    for (int i=0;i<15;++i) {
        mDCSGraph.insertEdge(edgeMap[i][0], edgeMap[i][1], lengths[i]);
    }

    G = mDCSGraph;
}




