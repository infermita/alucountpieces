#include "jsoncommand.h"

#include <QProcess>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QHash>
#include <QCryptographicHash>
#include <QSettings>

JsonCommand::JsonCommand()
{
    options << "halt" << "reboot" << "restart";

}
void JsonCommand::ElaborateCommand(QTcpSocket *socket){

    QProcess p;
    QByteArray buffer;
    quint16 packet_size;
    int rcv = 0;


    while(1){

        socket->waitForReadyRead(1000);

        packet_size=socket->bytesAvailable();//Read Count Bytes waiting on line
        //qDebug()<<"Packet Recived len: "<<QString::number(packet_size)<<"\n";
        //rcv += packet_size;


        if (packet_size>0)
        {

            buffer.append(socket->read(packet_size));//Read Packet
            //buffer.append(socket->readAll());
            //...
         }else{
            break;
        }
    }
    //qDebug()<<"Packet Recived len: "<<QString::number(rcv)<<"\n";

    QJsonParseError *error = new QJsonParseError();
    QJsonDocument d = QJsonDocument::fromJson(QString(buffer).toUtf8(),error);
    QJsonObject jObj = d.object();
    QString cmd = jObj["cmd"].toString();

    if(cmd!="alive")
        qDebug() << "Leggo da socket: " << QString(buffer).mid(0,200);

    //qDebug() << "Leggo Json: " << cmd;

    switch(options.indexOf(cmd)){

        case 0:
            //System halt
            SendResponse(socket,"ok "+cmd);
            p.start("halt");
            p.waitForFinished();
            break;
        case 1:
            //System reboot
            SendResponse(socket,"ok "+cmd);
            p.start("reboot");
            p.waitForFinished();
            break;
        case 2:
            //restart
            SendResponse(socket,"ok "+cmd);
            p.startDetached("/etc/init.d/alucount restart");
            break;
        default:
            SendResponse(socket,"error "+cmd);
            break;

    }


}
void JsonCommand::SendResponse(QTcpSocket *socket,QString response,QString addP){

    if(addP!=""){

        response = "{\"cmd\":\""+response+"\""+addP+"}";

    }else{

        response = "{\"cmd\":\""+response+"\"}";

    }

    socket->write(response.toUtf8());
    socket->flush();
    socket->waitForBytesWritten(3000);

    socket->close();

}


