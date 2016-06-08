#include "finder.h"


void Finder::abort()
{
    m_abort = true;
}

void Finder::showPartList()
{
    qDebug() << "------------------------PART LIST---------------------------";
    for(auto elem : m_partList) {
        std::shared_ptr<SinglePart> singlePart = std::dynamic_pointer_cast<SinglePart>(elem);
        if(singlePart!=nullptr) {
            qDebug() << "M: " << singlePart->getNumber() << " - "
                              << singlePart->getDrawingNumber() << " - "
                              << singlePart->getQuantity() << " - "
                              << singlePart->getMaterial() << " - "
                              << singlePart->getThickness() << " - "
                              << singlePart->getFilePath();
            for(auto machine : singlePart->getMachineList()) { qDebug() << machine; }
        }
        else
            qDebug() << "Z: " << elem->getNumber() << elem->getDrawingNumber() << " - " << elem->getQuantity() << " - " << std::dynamic_pointer_cast<Assembly>(elem)->getWeight() ;
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

QStringList Finder::getItemsFromFile(const QString &fileName)
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

QString Finder::defineMaterial(const QString & material)
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

    m_schedule = new QXlsx::Document(m_schedulePath);
    if(!checkSchedule()) {
        emit finished(false, "Harmonogram niepoprawnie sformatowany.");
        return;
    }
    else {
        m_orderNumber = m_schedule->cellAt(1,9)->value().toString();
        m_deliveryDate = m_schedule->cellAt(3,4)->dateTime().toString("yyyyMMdd");
        m_client = m_schedule->cellAt(2,7)->value().toString();
        qDebug() << m_orderNumber << m_deliveryDate << m_client << endl;
    }

    // find last row and column of scheudle
    if(!(columnCount(m_lastColumn) && rowCount(m_lastRow))) {
        emit finished(false, "Harmonogram niepoprawnie sformatowany."); // failed
        return;
    }

    emit signalProgress(0, "Tworzenie listy części ...");
    int currentRow = 7;
    while(currentRow <= m_lastRow) { // test all rows

        if (m_abort) {
            emit finished(false);
            return;
        }

        QString partType = m_schedule->cellAt(currentRow, 1)->value().toString();
        if(!partType.isEmpty() && partType == "Z") {

            std::shared_ptr<Assembly> assembly(std::move(createAssembly(currentRow)));
            if(assembly==nullptr)
                return;
            m_partList.push_back(std::move(assembly));

        }
        else if(!partType.isEmpty() && partType == "M") {

            std::shared_ptr<SinglePart> part(std::move(createSinglePart(currentRow)));
            if(part==nullptr)
                return;
            m_partList.push_back(std::move(part));
            ++currentRow;

        }
        else {
            emit finished(false,"Rysunek w wierszu: " + QString::number(currentRow) + " nie jest złożeniem.");
            return;
        }
    }

    showPartList();
    emit finished(true); // success
}

QList<std::shared_ptr<Part>> Finder::makeAssemblyList(QString previousNumber, int &currentRow)
{
    QList<std::shared_ptr<Part>> partList;
    QString number = m_schedule->cellAt(currentRow, 2)->value().toString().replace(".","");

    while( number.left(previousNumber.length()) == previousNumber ) {

        if (m_abort) {
            emit finished(false);
            break;
        }

        QString partType = m_schedule->cellAt(currentRow, 1)->value().toString();
        if(partType == "Z") {
            std::shared_ptr<Assembly> assembly(std::move(createAssembly(currentRow)));
            if(assembly==nullptr) {
                abort();
                break;
            }
            partList.push_back(std::move(assembly));
        }

        else if(partType == "M") {
            std::shared_ptr<SinglePart> singlePart(std::move(createSinglePart(currentRow)));
            if(singlePart==nullptr) {
                abort();
                break;
            }
            partList.push_back(std::move(singlePart));
            ++currentRow;
        }

        emit signalProgress(int((double(currentRow)/double(m_lastRow)*100))+1, "Tworzenie listy części ..."); // update progress bar
        number = m_schedule->cellAt(currentRow, 2)->value().toString().replace(".","");
    }

    return partList;
}

std::shared_ptr<Assembly> Finder::createAssembly(int &currentRow)
{
    QString number = m_schedule->cellAt(currentRow, 2)->value().toString().replace(".","");

    QString assemblyName = m_schedule->cellAt(currentRow, 4)->value().toString();
    if(assemblyName.isEmpty()){
        emit finished(false,"Brak nazwy zespołu w wierszu " + QString::number(currentRow) + ".");
        return nullptr;
    }

    QString drawingNumber = m_schedule->cellAt(currentRow, 3)->value().toString();
    if(drawingNumber.isEmpty()){
        emit finished(false,"Brak nazwy rysunku w wierszu " + QString::number(currentRow) + ".");
        return nullptr;
    }

    int quantity = m_schedule->cellAt(currentRow, 5)->value().toInt();
    if(quantity == 0){
        emit finished(false,"Brak ilości dla rysunku: "+ drawingNumber + "");
        return nullptr;
    }

    float weight = m_schedule->cellAt(currentRow, 7)->value().toFloat();
    if(weight == 0){
        emit finished(false,"Brak wagi dla rysunku: "+ drawingNumber + "");
        return nullptr;
    }

    std::shared_ptr<Assembly> assembly = std::make_shared<Assembly>(number, drawingNumber, assemblyName, quantity, weight);
    assembly->setSubPartList(makeAssemblyList(number,++currentRow));

    return assembly;
}

std::shared_ptr<SinglePart> Finder::createSinglePart(int &currentRow)
{
    QString number = m_schedule->cellAt(currentRow, 2)->value().toString().replace(".","");

    QString drawingNumber = m_schedule->cellAt(currentRow, 3)->value().toString();
    if(drawingNumber.isEmpty()){
        emit finished(false,"Brak nazwy rysunku w wierszu " + QString::number(currentRow) + ".");
        return nullptr;
    }

    int quantity = m_schedule->cellAt(currentRow, 5)->value().toInt();
    if(quantity == 0){
        emit finished(false,"Brak ilości dla rysunku: "+ drawingNumber + "");
        return nullptr;
    }

    QString dxfPath = findFilePath(m_schedule->cellAt(currentRow, 3)->value().toString());

    QString material =  defineMaterial(m_schedule->cellAt(currentRow, 8)->value().toString());
    if(material.isEmpty()) {
        emit finished(false,"Nie dopasowano gatunku dla rysunku: " + drawingNumber + "");
        return nullptr;
    }

    double thickness =  m_schedule->cellAt(currentRow, 11)->value().toDouble();
    if(thickness == 0) {
        emit finished(false,"Nie dopasowano grubości dla rysunku: " + drawingNumber + "");
        return nullptr;
    }

    std::shared_ptr<SinglePart> singlePart = std::make_shared<SinglePart>(number,drawingNumber,quantity,material,thickness,dxfPath);
    for(int i = 12; i <= m_lastColumn; ++i) {
        if(m_schedule->cellAt(currentRow, i)->value().toString().contains("X", Qt::CaseInsensitive)) {
            singlePart->addMachine(m_schedule->cellAt(6, i)->value().toString());
        }
    }
    if(singlePart->getMachineList().isEmpty()){
        emit finished(false,"Nie dopasowano maszyny dla rysunku: " + m_schedule->cellAt(currentRow, 3)->value().toString() + "");
        return nullptr;
    }

    emit addItemToListWidget(m_schedule->cellAt(currentRow, 3)->value().toString(), !dxfPath.isEmpty());
    return singlePart;
}

bool Finder::rowCount(int & lastRow)
{
    for (int row = 7; row < 65536; ++row) {
        if(QXlsx::Cell *cell=m_schedule->cellAt(row, 6)) {
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

bool Finder::columnCount(int & lastColumn)
{
    for(int i = 1; i < 65536; ++i) {
         if(m_schedule->cellAt(6,i)->value().toString().contains("Koniec")) {
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
    unsigned int filesCounter = 0;
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

bool Finder::checkSchedule()
{
    QString orderDigit = m_schedule->cellAt(6, 2)->value().toString();
    QString drawingNr = m_schedule->cellAt(6, 3)->value().toString();
    QString cooperator = m_schedule->cellAt(6, 10)->value().toString();
    QString thickness = m_schedule->cellAt(6,11)->value().toString();

    if( orderDigit.contains("L.p.") && drawingNr.contains("Nr rys.") && cooperator.contains("Kooperant") && thickness.contains("Grubość") )
        return true;
    else
        return false;
}
