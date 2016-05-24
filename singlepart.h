#ifndef SINGLEPART_H
#define SINGLEPART_H

#include <QStringList>
#include <part.h>

class SinglePart : public Part
{
public:
    explicit SinglePart(const QString & number,const QString & drawingNumber, const int & quantity = 1, const QString & material = QString(), const double &thickness = 0, const QString & filePath = QString(), QObject *parent = 0) :
        m_material(material), m_thickness(thickness), m_filePath(filePath), Part(drawingNumber,quantity,number) {}

    QString getMaterial() { return m_material; }
    void setMaterial(const QString & material) { m_material = material; }
    double getThickness() { return m_thickness; }
    void setThickness(const double & thickness) { m_thickness = thickness; }
    QString getFilePath() { return m_filePath; }
    void setFilePath(const QString & filePath) { m_filePath = filePath; }
    QStringList & getMachineList() { return m_machineList; }
    QStringList & getTechnologyList() { return m_technologyList; }
    void addMachine(const QString & machine) {
        m_machineList.push_back(machine);
        m_technologyList.push_back(defineTechnology(machine));
    }

signals:

public slots:

private:
    static QStringList cut2DList;
    QStringList m_technologyList;
    QString m_material;
    double m_thickness;
    QString m_filePath;
    QStringList m_machineList;
    static QString defineTechnology(QString machine);
};

#endif // SINGLEPART_H
