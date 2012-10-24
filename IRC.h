#pragma once
#include <QUrl>
#include <QDateTime>
#include <QTcpSocket>
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QListWidget>
#include <QTreeWidget>
#include <QAction>
#include <QSplitter>
#include "TerminalPrinter.h"
#include <queue>
#include "JarvisClient.h"
#include <QTimer>

class Channel : public QMainWindow {
    Q_OBJECT;
public:
    Channel(QString name, JarvisClient &jclient, TerminalPrinter &printer);
    void addUser(QString);
    void removeUser(QString);
    void addMessage(QString,QString);
signals:
    void notify(QString,QString);
    void send(QString,QString);
    void send_cmd(QString);
    void stopTransmission();
public slots:
    void send(QString);
    void currentRoomChanged(const QString &currentRoom);

private slots:
    void highlight(QString);
    void send();
private:
    QString currentnick = "jarc";
    QString wanswer;
    QString channel;
    QListWidget users;
    QPlainTextEdit text;
    QLineEdit lineEdit;
    JarvisClient &jclient;
    TerminalPrinter &printer;
    bool multiline{false};
    QString mlBuff;
};

class Client : public QObject {
    Q_OBJECT
public:
    Client();
    Channel* newChannel(QString target);
public slots:
    //void notify(QString,QString);
    void broadcast(QString msg);
protected:
    //void hideEvent(QHideEvent *);
private slots:
    //void open(QTreeWidgetItem*);
    //void fitWindow();
private:
    //QMap<QString,QTreeWidgetItem*> channels;
    QList<Channel*> actual_channels;
    //JarvisClient &jclient;
    //TerminalPrinter &printer;
};

class Network : public QTcpSocket {
    Q_OBJECT;
public:
    Network(QUrl,Client*);
    ~Network();
private slots:
    void receive();
    void send(QString);
    void send(QString,QString);
    void actual_send() { if (! queue.empty()) { send(queue.front()); queue.pop(); } }
    void stopTransmission() { std::queue<QString>().swap(queue); }
private:
    Channel* getChannel(QString);

    Client* client;
    QMap<QString,Channel*> channels;
    QString user;
    std::queue<QString> queue;
};
