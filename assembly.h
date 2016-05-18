#ifndef ASSEMBLY_H
#define ASSEMBLY_H

#include <QtAlgorithms>
#include <part.h>

class Assembly : public Part
{
public:
    Assembly(const int & number, const QString & drawingNumber, const float & weight, const int & quantity) :
        m_weight(weight), Part(drawingNumber,quantity, number) {}

    float getWeight() { return m_weight; }
private:
    float m_weight;
};

#endif // ASSEMBLY_H
