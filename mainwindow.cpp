#include <QMessageBox>
#include <QFileDialog>
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
    QString fileZip = QFileDialog::getSaveFileName(0, "Save to file", QDir::homePath(), "Zip file (*.zip)");
    ui->plainTextEdit->appendPlainText("Creating zip file : " + fileZip + "\n");
    struct zip_t *zip = zip_open(fileZip.toLocal8Bit().data(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');
    {
        for(QString file : fileSelected)
        {
            ui->plainTextEdit->appendPlainText("Compressing : " + file + "\n");
            zip_entry_open(zip, file.toLocal8Bit().data());
            {
                zip_entry_fwrite(zip, file.toLocal8Bit().data());
            }
            zip_entry_close(zip);
        }
    }
    zip_close(zip);
}
