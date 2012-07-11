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

void FMFST::findMFST(int nodeIndex, const QSet<QString>& fn)
{
    Graph fst;
    fst.insertNode(nodeIndex);
    findMFST(fst, fn);
}

void FMFST::findMFST(const Graph& fst, const QSet<QString>& fn)
{
    qDebug() << "-----------------------------------------------";
    qDebug() << "findMFST:" << ++mIterations; 
    dumpGraph(fst);
    qDebug() << "FN:" << fn;
    qDebug() << "-----------------------------------------------";

    if (fn.isEmpty()) {
        if (isMFST(fst)) {
            mFounds.append(fst);
            qDebug() << "Add MFST:";// << fst.toString();
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

        findMFST(extendFST, newFn);
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

void FMFST::run(const QString& program, const QSet<QString>& fn)
{
    G = mDCSGraph;
    //qDebug() << "--------------------------------------------------";
    //qDebug() << "RUN!";
    //qDebug() << "G:" << G.toString();
    //qDebug() << "FN:" << FN;
    //qDebug() << "--------------------------------------------------";
	mIterations = 0;

    // 1. Get nodes that contain the program P
    QSet<int> nodes = findNodesWithProgram(program);
    qDebug() << "INIT NODES:" << nodes;
    foreach (int node, nodes) {
        QSet<QString> fa = mNodeData[node].getFA().toSet();
        QSet<QString> newFn = fn;
        findMFST(node, newFn.subtract(fa));
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

        if (fst.getTotalEdgeLength()<minLength) {
            minLength = fst.getTotalEdgeLength();
            solution = fst;
        }
    }

    // Update The Solution To GraphicsScene
    GetGraphicsScene().clearHighlight();
    foreach (int node, solution.nodes()) {
        GetGraphicsScene().highlightNode(mNodeData[node].getName());
    }
    //foreach (const DCSEdge& edge, solution.edges()) {
        //GetGraphicsScene().highlightEdge(edge.v1().getName(), edge.v2().getName());
    //}
    GetGraphicsScene().update();
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

/*
Graph<Node>& GetSharedDCSGraph() {
    static DCSGraph sharedGraph;
    return sharedGraph;
}
*/



