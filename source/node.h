#ifndef NODE_H
#define NODE_H

#include <QtCore>



class Node
{
public:
    Node();
    Node(const QString&);
    Node(const Node&);
    Node& operator=(const Node&);
    bool operator==(const Node&) const;

    void connectTo(const Node&);
    void disconnectFrom(const Node&);
    bool hasConnections() const;
    QSet<Node> getConnections() const;

    void reset();

    void setId(int);
    int getId() const;

    void setName(const QString&);
    QString getName() const;

    Node& setFA(const QString&);
    Node& setFA(const QStringList&);
    QStringList getFA() const;

    Node& setPA(const QString&);
    Node& setPA(const QStringList&);
    QStringList getPA() const;

    QString toString() const;

private:
    int mId;
    QString mName;
    QStringList mFA;
    QStringList mPA;

    QSet<Node> mConnecteds;
};

inline uint qHash(const Node &node)
{
    return qHash(node.getName());
}

#endif // NODE_H
