#ifndef FINDER_H
#define FINDER_H

#include <QObject>
#include <QtXlsx>
#include <QDir>
#include <QDirIterator>
#include <QThread>
#include <QtAlgorithms>
#include <QSet>
#include <QMetaType>
#include <QString>
#include <QFile>
#include <memory>
#include <utility>
#include <algorithm>
#include "singlepart.h"
#include "assembly.h"

class SinglePart;

class Finder : public QObject
{
    Q_OBJECT

public:
    explicit Finder(QObject *parent, const QString & schedulePath, const QString & searchedFolder = QString("//K1/Produkcja/TECHNOLODZY/BAZA DO TXT/txt/")) :
        QObject(parent), m_schedulePath(schedulePath), m_searchedFolder(searchedFolder)
    {
        m_abort = false;
        m_S235JRG2 = getItemsFromFile("S235JRG2.txt");
        m_S355J2G3 = getItemsFromFile("S355J2G3.txt");
        m_StainlessSteel = getItemsFromFile("StainlessSteel.txt");
        qDebug() << "S235JRG2.txt file: " << m_S235JRG2 << endl;
        qDebug() << "S355J2G3.txt file: " << m_S355J2G3 << endl;
        qDebug() << "StainlessSteel.txt file: " << m_StainlessSteel << endl;
    }

    void abort();
    QList<std::shared_ptr<Assembly>> & getPartList() { return m_partList; }
    QString getOrderNumber() const { return m_orderNumber; }
    QString getDeliveryDate() const { return m_deliveryDate; }
    QString getClient() const { return m_client; }

signals:
    void signalProgress(int, QString);
    void finished(bool,QString = "");
    void addItemToListWidget(QString, bool);

public slots:
    void loadFileList();

private:
    QXlsx::Document * m_schedule;
    QList<std::shared_ptr<Assembly>> m_partList;
    QString m_schedulePath;
    QString m_searchedFolder;
    bool m_abort;
    int m_lastRow;
    int m_lastColumn;
    QString m_orderNumber;
    QString m_deliveryDate;
    QString m_client;
    QStringList m_S235JRG2;
    QStringList m_S355J2G3;
    QStringList m_StainlessSteel;

    bool rowCount(int &lastRow);
    bool columnCount(int &lastColumn);
    bool checkSchedule();
    QString findFilePath(const QString &filename);
    void showPartList();
    QStringList getItemsFromFile(const QString &fileName);
    QString defineMaterial(const QString &material);
    //void sortPartList();

    QList<std::shared_ptr<Part> > makeAssemblyList(QString previousNumber, int &currentRow);
    std::shared_ptr<SinglePart> createSinglePart(int &currentRow);
    std::shared_ptr<Assembly> createAssembly(int &currentRow);
};

#endif // FINDER_H
