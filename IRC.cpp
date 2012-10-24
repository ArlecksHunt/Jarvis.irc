#include "IRC.h"

Client::Client() {
    /*setWindowTitle("-Chat Client"); setWindowIcon(QIcon::fromTheme("kopete"));
    setFrameShape(QFrame::NoFrame);
    setHeaderHidden(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(this,SIGNAL(itemActivated(QTreeWidgetItem*,int)),SLOT(open(QTreeWidgetItem*)));
    */QStringList args = qApp->arguments(); args.removeFirst();
    foreach(QString arg,args) { QUrl url(arg);
        Network* network;
        if(url.scheme()=="irc") network = new Network(url,this);
    }
}
Channel* Client::newChannel(QString name) {
    JarvisClient *jclient = new JarvisClient("localhost", 4200, name, "supersecret");
    TerminalPrinter *printer = new TerminalPrinter(*jclient);
    Channel* channel = new Channel(name, *jclient, *printer);
    connect(printer, SIGNAL(output(QString)), channel, SLOT(send(QString)));
    connect(printer, SIGNAL(currentRoomChanged(const QString &)), channel, SLOT(currentRoomChanged(const QString&)));
    actual_channels.append(channel);
    /*QTreeWidgetItem* item = channels[name] = new QTreeWidgetItem(this,QStringList(name));
    item->setData(0,Qt::UserRole,QVariant::fromValue((QWidget*)channel));
    connect(channel,SIGNAL(notify(QString,QString)),SLOT(notify(QString,QString)));
    fitWindow();*/
    return channel;
}
/*
void Client::notify(QString channel,QString msg) {
    new QTreeWidgetItem(channels[channel],QStringList(msg));
    channels[channel]->setExpanded(true); fitWindow();
    setWindowTitle("*"+msg);
}
void Client::hideEvent(QHideEvent*) {
    for(int i=0;i<topLevelItemCount();i++) foreach(QTreeWidgetItem* item,topLevelItem(i)->takeChildren())
        delete item;
    fitWindow(); setWindowTitle("-Chat Client");
}
void Client::open(QTreeWidgetItem* item) {
    do {
        QWidget* channel = item->data(0,Qt::UserRole).value<QWidget*>();
        if(channel) { channel->showMaximized(); channel->raise(); break; }
    } while((item=item->parent()));
}
void Client::fitWindow() {
    int rowCount=0;
    for(int i=0;i<topLevelItemCount();i++,rowCount++)
        rowCount+=topLevelItem(i)->childCount();
    resize(sizeHintForColumn(0),sizeHintForRow(0)*rowCount);
}*/

Channel::Channel(QString channel, JarvisClient &jclient, TerminalPrinter &printer) : channel(channel), jclient(jclient), printer(printer) { setWindowTitle(channel);
    QSplitter* splitter = new QSplitter(this);
    text.setReadOnly(true);
    splitter->addWidget(&text);
    users.setSortingEnabled(true);
    connect(&users,SIGNAL(currentTextChanged(QString)),SLOT(highlight(QString)));
    splitter->addWidget(&users); users.hide();
    setCentralWidget(splitter);

    QToolBar* toolBar = new QToolBar(this);
    addToolBar(Qt::BottomToolBarArea,toolBar);
    toolBar->setMovable(false);
    toolBar->toggleViewAction()->setVisible(false);
    connect(&lineEdit,SIGNAL(returnPressed()),SLOT(send()));
    toolBar->addWidget(&lineEdit);
}
QString niceName(QString name) { name.replace(QRegExp("\\W"),""); return name[0].toUpper()+name.mid(1); }
void Channel::addUser(QString name) {
    name=niceName(name);
    if(users.findItems(name,Qt::MatchCaseSensitive).isEmpty()) users.addItem(name);
    users.setMaximumWidth(users.sizeHintForColumn(0)+16); users.show();
}
void Channel::removeUser(QString name) {
    name=niceName(name);
    foreach(QListWidgetItem* item, users.findItems(name,Qt::MatchCaseSensitive)) users.removeItemWidget(item);
}
void Channel::addMessage(QString origin,QString msg) {
    if (! wanswer.isEmpty() && niceName(origin) == "James_Jarvis" && msg.startsWith("@" + currentnick + ":") && msg.mid(3 + currentnick.length(), msg.indexOf(' ', 3 + currentnick.length()) - 3 - currentnick.length()) == wanswer) {
        if (msg.mid(msg.indexOf("roles:") + 7, msg.indexOf(" and", msg.indexOf("roles:") + 7) - msg.indexOf("roles:") - 7).split(',').contains("cankill")) {
            send("Goodbye, cruel " + wanswer);
            QTimer::singleShot(2000, QCoreApplication::instance(), SLOT(quit()));
        } else {
            send("@" + wanswer + ": SUCK MY DICK");
            send("hubot deal with it");
            send("hubot " + wanswer + " is an idiot");
            send("hubot it's been 0 days since " + wanswer + " GOT OWNED");
            send("hubot ascii JARC RULEZ");
        }
        wanswer.clear();
    }
    if (! msg.startsWith("jarc ")) return;
    msg = msg.mid(5);
    if (msg == "die") {
        send("hubot what role does " + niceName(origin) + " have");
        wanswer = niceName(origin);
    } else if (msg.startsWith("enter ")) QMetaObject::invokeMethod(&jclient, "enterRoom", Q_ARG(QString, msg.right(msg.length() - 6)));
    else if (msg.startsWith("leave ")) QMetaObject::invokeMethod(&jclient, "leaveRoom", Q_ARG(QString, msg.right(msg.length() - 6)));
    else if (msg.startsWith("open ")) QMetaObject::invokeMethod(&printer, "openRoom", Q_ARG(QString, msg.right(msg.length() - 5)));
    else if (msg == "modules") QMetaObject::invokeMethod(&printer, "printModules");
    else if (msg.startsWith("unload ")) QMetaObject::invokeMethod(&jclient, "unloadPkg", Q_ARG(QString, msg.right(msg.length() - 7)));
    else if (msg.startsWith("load ")) QMetaObject::invokeMethod(&jclient, "loadPkg", Q_ARG(QString, msg.right(msg.length() - 5)));
    else if (msg.startsWith("delete ")) QMetaObject::invokeMethod(&jclient, "deleteRoom", Q_ARG(QString, msg.right(msg.length() - 7)));
    else if (msg == "clients") QMetaObject::invokeMethod(&printer, "printClients");
    else if (msg == "rooms") QMetaObject::invokeMethod(&printer, "printRooms");
    else if (msg == "variables") QMetaObject::invokeMethod(&printer, "printVariables");
    else if (msg == "functions") QMetaObject::invokeMethod(&printer, "printFunctions");
    else if (msg == "reconnect") jclient.connect("localhost", 4200, channel, "supersecret");
    else if (msg == "stop" || msg == "stahp") emit stopTransmission();
    else if (msg == "ml") {
        if (multiline) {
            multiline = false;
            if (mlBuff.right(1) == "\n") mlBuff.chop(1);
            QMetaObject::invokeMethod(&printer, "msgToRoom", Q_ARG(QString, mlBuff));
            mlBuff.clear();
        } else multiline = true;
    } else if (msg == "help") {
        send("jarc ...");
        send("enter [room]");
        send("open [room]");
        send("leave [room]");
        send("delete [room]");
        send("die");
        send("load [modulepkg]");
        send("unload [modulepkg]");
        send("clients");
        send("rooms");
        send("variables");
        send("functions");
        send("modules");
        send("reconnect");
        send("stop");
    }
    else if (multiline) mlBuff += msg + '\n';
    else QMetaObject::invokeMethod(&printer, "msgToRoom", Q_ARG(QString, msg));
    text.appendPlainText(niceName(origin)+": "+msg+"\n");
    if(isHidden() && !origin.isEmpty()) emit notify(channel,msg);
}
void Channel::send() { emit send(channel,lineEdit.text()); }
void Channel::send(QString msg) { emit send(channel,msg); }

void Channel::currentRoomChanged(const QString &currentRoom)
{
    currentnick = "jarc[" + currentRoom + "]";
    send_cmd(QString("NICK %1").arg(currentnick));
}
void Channel::highlight(QString name) { lineEdit.setText(name+", "); }

Network::Network(QUrl url,Client* client) : client(client) {
    QTimer *unflood = new QTimer(this);
    unflood->start(1000);
    connect(unflood, SIGNAL(timeout()), SLOT(actual_send()));
    connect(this,SIGNAL(readyRead()),SLOT(receive()));
    connectToHost(url.host(),url.port(6667));
    user=url.userName();
    send(QString("NICK %1").arg(user));
    send(QString("USER %1 h s :%2").arg(user).arg(user));
    foreach(QString name, url.path().mid(1).split(",") ) {
        send(QString("JOIN #%1").arg(name)); getChannel("#"+name);
    }
}
Network::~Network() { send("QUIT :"+QTime::currentTime().toString("HH:mm")); }
Channel* Network::getChannel(QString name) {
    if(channels.contains(name)) return channels[name];
    Channel* channel = channels[name] = client->newChannel(name);
    connect(channel,SIGNAL(send(QString,QString)),SLOT(send(QString,QString)));
    connect(channel, SIGNAL(send_cmd(QString)), SLOT(send(QString)));
    connect(channel, SIGNAL(stopTransmission()), SLOT(stopTransmission()));
    return channel;
}
void Network::receive() {
    for(QByteArray msg; !(msg=readLine()).isEmpty(); ) {
        //qDebug() << msg;
        if(msg.startsWith("PING")) { send("PONG"); continue; }
        msg.chop(2); msg.remove(0,1); int last=msg.indexOf(" :");
        QList<QByteArray> params = last<0 ? msg.split(' ') : msg.mid(0,last).split(' ') << msg.mid(last+2);
        if(params.length()<3) continue;
        QString origin = params[0].split('!')[0];
        QString cmd = params[1];
        QString target = params[2];
        if(cmd=="PRIVMSG") {
            if(params[3]=="\01VERSION\01") send("PRIVMSG "+origin+" :"+"\01VERSION Jarvis.ircF\01");
            else if(target==user) getChannel(origin)->addMessage(origin,params[3]);
            else getChannel(target)->addMessage(origin,params[3]);
        }
        else if(cmd=="JOIN") getChannel(target)->addUser(origin);
        //else if(cmd=="PART"||cmd=="QUIT") getChannel(target)->removeUser(origin);
        else if(cmd=="332") getChannel(params[3])->addMessage("Topic",params[4]);
        else if(cmd=="353") foreach(QString user,params[5].split(' ')) getChannel(params[4])->addUser(user);
    }
}
void Network::send(QString cmd) { write((cmd+"\r\n").toUtf8()); }
void Network::send(QString target,QString msg) {
    //getChannel(target)->addMessage(user,msg);
    for (auto &line : msg.split('\n')) {
        while (! line.isEmpty()) {
            queue.emplace("PRIVMSG "+target+" :"+line.left(400));
            line.remove(0, 400);
        }
    }
}


void Client::broadcast(QString msg)
{
    QStringList lines(msg.split('\n'));
    std::string debug = msg.toStdString();
    for (const auto &line : lines)
        for (Channel *c : actual_channels) c->send(line);
}
