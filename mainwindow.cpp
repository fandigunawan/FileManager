#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include "zip/src/zip.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(0, tr("About File Manager"), tr("Created using Qt 5.6.2 by Fandi"));
}

void MainWindow::on_actionOpen_triggered()
{
    QStringList fileSelected = QFileDialog::getOpenFileNames(0, "Open File", QDir::homePath(), "Any file (*.*)");
    if(fileSelected.isEmpty())
    {
        QMessageBox::information(this, "Information", "No file is selected, the process is aborted");
        return;
    }
    QString fileZip = QFileDialog::getSaveFileName(0, "Save to file", QDir::homePath(), "Zip file (*.zip)");
    if(fileZip == nullptr)
    {
        QMessageBox::information(this, "Information", "No file is selected, the process is aborted");
        return;
    }
    updateTextEdit("Creating zip file : " + fileZip + "\n");
    processFile(fileSelected, fileZip, "");


}
void MainWindow::updateTextEdit(QString text)
{
    ui->plainTextEdit->appendPlainText(text);
}

void MainWindow::processFile(QStringList filesInput, QString fileZip, QString rootDir = "")
{
    qint64 fileSizeTotal = 0;
    struct zip_t *zip = zip_open(fileZip.toLocal8Bit().data(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');
    {
        for(QString file : filesInput)
        {
            qDebug() << file;
            QFileInfo fi(file);
            if(fi.exists(file))
            {
                updateTextEdit("Compressing : " + file + " " + QString::number(fi.size()) + " bytes");
                fileSizeTotal += fi.size();
                QString temp(file);
                if(rootDir != "")
                {
                    temp = temp.remove(rootDir + "/");
                    qDebug() << "Temp = " << temp << " File " << file;
                }

                zip_entry_open(zip, temp.toLocal8Bit().data());
                {
                    zip_entry_fwrite(zip, file.toLocal8Bit().data());
                }
                zip_entry_close(zip);
            }
            else
            {
                updateTextEdit("File is not exist : " + file);
            }
        }
    }
    zip_close(zip);
    QFileInfo fiZip(fileZip);
    if(fiZip.exists())
    {
        updateTextEdit("Compressed file result size " + QString::number(fiZip.size()) + "/" + QString::number(fileSizeTotal)
                                           + " " + QString::number(fiZip.size() * 100 / fileSizeTotal) + "%");
    }
}

void MainWindow::on_actionOpen_Folder_triggered()
{
    QString dirSelected = QFileDialog::getExistingDirectory(this, "Open Directory", QDir::homePath());
    if(dirSelected.isEmpty())
    {
        QMessageBox::information(this, "Information", "No directory is selected, the process is aborted");
        return;
    }
    QString fileZip = QFileDialog::getSaveFileName(this, "Save to file", QDir::homePath(), "Zip file (*.zip)");
    if(fileZip == nullptr)
    {
        QMessageBox::information(this, "Information", "No file is selected, the process is aborted");
        return;
    }

    QStringList fileSelected = findFilesRecursively(dirSelected);
    updateTextEdit("Creating zip file : " + fileZip + "\n");
    processFile(fileSelected, fileZip, dirSelected);
}
// this function stores the absolute paths of each file in a QVector
QStringList MainWindow::findFilesRecursively(QString dirPath)
{
    QStringList listOfFiles;
    QDir dir(dirPath);
    dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    QDirIterator it(dir, QDirIterator::Subdirectories);
    while(it.hasNext())
    {
       listOfFiles.push_back(it.next());
    }
    return listOfFiles;
}
