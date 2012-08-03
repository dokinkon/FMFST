#include "graph.h"
#include "edge.h"
#include "node.h"

Graph::Graph()
{
}

Graph::Graph(const Graph& other)
    : mNodes(other.mNodes)
    , mEdges(other.mEdges)
{
}

void Graph::clear()
{
    mNodes.clear();
    mEdges.clear();
}

bool Graph::operator ==(const Graph& rhs) const
{
    if ( mNodes != rhs.mNodes )
        return false;
    if ( mEdges != rhs.mEdges )
        return false;
    return true;
}

QSet<int> Graph::nodes() const
{
    return mNodes;
}

QSet<Edge> Graph::edges() const
{
    return mEdges;
}

bool Graph::containsNode(int n) const
{
    return mNodes.contains(n);
}

bool Graph::containsNode(const QSet<int>& nodes) const
{
    return mNodes.contains(nodes);
}

bool Graph::containsEdge(const Edge& e) const
{
    return mEdges.contains(e);
}

bool Graph::containsEdge(const QSet<Edge>& edges) const
{
    return mEdges.contains(edges);
}

bool Graph::contains(const Graph& g) const
{
    if (!containsNode(g.nodes()))
        return false;
    if (!containsEdge(g.edges()))
        return false;
    return true;
}

void Graph::insertNode(int node)
{
    mNodes.insert(node);
}

void Graph::removeNode(int node)
{
    QSet<Edge>::Iterator it = mEdges.begin();
    while (it!=mEdges.end()) {
        const Edge& edge = *it;
        if (edge.v1()==node || edge.v2()==node) {
            it = mEdges.erase(it);
        } else {
            ++it;
        }
    }
    mNodes.remove(node);
}

void Graph::insertEdge(int v1, int v2, float length)
{
    insertNode(v1);
    insertNode(v2);
    mEdges.insert(Edge(v1, v2, length, 1));
}

void Graph::insertEdge(const Edge& e)
{
    insertEdge(e.v1(), e.v2(), e.length());
}

void Graph::insertEdge(const QSet<Edge>& edges)
{
    foreach (const Edge& edge, edges) {
        insertEdge(edge);
    }
}

void Graph::removeEdge(const Edge& e)
{
    mEdges.remove(e);
}

float Graph::getTotalEdgeLength() const
{
    float w = 0;
    foreach (Edge e, mEdges) {
        w += e.length();
    }
    return w;
}

QString Graph::toString() const
{
    QString result = "GRAPH:\n";
    result.append("V = {");
    for (int i=0;i<mNodes.size();++i) {
        if (i!=0) {
            result.append(",");
        }
        //result.append("V[").append(QString::number(mNodes[i]).append("]");
    }
    result.append("}\n");
    return result;
}
