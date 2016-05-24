#ifndef SINGLEPART_H
#define SINGLEPART_H

#include <QStringList>
#include <part.h>

class SinglePart : public Part
{
public:
    explicit SinglePart(const QString & number,const QString & drawingNumber, const int & quantity = 1, const QString & material = QString(), const double &thickness = 0, const QString & filePath = QString(), QObject *parent = 0) :
        m_material(material), m_thickness(thickness), m_filePath(filePath), Part(drawingNumber,quantity,number) {}

    QString getMaterial() const { return m_material; }
    double getThickness() const { return m_thickness; }
    QString getFilePath() const { return m_filePath; }
    QStringList & getMachineList() { return m_machineList; }
    QStringList & getTechnologyList() { return m_technologyList; }

    void setMaterial(const QString & material) { m_material = material; }
    void setThickness(const double & thickness) { m_thickness = thickness; }
    void setFilePath(const QString & filePath) { m_filePath = filePath; }
    void addMachine(const QString & machine) {
        m_machineList.push_back(machine);
        m_technologyList.push_back(defineTechnology(machine));
    }

private:
    static QStringList cut2DList;
    QStringList m_technologyList;
    QString m_material;
    double m_thickness;
    QString m_filePath;
    QStringList m_machineList;
    static QString defineTechnology(const QString & machine);
};

#endif // SINGLEPART_H
