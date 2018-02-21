#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void processFile(QStringList fileSelected, QString fileZip, QString rootDir);
    QStringList findFilesRecursively(QString dirPath);
private slots:
    void on_actionAbout_triggered();
    void on_actionOpen_triggered();
    void updateTextEdit(QString text);
    void on_actionOpen_Folder_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
