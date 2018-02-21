#include <QThread>
#include <QDebug>
#include <QFileInfo>
#include "zipthread.h"
#include "zip/src/zip.h"
/*!
 * \brief ZipThread::ZipThread Constructor
 * \param parent
 */
ZipThread::ZipThread(QObject * parent) : QThread(parent)
{
    abort = false;
}
/*!
 * \brief ZipThread::~ZipThread Destructor
 */
ZipThread::~ZipThread()
{
    Abort();
}
/*!
 * \brief Aborth thread if it is running
 */
void ZipThread::Abort()
{
    if(isRunning())
    {
        mutex.lock();
        abort = true;
        condition.wakeOne();
        mutex.unlock();

        wait();
    }
}
/*!
 * \brief run main thread, this is an overriden of protected run function derrived from QThread
 */
void ZipThread::run()
{
    // Prevent data from being overwritten
    mutex.lock();
    QStringList filesInput(this->filesInput);
    QString fileOutput(this->fileOutput);
    QString rootDir(this->rootDir);
    mutex.unlock();

    // Actual zip process
    qint64 fileSizeTotal = 0;
    struct zip_t *zip = zip_open(fileOutput.toLocal8Bit().data(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');
    {
        for(QString file : filesInput)
        {
            if(abort)
            {
                zip_close(zip);
                QFileInfo fiZip(fileOutput);
                if(fiZip.exists())
                {
                    emit updateStatus("Compressed file result size " + QString::number(fiZip.size()) + "/" + QString::number(fileSizeTotal)
                                                       + " " + QString::number(fiZip.size() * 100 / fileSizeTotal) + "%");
                }
                emit updateStatus("Aborting");
                return;
            }
            qDebug() << "Process file : " << file;
            QFileInfo fi(file);
            if(fi.exists(file))
            {
                if(fi.isFile())
                {
                    emit updateStatus("Compressing : " + file + " " + QString::number(fi.size()) + " bytes");
                    fileSizeTotal += fi.size();
                    QString temp(file);
                    if(rootDir != "")
                    {
                        temp = temp.remove(rootDir + "/");
                    }

                    zip_entry_open(zip, temp.toLocal8Bit().data());
                    {
                        zip_entry_fwrite(zip, file.toLocal8Bit().data());
                    }
                    zip_entry_close(zip);
                }
            }
            else
            {
                emit updateStatus("File is not exist : " + file);
            }
        }
    }
    zip_close(zip);
    QFileInfo fiZip(fileOutput);
    if(fiZip.exists())
    {
        emit updateStatus("Compressed file result size " + QString::number(fiZip.size()) + "/" + QString::number(fileSizeTotal)
                                           + " " + QString::number(fiZip.size() * 100 / fileSizeTotal) + "%");
    }
    qDebug() << "Exit zip process";
}
/*!
 * \brief Compress file, a thread will be started
 * \param filesInput list of files
 * \param fileOutput zip file output
 * \param rootDir directory root of filesInput, if selected directory is root directory insert "" instead
 */
void ZipThread::Compress(QStringList filesInput, QString fileOutput, QString rootDir)
{
    if(isRunning() == true)
    {
        return;
    }
    this->abort = false;
    this->filesInput.clear();
    this->filesInput.append(filesInput);

    this->fileOutput = fileOutput;

    this->rootDir = rootDir;
    // Start thread
    start(LowPriority);

}
