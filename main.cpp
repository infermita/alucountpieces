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
#include <QFileInfoList>
#include <QDir>

QString Costant::nfcIdW = "";
QString Costant::nfcIdM = "";
QString Costant::workers = "";
QString Costant::molds = "";
bool Costant::config = false;
bool Costant::maintenance = false;
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
    QString pathLog = "/var/log/";
    if(QString(getenv("USER"))=="alberto"){

        pathLog = "";

    }

    outFile.setFileName(pathLog+"alucount.log");

    outFile.open(QIODevice::WriteOnly | QIODevice::Append);

    if(outFile.size()>15728640){

        QDir dir(pathLog);
        QStringList filter;
        filter << "alucount.*.log";
        dir.setNameFilters(filter);

        QFileInfoList listFs = dir.entryInfoList(QDir::Files,QDir::Time);

        if(listFs.size()>5){
            for(int i = 5; i < listFs.size();i++){

                QFile::remove(listFs.at(i).absoluteFilePath());

            }
        }

        outFile.rename(pathLog+"alucount.log",pathLog+"alucount."+QDateTime::currentDateTime().toString("dd-MM-yy_hh:mm")+".log");
        outFile.close();
        outFile.setFileName(pathLog+"alucount.log");
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);

    }


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
        pinMode (Costant::insx(), INPUT) ;
        pinMode (Costant::indx(), INPUT) ;

    }

    SocketServer server;

    NfcThread nfcTh;
    nfcTh.start();
    ReadInput rdSX;
    rdSX.foot = "sx";
    rdSX.start();

    ReadInput rdDX;
    rdDX.foot = "dx";
    rdDX.start();

    return a.exec();
}
