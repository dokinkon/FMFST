#include "edge.h"
#include "node.h"

Edge::Edge()
    : mV1(-1)
    , mV2(-1)
    , mTimeWeight(1)
    , mReliability(1)
{
}

Edge::Edge(int v1, int v2, float timeWeight, float reliablity)
    : mV1(v1)
    , mV2(v2)
    , mTimeWeight(timeWeight)
    , mReliability(reliablity)
{
}

Edge::Edge(const Edge& other)
    : mV1(other.mV1)
    , mV2(other.mV2)
    , mReliability(other.mReliability)
    , mTimeWeight(other.mTimeWeight)
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
    return mTimeWeight / mReliability;
}

Edge& Edge::operator=(const Edge& rhs)
{
    mV1 = rhs.mV1;
    mV2 = rhs.mV2;
    mTimeWeight = rhs.mTimeWeight;
    mReliability = rhs.mReliability;
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

void Edge::setTimeWeight(float w)
{
    mTimeWeight = w;
}

void Edge::setReliability(float r)
{
    mReliability = r;
}

float Edge::getTimeWeight() const
{
    return mTimeWeight;
}

float Edge::getReliability() const
{
    return mReliability;
}









