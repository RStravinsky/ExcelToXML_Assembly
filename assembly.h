#ifndef ASSEMBLY_H
#define ASSEMBLY_H

#include <QtAlgorithms>
#include <part.h>

class Assembly : public Part
{
public:
    Assembly(const QString & number, const QString & drawingNumber, const int & quantity, const float & weight) :
        Part(drawingNumber,quantity, number),m_weight(weight) {}

    float getWeight() { return m_weight; }
    void addSubPart(Part * part) {
        m_subpartsList.push_back(part);
    }

    void setSubPartList(QList<Part*> list) {
        m_subpartsList = list;
    }

    QList<Part*> getSubpartList() { return m_subpartsList; }


private:
    float m_weight;
    QList<Part*> m_subpartsList;
};

#endif // ASSEMBLY_H
