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
    void processFile(Ui::MainWindow *ui, QStringList fileSelected, QString fileZip);
private slots:
    void on_actionAbout_triggered();
    void on_actionOpen_triggered();
    void updateTextEdit(QString text);
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
