#ifndef ASSEMBLY_H
#define ASSEMBLY_H

#include <QtAlgorithms>
#include <memory>
#include <part.h>

class Assembly : public Part
{
public:
    Assembly(const QString & number, const QString & drawingNumber, const int & quantity, const float & weight) :
        Part(drawingNumber,quantity, number), m_weight(weight) {}

    float getWeight() const { return m_weight; }
    void setSubPartList(const QList<std::shared_ptr<Part>> &list) { m_subpartsList = list; }
    QList<std::shared_ptr<Part>> & getSubpartList() { return m_subpartsList; }

private:
    float m_weight;
    QList<std::shared_ptr<Part>> m_subpartsList;
};

#endif // ASSEMBLY_H
