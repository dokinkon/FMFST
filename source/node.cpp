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

float Node::getFamiliarWithPrograms(const QSet<QString>& programs) const
{
    float f = 0;

    foreach (const QString& program, programs) {
        if (mPA.contains(program)) {
            f += mPA[program];
        }
    }
    return f;
}

float Node::getFileWeighting(const QString& file) const
{
    if (mFA.contains(file)) {
        return mFA[file];
    }
    return 0.0f;
}

bool Node::containsFile(const QString& file) const
{
    return mFA.contains(file);
}

bool Node::containsPrograms(const QSet<QString>& programs) const
{
    foreach (const QString& program, programs) {
        if (!mPA.contains(program))
            return false;
    }
    return true;
}

bool Node::operator==(const Node& rhs) const
{
    if (mId!=rhs.mId)
        return false;

    if (mName!=rhs.mName)
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

Node& Node::setFA(const QString& name, float weight)
{
	mFA[name] = weight;
    return *this;
}

Node& Node::setFA(const QMap<QString, float>& fa)
{
    mFA = fa;
    return *this;
}

QMap<QString, float> Node::getFA() const
{
    return mFA;
}

QSet<QString> Node::getFASet() const
{
    QSet<QString> fas;
    foreach (const QString& fa, mFA.keys()) {
        fas.insert(fa);
    }
    return fas;
}

Node& Node::setPA(const QString& pa, float familiar)
{
    mPA[pa] = familiar;
    return *this;
}

Node& Node::setPA(const QMap<QString, float>& pa)
{
    mPA = pa;
    mPA.remove("");
    return *this;
}

QMap<QString, float> Node::getPA() const
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

