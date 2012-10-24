#include "TerminalPrinter.h"

TerminalPrinter::TerminalPrinter(JarvisClient &client) : client(client), qtout(stdout)
{
   connect(&client, SIGNAL(msgInRoom(const QString &, const QString &, const QString &)), SLOT(msgInRoom(const QString &, const QString &, const QString &)));
   connect(&client, SIGNAL(newFunction(const QString &, const QString &, const QStringList &, const QString &)), SLOT(newFunction(const QString &, const QString &, const QStringList &, const QString &)));
   connect(&client, SIGNAL(newRoom(const QString &)), SLOT(newRoom(const QString &)));
   connect(&client, SIGNAL(deletedRoom(const QString &)), SLOT(deletedRoom(const QString &)));
   connect(&client, SIGNAL(newVariable(const QString &, const QString &, const QString &)), SLOT(newVariable(const QString &, const QString &, const QString &)));
   connect(&client, SIGNAL(newClient(const QString &, const QString &)), SLOT(newClient(const QString &, const QString &)));
   connect(&client, SIGNAL(clientLeft(const QString &, const QString &)), SLOT(clientLeft(const QString &, const QString &)));
   connect(&client, SIGNAL(error(JarvisClient::ClientError)), SLOT(error(JarvisClient::ClientError)));
   connect(&client, SIGNAL(pkgLoaded(const ModulePackage &)), SLOT(pkgLoaded(const ModulePackage &)));
   connect(&client, SIGNAL(pkgUnloaded(const QString &)), SLOT(pkgUnloaded(const QString &)));
   connect(&client, SIGNAL(enteredRoom(const QString &, const Room &)), SLOT(enteredRoom(const QString &, const Room &)));
   connect(&client, SIGNAL(receivedInitInfo(const QStringList &, const QList<ModulePackage> &)), SLOT(receivedInitInfo(const QStringList &, const QList<ModulePackage> &)));
   connect(&client, SIGNAL(disconnected()), SLOT(disconnected()));
}

void TerminalPrinter::newRoom(const QString &name)
{
    emit output("New Room: " + name);
    serverRooms.append(name);
}

void TerminalPrinter::newFunction(const QString &room, const QString &identifier, const QStringList &arguments, const QString &def)
{
    QString result = "New function definition (room " + room + "): " + identifier + "(" + arguments.front();
    for (QStringList::const_iterator it = arguments.begin() + 1; it != arguments.end(); ++it) result += "," + *it;
    emit output(result + ")=" + def);
    roomByName[room].functions.insert(identifier, qMakePair(arguments, def));
}

void TerminalPrinter::newVariable(const QString &room, const QString &identifier, const QString &definition)
{
    emit output("New variable definition (room " + room + "): " + identifier + "=" + definition);
    roomByName[room].variables.insert(identifier, definition);
}

void TerminalPrinter::newClient(const QString &room, const QString &name)
{
   emit output("New client (room " + room + "): " + name);
   roomByName[room].clients.append(name);
}

void TerminalPrinter::clientLeft(const QString &room, const QString &name)
{
    emit output("Client left (room " + room + "): " + name);


   roomByName[room].clients.removeOne(name);
}

void TerminalPrinter::msgInRoom(const QString &room, const QString &sender, const QString &msg)
{
   emit output("[" + room + "] " + sender + ": " + msg);


}

void TerminalPrinter::error(JarvisClient::ClientError error)
{
   emit output("Client Error " + QString::number(error));
}

void TerminalPrinter::pkgLoaded(const ModulePackage &pkg)
{
   emit output("Package loaded:");
   printPackage(pkg);


   pkgs.append(pkg);
}

void TerminalPrinter::pkgUnloaded(const QString &name)
{
   emit output("Package unloaded: " + name);


   pkgs.erase(std::remove_if(pkgs.begin(), pkgs.end(), [&](const ModulePackage &pkg) { return pkg.name == name; }));
}

void TerminalPrinter::enteredRoom(const QString &name, const Room &info)
{
   emit output("Entered room " + name + "; Clients:");
   for (const auto &client : info.clients) emit output(client + " ");
   emit output("Variables:");
   doPrintVars(info);
   emit output("Functions:");
   doPrintFuncs(info);

   roomByName.insert(name, info);
}

void TerminalPrinter::receivedInitInfo(const QStringList &rooms, const QList<ModulePackage> &pkgs)
{
    emit output("InitInfo received; Server has " + QString::number(rooms.size()) + " rooms and " + QString::number(pkgs.size()) + " packages.");
    serverRooms = rooms;
    this->pkgs = pkgs;
}

void TerminalPrinter::printClients()
{
    if (! currentRoom.isEmpty())
        for (const auto &client : roomByName[currentRoom].clients) emit output(client + " ");
}

void TerminalPrinter::printRooms()
{
    for (const auto &room : serverRooms) emit output(room + " ");


}

void TerminalPrinter::deletedRoom(const QString &name)
{
    emit output("Deleted room " + name);
    if (currentRoom == name) setCurrentRoom(QString());
    roomByName.remove(name);
    serverRooms.removeOne(name);
}

void TerminalPrinter::printModules()
{
    for (const auto &pkg : pkgs) printPackage(pkg);
}

void TerminalPrinter::leaveRoom(const QString &name)
{
    if (roomByName.contains(name)) {
        roomByName.remove(name);
        if (currentRoom == name) setCurrentRoom(QString());
        client.leaveRoom(name);
        emit output("Left room " + name);
    } else emit output("I'm not in a room called" + name);
}

void TerminalPrinter::printPackage(const ModulePackage &pkg)
{
    emit output(pkg.name);
    emit output(" * Terminals:");
    for (const auto &mod : pkg.terminals) {
        emit output("  > " + mod.name);
        emit output("   - description: " + mod.description);
    }
    emit output(" * Operators:");
    for (const auto &mod : pkg.operators) {
        emit output("  > " + mod.name);
        emit output("   - description: " + mod.description);
        emit output("   - matches: " + ((mod.matches == nullptr)? "<dynamic>" : *mod.matches));
        emit output("   - priority: " + (mod.priority.first ? QString::number(mod.priority.second) : "<dynamic>"));
        if (mod.associativity.first)
            emit output(QString("   - associativity: ") + ((mod.associativity.second == OperatorModule::LEFT)? "left" : "right"));
        else emit output("   - associativity: <dynamic>");
        emit output(QString("   - needsParseForMatch: ") + ((mod.needsParseForMatch) ? "true" : "false"));
    }
    emit output(" * Functions:");
    for (const auto &mod : pkg.functions) {
        emit output("  > " + mod.name);
        emit output("   - description: " + mod.description);
        emit output("   - matches: " + ((mod.matches == nullptr)? "<dynamic>" : (mod.matches->first + " | " + QString::number(mod.matches->second))));
        emit output("   - priority: " + (mod.priority.first ? QString::number(mod.priority.second) : "<dynamic>"));
    }
}

void TerminalPrinter::doPrintVars(const Room &room)
{
    for (auto it = room.variables.begin(); it != room.variables.end(); ++it) emit output(it.key() + "=" + it.value());
}

void TerminalPrinter::doPrintFuncs(const Room &room)
{
    for (auto it = room.functions.begin(); it != room.functions.end(); ++it) {
        QString result = it.key() + "(" + it.value().first.front();
        for (auto it_args = it.value().first.begin() + 1; it_args != it.value().first.end(); ++it_args) result += "," + *it_args;
        emit output(result + ")=" + it.value().second);
    }
}

void TerminalPrinter::openRoom(const QString &name)
{
    if (roomByName.contains(name)) setCurrentRoom(name);
    else emit output("Enter the room before opening it.");
}

void TerminalPrinter::disconnected()
{
    emit output("Server died :/ u happy now? Send Arlecks to revive me... mb try \"jarc reconnect\"");
    setCurrentRoom();
    roomByName.clear();
    serverRooms.clear();
    pkgs.clear();
}
