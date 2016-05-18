#ifndef PART_H
#define PART_H

#include <QObject>

class Part : public QObject
{
    Q_OBJECT
public:
    explicit Part(const QString & drawingNumber, const int & quantity, const int & number, QObject *parent = 0) :
        m_drawingNumber(drawingNumber), m_quantity(quantity), m_number(number), QObject(parent){}
    Part(){}
    QString getDrawingNumber() { return m_drawingNumber; }
    int getQuantity() { return m_quantity; }
    int getNumber() { return m_number; }
    QString m_drawingNumber;
    int m_quantity;
    int m_number;

signals:

public slots:

};

#endif // PART_H
