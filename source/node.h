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

    bool isValid() const;
    bool containsPrograms(const QSet<QString>&) const;
    bool containsFile(const QString&) const;

    float getFileWeighting(const QString&) const;

    float getFamiliarWithPrograms(const QSet<QString>&) const;

    void connectTo(const Node&);
    void disconnectFrom(const Node&);
    bool hasConnections() const;
    QSet<Node> getConnections() const;

    void reset();

    void setId(int);
    int getId() const;

    void setName(const QString&);
    QString getName() const;

    Node& setFA(const QString&, float weight = 1.0f);
    Node& setFA(const QMap<QString, float>&);
    QMap<QString, float> getFA() const;
    /*!
     * get FA as set
     */
    QSet<QString> getFASet() const;

    Node& setPA(const QString&, float = 1.0f);
    Node& setPA(const QMap<QString, float>&);
    QMap<QString, float> getPA() const;

    QString toString() const;

private:
    int mId;
    QString mName;
    QMap<QString, float> mFA;
    QMap<QString, float> mPA;
    QSet<Node> mConnecteds;
};

inline uint qHash(const Node &node)
{
    return qHash(node.getName());
}

#endif // NODE_H
