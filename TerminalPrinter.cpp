#include "TerminalPrinter.h"

TerminalPrinter::TerminalPrinter(JarvisClient &client) : client(client), qtout(stdout)
{
   connect(&client, SIGNAL(msgInScope(const QString &, const QString &, const QString &)), SLOT(msgInScope(const QString &, const QString &, const QString &)));
   connect(&client, SIGNAL(newFunction(const QString &, const QString &, const QStringList &, const QString &)), SLOT(newFunction(const QString &, const QString &, const QStringList &, const QString &)));
   connect(&client, SIGNAL(newScope(const QString &)), SLOT(newScope(const QString &)));
   connect(&client, SIGNAL(deletedScope(const QString &)), SLOT(deletedScope(const QString &)));
   connect(&client, SIGNAL(newVariable(const QString &, const QString &, const QString &)), SLOT(newVariable(const QString &, const QString &, const QString &)));
   connect(&client, SIGNAL(newClient(const QString &, const QString &)), SLOT(newClient(const QString &, const QString &)));
   connect(&client, SIGNAL(clientLeft(const QString &, const QString &)), SLOT(clientLeft(const QString &, const QString &)));
   connect(&client, SIGNAL(error(JarvisClient::ClientError)), SLOT(error(JarvisClient::ClientError)));
   connect(&client, SIGNAL(pkgLoaded(const QVariant &)), SLOT(pkgLoaded(const QVariant &)));
   connect(&client, SIGNAL(pkgUnloaded(const QString &)), SLOT(pkgUnloaded(const QString &)));
   connect(&client, SIGNAL(enteredScope(const QString &, const QVariant &)), SLOT(enteredScope(const QString &, const QVariant &)));
   connect(&client, SIGNAL(receivedInitInfo(const QVariant &, const QVariant &)), SLOT(receivedInitInfo(const QVariant &, const QVariant &)));
   connect(&client, SIGNAL(disconnected()), SLOT(disconnected()));
}

void TerminalPrinter::newScope(const QString &name)
{
   emit output("New Scope: " + name);
   scopeByName.insert(name, Scope());
}

void TerminalPrinter::newFunction(const QString &scope, const QString &identifier, const QStringList &arguments, const QString &def)
{
   QString result = "New function definition (scope " + scope + "): " + identifier + "(" + arguments.front();
   for (QStringList::const_iterator it = arguments.begin() + 1; it != arguments.end(); ++it) result += "," + *it;
   emit output(result + ")=" + def);
}

void TerminalPrinter::newVariable(const QString &scope, const QString &identifier, const QString &definition)
{
   emit output("New variable definition (scope " + scope + "): " + identifier + "=" + definition);


}

void TerminalPrinter::newClient(const QString &scope, const QString &name)
{
   emit output("New client (scope " + scope + "): " + name);


   scopeByName[scope].clients.append(name);
}

void TerminalPrinter::clientLeft(const QString &scope, const QString &name)
{
   emit output("Client left (scope " + scope + "): " + name);


   scopeByName[scope].clients.removeOne(name);
}

void TerminalPrinter::msgInScope(const QString &scope, const QString &sender, const QString &msg)
{
   emit output("[" + scope + "] " + sender + ": " + msg);


}

void TerminalPrinter::error(JarvisClient::ClientError error)
{
   emit output("Client Error " + QString::number(error));
}

void TerminalPrinter::pkgLoaded(const QVariant &pkg)
{
   emit output("Package loaded:");
   printPackage(pkg.value<ModulePackage>());


   pkgs.append(pkg.value<ModulePackage>());
}

void TerminalPrinter::pkgUnloaded(const QString &name)
{
   emit output("Package unloaded: " + name);


   pkgs.erase(std::remove_if(pkgs.begin(), pkgs.end(), [&](const ModulePackage &pkg) { return pkg.name == name; }));
}

void TerminalPrinter::enteredScope(const QString &name, const QVariant &info)
{
   Scope infoScope = info.value<Scope>();
   emit output("Entered scope " + name + "; Clients:");
   for (const auto &client : infoScope.clients) emit output(client + " ");
   emit output("Variables:");
   for (QMap<QString, QString>::ConstIterator it = infoScope.variables.begin(); it != infoScope.variables.end(); ++it) emit output(it.key() + "=" + it.value() + " ");
   emit output("Functions:");
   for (QMap<QString, QPair<QStringList, QString>>::ConstIterator it = infoScope.functions.begin(); it != infoScope.functions.end(); ++it) emit output(it.key() + it.value().first[0] + "=" + it.value().second + " ");

   scopeByName.insert(name, info.value<Scope>());
}

void TerminalPrinter::receivedInitInfo(const QVariant &scopes, const QVariant &pkgs)
{
   emit output("InitInfo:");
   emit output("Scopes:");
   for (const auto &scope : scopes.value<QStringList>()) {
       emit output(scope + " ");
       scopeByName.insert(scope, Scope());
   }
   emit output("Packages:");
   for (const auto &pkg : pkgs.value<QList<ModulePackage> >()) {
       printPackage(pkg);
   }
   this->pkgs = pkgs.value<QList<ModulePackage> >();


}

void TerminalPrinter::printClients()
{
    for (const auto &client : scopeByName[currentScope].clients) emit output(client + " ");


}

void TerminalPrinter::printScopes()
{
    for (const auto &scope : scopeByName.keys()) emit output(scope + " ");


}

void TerminalPrinter::deletedScope(const QString &name)
{
    emit output("Deleted scope " + name);

    scopeByName.remove(name);
}

void TerminalPrinter::printModules()
{
    for (const auto &pkg : pkgs) printPackage(pkg);


}

void TerminalPrinter::leaveScope(const QString &name)
{
    scopeByName.remove(name);
    client.leaveScope(name);
}

void TerminalPrinter::printPackage(const ModulePackage &pkg)
{
    emit output("Package Name Module Name Module Description");
    emit output(pkg.name);
    emit output(" Terminals:");
    for (const auto &mod : pkg.terminals) {
        emit output("  " + mod.name + " " + mod.description);
    }
    emit output(" Operators:");
    for (const auto &mod : pkg.operators) {
        emit output("  " + mod.name + " " + mod.description);
    }
    emit output(" Functions:");
    for (const auto &mod : pkg.functions) {
        emit output("  " + mod.name + " " + mod.description);
    }

}

