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
    void Compress(QStringList filesInput, QString fileZip, QString rootDir);
    void Abort();
    void Extract(QString fileZip, QString dirOutput);
signals:
    void updateStatus(QString status);

protected:
    void run() Q_DECL_OVERRIDE;

private:
    QMutex mutex;
    QWaitCondition condition;
    bool abort;
    QStringList filesInput;
    QString fileZip;
    QString dirOutput;
    QString rootDir;
    int mode;
    enum { MODE_COMPRESS, MODE_EXTRACT, MODE_UNKNOWN};
    int on_extract_entry(const char *fileName, void *arg);
};

#endif // ZIPTHREAD_H
