#include <QCoreApplication>
#include <QThread>
#include "TerminalPrinter.h"
//#include "InputWorker.h"
#include "IRC.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    /*QTextStream qtout(stdout);
    QTextStream qtin(stdin);
    QString nick;
    QString server;
    qtout << "Server:\t";
    qtout.flush();
    qtin >> server;
    qtout << "Nick:\t";
    qtout.flush();
    qtin >> nick;
    QThread thread;*/
    Client ircc;
    //worker.moveToThread(&thread);
    //thread.start();
    //QMetaObject::invokeMethod(&worker, "doWork", Qt::QueuedConnection);
    return a.exec();
}
