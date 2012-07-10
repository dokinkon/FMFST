#ifndef FMFST_H
#define FMFST_H

#include <QSet>
#include <QVector>
#include "graph.h"
#include "node.h"

class FMFST
{
public:
    void init(const QVector<Node>& nodeData);
    /*!
     *
     */
    void run(const QString& program, const QSet<QString>& fileSet);
private:

    void findMFST(int, const QSet<QString>&);

    void findMFST(const Graph&, const QSet<QString>&);

    QSet<int> findNodesWithProgram(const QString& program);

    QSet<Edge> getSpanningEdgesFromFST(const Graph&);

    bool isMFST(const Graph& fst);

    void dumpGraph(const Graph&);

    Graph mDCSGraph;
    QList<Graph> mFounds;
    Graph G;
    QVector<Node> mNodeData;
};


























































































#endif // FMFST_H
