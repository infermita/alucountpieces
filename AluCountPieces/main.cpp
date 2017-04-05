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

QString Costant::nfcId = "";

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

    wiringPiSetup();
    pinMode (Costant::led1(), OUTPUT) ;
    pinMode (Costant::led2(), OUTPUT) ;
    pinMode (Costant::in(), INPUT) ;
    NfcThread nfcTh;
    nfcTh.start();
    ReadInput rd;
    rd.start();
    return a.exec();
}
