#ifndef EDGE_H
#define EDGE_H

#include <QString>
#include <QHash>

class Edge
{
public:
    Edge();

    Edge(int, int, float);

    Edge(const Edge& other);

    int v1() const;

    int v2() const;

    float length() const;

    Edge& operator=(const Edge& rhs);

    bool operator==(const Edge& rhs) const;

    QString toString() const;

private:
    int mV1;
    int mV2;
    float mLength;
};

inline uint qHash(const Edge &e)
{
    return qHash(e.v1()) ^ qHash(e.v2());
}

#endif // EDGE_H
