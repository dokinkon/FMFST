#ifndef GRAPH_H
#define GRAPH_H

#include <QSet>
#include "edge.h"

class Node;

/*!
 * The Node has index from 0 to N
 * The Edge has node index reference
 */
class Graph
{
public:

    Graph();

    Graph(const Graph& other);

    void clear();

    bool operator==(const Graph& rhs) const;

    QSet<int> nodes() const;

    QSet<Edge> edges() const;

    bool containsNode(int n) const;

    bool containsNode(const QSet<int>& nodes) const;

    bool containsEdge(const Edge& e) const;

    bool containsEdge(const QSet<Edge>& edges) const;

    bool contains(const Graph& g) const;

    void insertNode(int);

    void removeNode(int);

    void insertEdge(int v1, int v2, float length = 0);

    void insertEdge(const Edge& e);

    void insertEdge(const QSet<Edge>& edges);

    void removeEdge(const Edge& e);

    float getTotalEdgeLength() const;

    QString toString() const;

private:
    QSet<int> mNodes;
    QSet<Edge> mEdges;

};

#endif // GRAPH_H
