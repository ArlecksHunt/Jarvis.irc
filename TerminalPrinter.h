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
    QString currentScope;
    QMap<QString, Scope>  scopeByName;
    QList<ModulePackage> pkgs;
    void printPackage(const ModulePackage &pkg);

public:
    explicit TerminalPrinter(JarvisClient &client);

signals:
    void output(const QString &);
public slots:
    void newScope(const QString &name);
    void newFunction(const QString &scope, const QString &identifier, const QStringList &arguments, const QString &def);
    void newVariable(const QString &scope, const QString &identifier, const QString &definition);
    void newClient(const QString &scope, const QString &name);
    void clientLeft(const QString &scope, const QString &name);
    void msgInScope(const QString &scope, const QString &sender, const QString &msg);
    void error(JarvisClient::ClientError error);
    void pkgLoaded(const QVariant &pkg);
    void pkgUnloaded(const QString &name);
    void enteredScope(const QString &name, const QVariant &info);
    void receivedInitInfo(const QVariant &scopes, const QVariant &pkgs);
    void openScope(const QString &name) { currentScope = name; qtout << "\n(" << currentScope << ")->"; qtout.flush(); }
    void printClients();
    void printModules();
    void leaveScope(const QString &name);
    void printScopes();
    void msgToScope(const QString &msg) { QMetaObject::invokeMethod(&client, "msgToScope", Q_ARG(QString, currentScope), Q_ARG(QString, msg)); }
    void deletedScope(const QString &name);
    void disconnected() { qtout << "\nDisconnected!\n"; qtout.flush(); }

};

#endif // TERMINALPRINTER_H