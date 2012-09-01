#ifndef INPUTWORKER_H
#define INPUTWORKER_H

#include "IRC.h"

class InputWorker : public QObject
{
    Q_OBJECT

private:
    Client &client;

public:
    InputWorker(Client &client) : client(client) {};

public slots:
    void doWork() {
        QTextStream qtin(stdin);
        QString input;
        for (;;) {
            QMetaObject::invokeMethod(&client, "broadcast", Q_ARG(QString, qtin.readLine()));
        }
    }
};

#endif // INPUTWORKER_H
