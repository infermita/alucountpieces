#include <QCoreApplication>
#include "lib/nfcthread.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include "lib/nfcthread.h"
#include <wiringPi.h>
#include "lib/costant.h"
#include "lib/readinput.h"
#include <QString>
#include "lib/writelcd.h"
#include "lib/socketserver.h"
#include "lib/dao.h"


#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

QString Costant::nfcIdW = "";
QString Costant::nfcIdM = "";
QString Costant::workers = "";
QString Costant::molds = "";
bool Costant::config = false;
//int Costant::lcdAddr = 0x27;

WriteLcd *Costant::wLcd = new WriteLcd();
Dao Costant::dao;
int Costant::pCount = 0;
void myMessageHandler(QtMsgType type, const QMessageLogContext &context,const QString &msg)
{
    QString txt = QDateTime::currentDateTime().toString("dd-MM-yy hh:mm:ss")+" ";
    switch (type) {
    case QtDebugMsg:
        txt += "Debug: " + msg;
        break;
    case QtWarningMsg:
        txt += "Warning: " + msg;
    break;
    case QtCriticalMsg:
        txt += "Critical: " + msg;
    break;
    case QtFatalMsg:
        txt += "Fatal: " + msg;
        abort();
    }
    QFile outFile;
    if(QString(getenv("USER"))=="alberto"){

        outFile.setFileName("alucount.log");

    }else{
        outFile.setFileName("/var/log/alucount.log");
    }
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qInstallMessageHandler(myMessageHandler);

    if(QString(getenv("USER"))!="alberto"){

        wiringPiSetup();
        pinMode (Costant::led1(), OUTPUT) ;
        pinMode (Costant::led2(), OUTPUT) ;
        pinMode (Costant::in1(), INPUT) ;
        pinMode (Costant::in2(), INPUT) ;

    }

    SocketServer server;

    NfcThread nfcTh;
    nfcTh.start();
    ReadInput rd;
    rd.start();

    return a.exec();
}
