#ifndef FMFST_H
#define FMFST_H

#include <QSet>
#include <QVector>
#include "graph.h"
#include "node.h"

class FMFST
{
public:
    FMFST();
    ~FMFST();
    /*!
     * DEPRECATED
     */
    void init(const QVector<Node>& nodeData);
    /*!
     *
     */
    QString run(const QString& program, const QSet<QString>& fileSet, const QSet<int>& necessaryNodes);
private:

    struct Private;
    Private* mPrivate;

    void findMFST(int, const QSet<QString>&, const QSet<int>& necessaryNodes);

    void findMFST(const Graph&, const QSet<QString>& fn, const QSet<int>& necessaryNodes);

    QSet<int> findNodesWithProgram(const QString& program);

    QSet<Edge> getSpanningEdgesFromFST(const Graph&);

    bool isMFST(const Graph& fst);

    void dumpGraph(const Graph&);

    float getMinimumEdgeLength(const QString&, const Graph&);

    Graph mDCSGraph;
    QList<Graph> mFounds;
    Graph G;
    int mIterations;
};


























































































#endif // FMFST_H
