#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "zipthread.h"
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
    void statusUpdated(QString text);
    void actionCancelEnabled();
    void on_actionAbout_triggered();
    void on_actionOpen_triggered();
    void on_actionOpen_Folder_triggered();
    void on_actionCancel_triggered();



    void on_actionOpen_Zip_triggered();

    void on_actionExit_triggered();

private:
    Ui::MainWindow *ui;
    ZipThread thread;
    void processFile(QStringList fileSelected, QString fileZip, QString rootDir);
    QStringList findFilesRecursively(QString dirPath);
};

#endif // MAINWINDOW_H
