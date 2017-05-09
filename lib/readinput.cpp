#include "readinput.h"
#include "wiringPi.h"
#include "lib/costant.h"
#include <QDebug>
#include <QNetworkInterface>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include "lib/dao.h"
#include "lib/httpclient.h"

ReadInput::ReadInput()
{

}
void ReadInput::run(){

    int dl = 50,i,read=0,input;
    QString lblCnt;
    QString ip,url,mac;
    QString resp;
    HttpClient http;
    bool ipcheck = true;


    if(QString(getenv("USER"))!="alberto"){

        Costant::wLcd->clear();
        Costant::wLcd->write(0,0,"Attesa rete");
    }

    sleep(5);

    mac = QNetworkInterface::interfaceFromName("wlan0").hardwareAddress();

    if(foot=="sx"){

        input = Costant::insx();

        while(ipcheck){

            ip = QNetworkInterface::interfaceFromName("wlan0").addressEntries().first().ip().toString();

            qDebug() << "Attesa rete con ip: " << ip << " count: " << QString::number(ip.split(".").count());

            if(ip.split(".").count()==4)
                ipcheck = false;
            else
                sleep(1);
        }

        url = "/default/json/updatest/mac/"+mac+"/ip/"+ip;
        http.Get(url);

        url = "/default/json/getdb/";

        resp = http.Get(url);;

        QJsonParseError *error = new QJsonParseError();
        QJsonDocument d = QJsonDocument::fromJson(resp.toUtf8(),error);

        if(error->error==QJsonParseError::NoError){

            Costant::dao.deleteRow("cards","1");

            QJsonObject jObj = d.object();
            QJsonArray listUsers;
            QHash<QString,QString> field;

            listUsers = jObj["list"].toArray();
            foreach (const QJsonValue & users, listUsers) {

                field.clear();

                QJsonObject o = users.toObject();
                qDebug() << "Id user: " << o["id"].toString();
                field.insert("id",o["id"].toString());
                field.insert("cardkey",o["cardkey"].toString());
                field.insert("`table`",o["table"].toString());
                field.insert("value",o["value"].toString());

                if(Costant::dao.replaceRow("cards",field)){
                    qDebug() << "Ok insert: " << o["id"].toString();
                }else{
                    qDebug() << "Error insert: " << o["id"].toString();
                }

            }
        }

        if(QString(getenv("USER"))!="alberto"){

            Costant::wLcd->clear();
            Costant::wLcd->write(0,0,"Attesa badge");
        }
    }else{
        input = Costant::indx();
    }

    while(1){

        if(QString(getenv("USER"))!="alberto"){

            if(digitalRead(input)){

                if(Costant::nfcIdW!="" && Costant::nfcIdM!="" && Costant::maintenance==false){

                    if(read==0){

                        for(i=0; i < 2;i++){

                            digitalWrite (Costant::led1(), HIGH) ; delay (dl) ;
                            digitalWrite (Costant::led1(), LOW);
                            digitalWrite (Costant::led2(), HIGH) ; delay (dl) ;
                            digitalWrite (Costant::led2(), LOW);


                        }
                        digitalWrite (Costant::led1(), HIGH);
                        //Costant::pCount++;
                        //qDebug() << "Conto: " << Costant::pCount;
                        read = 1;
                        /*
                        lblCnt = "Pezzi: "+QString::number(Costant::pCount);
                        Costant::wLcd->write(0,1,lblCnt.toLatin1().data());
                        */
                        url = "/default/json/index";
                        url += "/mac/"+mac;
                        url += "/cardkeyw/"+Costant::nfcIdW;
                        url += "/cardkeym/"+Costant::nfcIdM;
                        url += "/worker/"+Costant::workers;
                        url += "/mold/"+Costant::molds;
                        //url += "/pezzi/"+QString::number(Costant::pCount);
                        url += "/foot/"+foot;

                        http.Get(url);
                    }
                }/*else{

                    digitalWrite (Costant::led2(), HIGH) ; delay (dl) ;
                    digitalWrite (Costant::led2(), LOW);delay (dl) ;
                    read = 0;
                    digitalWrite (Costant::led1(), HIGH);
                }*/

            }else{
                read = 0;
            }

        }
        usleep(1000);

    }

}
void ReadInput::replyFinished (QNetworkReply *reply)
{
    if(reply->error())
    {
        qDebug() << "ERROR!";
        qDebug() << reply->errorString();
    }
    else
    {
        qDebug() << reply->readAll();

    }

    reply->deleteLater();
}
