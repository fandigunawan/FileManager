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
    connect(&thread, SIGNAL(updateStatus(QString)), this, SLOT(statusUpdated(QString)));
    connect(&thread, SIGNAL(finished()), this, SLOT(actionCancelEnabled()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About File Compression"), tr("Created using Qt 5.6.2 by Fandi"));
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
    ui->plainTextEdit->clear();
    statusUpdated("Creating zip file : " + fileZip + "\n");
    ui->actionCancel->setEnabled(true);
    ui->actionOpen->setEnabled(false);
    ui->actionOpen_Folder->setEnabled(false);
    thread.Compress(fileSelected, fileZip, "");


}
void MainWindow::statusUpdated(QString text)
{
    ui->plainTextEdit->appendPlainText(text);
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
    ui->plainTextEdit->clear();
    statusUpdated("Creating zip file : " + fileZip + "\n");
    ui->actionCancel->setEnabled(true);
    ui->actionOpen->setEnabled(false);
    ui->actionOpen_Folder->setEnabled(false);
    thread.Compress(fileSelected, fileZip, dirSelected);
}
/*!
 * \brief Find files recursively
 * \param dirPath input directory name
 * \return list of files, including directory name
 */
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


void MainWindow::on_actionCancel_triggered()
{
    thread.Abort();
}
void MainWindow::actionCancelEnabled()
{
    ui->actionCancel->setEnabled(false);
    ui->actionOpen->setEnabled(true);
    ui->actionOpen_Folder->setEnabled(true);
}
