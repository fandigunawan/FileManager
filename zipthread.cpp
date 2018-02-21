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
    // Intially set mode to unknown
    mode = MODE_UNKNOWN;
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
        this->mode = MODE_UNKNOWN;
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
    QString fileZip(this->fileZip);
    QString rootDir(this->rootDir);
    QString dirOutput(this->dirOutput);
    int mode = this->mode;
    mutex.unlock();

    switch(mode)
    {
    case MODE_COMPRESS:
        {
            // Actual zip process
            qint64 fileSizeTotal = 0;
            struct zip_t *zip = zip_open(fileZip.toLocal8Bit().data(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');
            {
                for(QString file : filesInput)
                {
                    if(abort)
                    {
                        zip_close(zip);
                        QFileInfo fiZip(fileZip);
                        if(fiZip.exists())
                        {
                            emit updateStatus("Compressed file result size " + QString::number(fiZip.size()) + "/" + QString::number(fileSizeTotal)
                                                               + " " + QString::number(fiZip.size() * 100 / fileSizeTotal) + "%");
                        }
                        emit updateStatus("Aborting");
                        return;
                    }
                    qDebug() << "Compressing file : " << file;
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
            QFileInfo fiZip(fileZip);
            if(fiZip.exists())
            {
                emit updateStatus("Compressed file result size " + QString::number(fiZip.size()) + "/" + QString::number(fileSizeTotal)
                                                   + " " + QString::number(fiZip.size() * 100 / fileSizeTotal) + "%");
            }
        }
        break;

    case MODE_EXTRACT:
        emit updateStatus("Please wait.....");
        zip_extract(fileZip.toLocal8Bit().data(), dirOutput.toLocal8Bit().data(), NULL, NULL);
        emit updateStatus("Extract zip file is completed");
        break;
    }
    qDebug() << "Exit zip process";
    mutex.lock();
    this->mode = MODE_UNKNOWN;
    mutex.unlock();
}
int ZipThread::on_extract_entry(const char *fileName, void *arg)
{
    static int i = 0;
    int n = *(int *)arg;
    emit updateStatus(QString::number(++i) + "/" + QString::number(n) + " Extracting " + QString(fileName));
    //printf("Extracted: %s (%d of %d)\n", filename, ++i, n);

    return 0;
}
/*!
 * \brief Compress file, a thread will be started
 * \param filesInput list of files
 * \param fileOutput zip file output
 * \param rootDir directory root of filesInput, if selected directory is root directory insert "" instead
 */
void ZipThread::Compress(QStringList filesInput, QString fileZip, QString rootDir)
{
    // Make sure that there is no thread is running
    if(isRunning() == true)
    {
        return;
    }
    this->abort = false;
    this->filesInput.clear();
    this->filesInput.append(filesInput);

    this->fileZip = fileZip;

    this->rootDir = rootDir;
    this->mode = MODE_COMPRESS;

    // Start thread
    start(LowPriority);

}
/*!
 * \brief Extract zip file
 * \param Input zip file
 * \param Ouput directory
 */
void ZipThread::Extract(QString fileZip, QString dirOutput)
{
    // Make sure that the thread is not running
    if(isRunning() == true)
    {
        return;
    }
    this->filesInput.clear();
    this->abort = false;
    this->fileZip = fileZip;
    this->dirOutput = dirOutput;
    this->mode = MODE_EXTRACT;
    this->rootDir = "";

    start(LowPriority);
}
