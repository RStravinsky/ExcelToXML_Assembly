#ifndef PART_H
#define PART_H

#include <QObject>

class Part : public QObject
{
    Q_OBJECT
public:
    explicit Part(const QString & drawingNumber, const int & quantity, const QString & number, QObject *parent = 0) :
        m_drawingNumber(drawingNumber), m_quantity(quantity), m_number(number), QObject(parent){}
    Part(){}
    QString getDrawingNumber() const { return m_drawingNumber; }
    int getQuantity() const { return m_quantity; }
    QString getNumber() const { return m_number; }

private:
    QString m_drawingNumber;
    int m_quantity;
    QString m_number;
};

#endif // PART_H
