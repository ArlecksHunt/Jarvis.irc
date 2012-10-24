#ifndef TERMINALPRINTER_H
#define TERMINALPRINTER_H

#include <QObject>
#include "JarvisClient.h"
#include "ModulePackage.h"
#include <QTextStream>

class TerminalPrinter : public QObject
{
    Q_OBJECT

private:
    JarvisClient &client;
    QTextStream qtout;
    QString currentRoom;
    QMap<QString, Room>  roomByName;
    QStringList serverRooms;
    QList<ModulePackage> pkgs;
    void printPackage(const ModulePackage &pkg);

    void doPrintVars(const Room &room);
    void doPrintFuncs(const Room &room);
    void setCurrentRoom(const QString &name = QString()) { currentRoom = name; emit currentRoomChanged(name); }

public:
    explicit TerminalPrinter(JarvisClient &client);

signals:
    void output(const QString &);
    void currentRoomChanged(const QString &);
public slots:
    void newRoom(const QString &name);
    void newFunction(const QString &room, const QString &identifier, const QStringList &arguments, const QString &def);
    void newVariable(const QString &room, const QString &identifier, const QString &definition);
    void newClient(const QString &room, const QString &name);
    void clientLeft(const QString &room, const QString &name);
    void msgInRoom(const QString &room, const QString &sender, const QString &msg);
    void error(JarvisClient::ClientError error);
    void pkgLoaded(const ModulePackage &pkg);
    void pkgUnloaded(const QString &name);
    void enteredRoom(const QString &name, const Room &info);
    void receivedInitInfo(const QStringList &rooms, const QList<ModulePackage> &pkgs);
    void openRoom(const QString &name);
    void printClients();
    void printModules();
    void printVariables() { if (! currentRoom.isEmpty()) doPrintVars(roomByName[currentRoom]); }
    void printFunctions() { if (! currentRoom.isEmpty()) doPrintFuncs(roomByName[currentRoom]); }
    void leaveRoom(const QString &name);
    void printRooms();
    void msgToRoom(const QString &msg) { if (! currentRoom.isEmpty()) QMetaObject::invokeMethod(&client, "msgToRoom", Q_ARG(QString, currentRoom), Q_ARG(QString, msg)); }
    void deletedRoom(const QString &name);
    void disconnected();

};

#endif // TERMINALPRINTER_H
