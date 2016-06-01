#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QtXml>
#include <memory>
#include <QThread>
#include "finder.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_excelPathBtn_released();
    void on_xmlPathBtn_released();
    void on_searchPathBtn_clicked();
    void on_convertButton_released();
    void on_setValue(int value, QString labelText);
    void on_addItemToListWidget(QString itemName, bool isFound);
    void on_processingFinished(bool isSuccess, QString information);

private:
    Ui::MainWindow *ui;
    QThread * m_finderThread{nullptr};
    Finder * m_finder{nullptr};
    bool m_processing{false};
    bool m_isUpload;
    void createCommandTag(std::unique_ptr<QXmlStreamWriter> &xml, std::shared_ptr<Part> part, int counter);

    void createAssemblyCommand(std::unique_ptr<QXmlStreamWriter> &xml, const std::shared_ptr<Assembly> &assembly);
    void createPartCommand(std::unique_ptr<QXmlStreamWriter> &xml, const std::shared_ptr<SinglePart> &part);
    void createManufacturingCommand(std::unique_ptr<QXmlStreamWriter> &xml, const std::shared_ptr<Part> &part, int &counter);
    void assignPartToAssembly(std::unique_ptr<QXmlStreamWriter> &xml, const QString &assemblyDrawingNumber, const std::shared_ptr<Part> &part);
    void createXMLContent(std::unique_ptr<QXmlStreamWriter> &xml, const QString & assemblyDrawingNumber, QList<std::shared_ptr<Part>> &partsList);
    bool createXML();
    QStringList getItemsFromFile(const QString &fileName);
    void generatePartList();
    void setProcessing(bool isEnabled);
};

#endif // MAINWINDOW_H
