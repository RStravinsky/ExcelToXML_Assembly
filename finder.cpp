#include "finder.h"


void Finder::abort()
{
    m_abort = true;
}

void Finder::showPartList()
{
    qDebug() << "------------------------PART LIST---------------------------";
    for(auto elem : m_partList) {
        SinglePart* singlePart = dynamic_cast<SinglePart*>(elem);
        if(singlePart!=NULL) {
            qDebug() << "M: " << singlePart->getNumber() << " - "
                              << singlePart->getDrawingNumber() << " - "
                              << singlePart->getQuantity() << " - "
                              << singlePart->getMaterial() << " - "
                              << singlePart->getThickness() << " - "
                              << singlePart->getFilePath();
            for(auto machine : singlePart->getMachineList()) { qDebug() << machine; }
        }
        else
            qDebug() << "Z: " << elem->getNumber() << elem->getDrawingNumber() << " - " << elem->getQuantity() << " - " << dynamic_cast<Assembly*>(elem)->getWeight() ;
    }
    qDebug() << "------------------------END PART LIST---------------------------";
}

//void Finder::sortPartList()
//{
//    qSort(m_partList.begin(), m_partList.end(),
//          [](SinglePart * part1, SinglePart * part2) {
//                return (part1->getDrawingNumber() < part2->getDrawingNumber());
//    });
//}

QStringList Finder::getItemsFromFile(QString fileName)
{
    QStringList list;
    QFile file(QDir::currentPath() + "\\" + fileName);
    if ( file.open(QIODevice::ReadWrite) ) {
        QString line;
        QTextStream t( &file );
        do {
            line = t.readLine();
            if(!line.isEmpty())
                list.append(line);
         } while (!line.isNull());
        file.close();
    }

    else {
        return QStringList();
    }

    return list;
}

QString Finder::defineMaterial(QString material)
{
    if(std::any_of(m_S235JRG2.begin(),m_S235JRG2.end(), [material](QString elem){ return material.contains(elem);}))
        return QString("S235JRG2");
    else if(std::any_of(m_S355J2G3.begin(),m_S355J2G3.end(), [material](QString elem){ return material.contains(elem);}))
        return QString("S355J2G3");
    else if(std::any_of(m_StainlessSteel.begin(),m_StainlessSteel.end(), [material](QString elem){ return material.contains(elem);}))
        return QString("Stainless Steel");

    return QString();
}

void Finder::loadFileList()
{
    emit signalProgress(100, "Wczytywanie harmonogramu ...");

    QXlsx::Document schedule(m_schedulePath);
    if(!checkSchedule(schedule)) {
        emit finished(false, "Harmonogram niepoprawnie sformatowany."); // failed
        return;
    }
    else {
        m_orderNumber = schedule.cellAt(1,9)->value().toString();
        m_deliveryDate = schedule.cellAt(3,4)->dateTime().toString("yyyyMMdd");
        m_client = schedule.cellAt(2,7)->value().toString();
        qDebug() << m_orderNumber << m_deliveryDate << m_client << endl;
    }

    // find last column of scheudle
    int lastColumn = 0;
    if(!columnCount(schedule,lastColumn)) {
        emit finished(false, "Harmonogram niepoprawnie sformatowany."); // failed
        return;
    }

    // find last row of schedule
    int lastRow = 0;
    if(!rowCount(schedule,lastRow)) {
        emit finished(false, "Harmonogram niepoprawnie sformatowany."); // failed
        return;
    }

    emit signalProgress(0, "Tworzenie listy części ...");
    for (int row = 7; row <= lastRow; ++row)
    {
        bool abort = m_abort;
        if (abort) {
            emit finished(false); // break
            return;
        }


        QString drawingNumber = schedule.cellAt(row, 3)->value().toString();
        if(drawingNumber.isEmpty()){
            emit finished(false,"Brak nazwy rysunku.");
            return;
        }

        QString dxfPath = findFilePath(schedule.cellAt(row, 3)->value().toString());
        QString material =  defineMaterial(schedule.cellAt(row, 8)->value().toString());
        if(material.isEmpty()) {
            emit finished(false,"Nie dopasowano gatunku dla rysunku: "+schedule.cellAt(row, 3)->value().toString()+"");
            return;
        }

        int number = schedule.cellAt(row, 2)->value().toString().replace(".","").toInt();
//        if(number.isEmpty()){
//            emit finished(false,"Brak liczby porządkowej dla rysunku: "+schedule.cellAt(row, 3)->value().toString()+"");
//            return;
//        }

        QString partType = schedule.cellAt(row, 1)->value().toString();
        if(partType.isEmpty()){
            emit finished(false,"Brak rodzaju dla rysunku: "+schedule.cellAt(row, 3)->value().toString()+"");
            return;
        }
        if(partType == "Z")
            m_partList.push_back(new Assembly(number, drawingNumber, 230.30, schedule.cellAt(row, 5)->value().toInt()));
        else if(partType == "M") {
            SinglePart * sp = new SinglePart(number, drawingNumber, material, schedule.cellAt(row, 11)->value().toDouble(), schedule.cellAt(row, 5)->value().toInt(), dxfPath);

            for(int i = 12; i <= lastColumn; ++i) {
                if(schedule.cellAt(row, i)->value().toString().contains("X", Qt::CaseInsensitive)) {
                    sp->addMachine(schedule.cellAt(6, i)->value().toString());
                }
            }
            if(sp->getMachineList().isEmpty()){
                emit finished(false,"Nie dopasowano maszyny dla rysunku: "+schedule.cellAt(row, 3)->value().toString()+"");
                return;
            }
            m_partList.push_back(sp); // TODO: Add thickness reading !!

        }

        emit addItemToListWidget(schedule.cellAt(row, 3)->value().toString(), !dxfPath.isEmpty());
        emit signalProgress(int((double(row)/double(lastRow)*100))+1, "Tworzenie listy części ...");
    }

    //sortPartList();
    showPartList();
    emit finished(true); // success
}

bool Finder::rowCount(QXlsx::Document &schedule, int & lastRow)
{
    for (int row = 7; row < 65536; ++row)
    {
        bool abort = m_abort;
        if (abort) {
            emit finished(false);
            return false;
        }

        if(QXlsx::Cell *cell=schedule.cellAt(row, 6))
        {
            if(cell->value() == QVariant("Masa")) {
                lastRow = row - 1;
                break;
            }
        }
    }

    if(lastRow == 0)
        return false;

    return true;
}

bool Finder::columnCount(QXlsx::Document &schedule, int &lastColumn)
{
    for(int i = 1; i < 65536; ++i) {
        qDebug() << schedule.cellAt(6,i)->value().toString() << endl;
         if(schedule.cellAt(6,i)->value().toString().contains("Koniec")) {
            lastColumn = i - 1;
            break;
        }
    }

    if(lastColumn == 0)
        return false;

    return true;
}

QString Finder::findFilePath(const QString & filename)
{
    QDir dir(m_searchedFolder, QString("*.dxf"), QDir::NoSort, QDir::Files | QDir::NoSymLinks);
    QDirIterator counterIt(dir, QDirIterator::Subdirectories);
    filesCounter = 0;
    while (counterIt.hasNext()) {

            bool abort = m_abort;
            if (abort) {
                emit finished(false);
                break;
            }

            if(counterIt.fileName().contains(filename, Qt::CaseInsensitive)) {
                ++filesCounter;
                return counterIt.filePath();
            }
            counterIt.next();
    }
    if(filesCounter == 0)
        return QString("");

    return QString("");
}

bool Finder::checkSchedule(QXlsx::Document &schedule)
{
    QString orderDigit = schedule.cellAt(6, 2)->value().toString();
    QString drawingNr = schedule.cellAt(6, 3)->value().toString();
    QString cooperator = schedule.cellAt(6, 10)->value().toString();
    QString thickness = schedule.cellAt(6,11)->value().toString();

    if( orderDigit.contains("L.p.") && drawingNr.contains("Nr rys.") && cooperator.contains("Kooperant") && thickness.contains("Grubość") )
        return true;
    else
        return false;
}

