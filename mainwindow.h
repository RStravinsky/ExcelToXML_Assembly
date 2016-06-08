#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QtXml>
#include <memory>
#include <QThread>
#include <QFile>
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
    void on_zCb_toggled(bool checked);
    void on_manufacturingCb_toggled(bool checked);
    void on_productsCb_toggled(bool checked);
    void on_prodOperCb_toggled(bool checked);
    void on_importgeoCB_toggled(bool checked);

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QThread * m_finderThread{nullptr};
    Finder * m_finder{nullptr};
    bool m_processing{false};
    bool m_isUpload;

    bool m_isZ{true};
    bool m_isProducts{true};
    bool m_isProdOper{true};
    bool m_isImportgeo{true};
    bool m_isManufacturing{true};

    void createAssemblyCommand(std::unique_ptr<QXmlStreamWriter> &xml, const std::shared_ptr<Assembly> &assembly);
    void createPartCommand(std::unique_ptr<QXmlStreamWriter> &xml, const std::shared_ptr<SinglePart> &part);
    void createManufacturingCommand(std::unique_ptr<QXmlStreamWriter> &xml, const std::shared_ptr<Part> &part, int &counter);
    void assignPartToAssembly(std::unique_ptr<QXmlStreamWriter> &xml, const std::shared_ptr<Assembly> &mainAssembly, const std::shared_ptr<Part> &part);
    void createXMLContent(std::unique_ptr<QXmlStreamWriter> &xml, const std::shared_ptr<Assembly> &mainAssembly, QList<std::shared_ptr<Part>> &partsList, int &counter);
    bool createXML();
    QStringList getItemsFromFile(const QString &fileName);
    void generatePartList();
    void setProcessing(bool isEnabled);
};

#endif // MAINWINDOW_H
