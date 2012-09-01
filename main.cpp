#include <QCoreApplication>
#include <QThread>
#include "InputWorker.h"
#include "IRC.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Client ircc;
    QThread thread;
    InputWorker worker(ircc);
    worker.moveToThread(&thread);
    thread.start();
    QMetaObject::invokeMethod(&worker, "doWork", Qt::QueuedConnection);
    return a.exec();
}
