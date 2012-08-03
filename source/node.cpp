#include "node.h"

Node::Node()
    : mId(-1)
{
}

Node::Node(const QString& name)
    : mId(-1)
    , mName(name)
{

}

Node::Node(const Node& rhs)
    : mId(rhs.mId)
    , mName(rhs.mName)
    , mFA(rhs.mFA)
    , mPA(rhs.mPA)
    , mConnecteds(rhs.mConnecteds)
{
}

bool Node::isValid() const
{
    return mId >= 0;
}

bool Node::operator==(const Node& rhs) const
{
    if (mId!=rhs.mId)
        return false;

    if (mName!=rhs.mName)
        return false;

    if (mFA!=rhs.mFA)
        return false;

    if (mPA!=rhs.mPA)
        return false;

    return true;
}

void Node::reset()
{
    mId = -1;
    mName = "";
    mFA.clear();
    mPA.clear();
}

Node& Node::operator=(const Node& rhs)
{
    if (this==&rhs)
        return *this;

    this->mId   = rhs.mId;
    this->mName = rhs.mName;
    this->mFA   = rhs.mFA;
    this->mPA   = rhs.mPA;
    this->mConnecteds = rhs.mConnecteds;
    return *this;
}

void Node::setId(int id)
{
    mId = id;
}

int Node::getId() const
{
    return mId;
}

void Node::setName(const QString& name)
{
    mName = name;
}

QString Node::getName() const
{
    return mName;
}

Node& Node::setFA(const QString& fa)
{
	mFA.clear();
    mFA.append(fa);
    mFA.removeDuplicates();
    return *this;
}

Node& Node::setFA(const QStringList& fa)
{
	mFA.clear();
    mFA.append(fa);
    mFA.removeDuplicates();
    return *this;
}

QStringList Node::getFA() const
{
    return mFA;
}

Node& Node::setPA(const QString& pa, int familiar)
{
    mPA[pa] = familiar;
    return *this;
}

Node& Node::setPA(const QMap<QString, int>& pa)
{
    mPA = pa;
    mPA.remove("");
    return *this;
}

QMap<QString, int> Node::getPA() const
{
    return mPA;
}

QString Node::toString() const
{
    return QString("V(%1)").arg(mName);
}

void Node::connectTo(const Node& n)
{
    if (*this == n)
        return;
    if (mConnecteds.contains(n))
        return;

    mConnecteds.insert(n);
}

void Node::disconnectFrom(const Node& n)
{
    if (!mConnecteds.contains(n))
        return;
    mConnecteds.remove(n);
   // n.disconnectFrom(*this);
}

QSet<Node> Node::getConnections() const
{
    return mConnecteds;
}

bool Node::hasConnections() const
{
    return mConnecteds.size() > 0;
}

