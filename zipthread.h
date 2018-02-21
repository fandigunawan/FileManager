#ifndef ZIPTHREAD_H
#define ZIPTHREAD_H

#include <QMutex>
#include <QThread>
#include <QWaitCondition>

class ZipThread : public QThread
{
    Q_OBJECT
public:
    ZipThread(QObject * parent = 0);
    ~ZipThread();
    void Compress(QStringList filesInput, QString fileOutput, QString rootDir);
    void Abort();

signals:
    void updateStatus(QString status);

protected:
    void run() Q_DECL_OVERRIDE;

private:
    QMutex mutex;
    QWaitCondition condition;
    bool abort;
    QStringList filesInput;
    QString fileOutput;
    QString rootDir;
};

#endif // ZIPTHREAD_H
