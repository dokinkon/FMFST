#include "edge.h"
#include "node.h"

Edge::Edge()
{
}

Edge::Edge(int v1, int v2, float length)
    : mV1(v1)
    , mV2(v2)
    , mLength(length)
{
}

Edge::Edge(const Edge& other)
    : mV1(other.mV1)
    , mV2(other.mV2)
    , mLength(other.mLength)
{

}

int Edge::v1() const
{
    return mV1;
}

int Edge::v2() const
{
    return mV2;
}

float Edge::length() const
{
    return mLength;
}

Edge& Edge::operator=(const Edge& rhs)
{
    mV1 = rhs.mV1;
    mV2 = rhs.mV2;
    mLength = rhs.mLength;
    return *this;
}

bool Edge::operator==(const Edge& rhs) const
{
    if ( mV1 == rhs.mV1 && mV2 == rhs.mV2 )
        return true;

    if ( mV1 == rhs.mV2 && mV2 == rhs.mV1 )
        return true;

    return false;
}

QString Edge::toString() const
{
    return QString();
    //return QString("E[%1,%2]").arg(mV1.toString()).arg(mV2.toString());
}
