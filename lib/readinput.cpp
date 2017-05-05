#include "readinput.h"
#include "wiringPi.h"
#include "lib/costant.h"
#include <QDebug>
#include <QNetworkInterface>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>
#include <QEventLoop>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include "lib/dao.h"

ReadInput::ReadInput()
{

}
void ReadInput::run(){

    int dl = 50,i,read=0;
    QString lblCnt;
    QString ip,url,mac;
    QNetworkAccessManager manager;
    QEventLoop loop;
    QNetworkReply *rep;
    QString resp;

    mac = QNetworkInterface::interfaceFromName("wlan0").hardwareAddress();
    ip = QNetworkInterface::interfaceFromName("wlan0").addressEntries().first().ip().toString();

    url = "http://alucount.al.it/default/json/updatest/mac/"+mac+"/ip/"+ip;

    connect(&manager, SIGNAL(finished(QNetworkReply*)),&loop, SLOT(quit()));
    rep = manager.get(QNetworkRequest(QUrl(url)));
    loop.exec();

    resp = rep->readAll();

    url = "http://alucount.al.it/default/json/getdb/";

    connect(&manager, SIGNAL(finished(QNetworkReply*)),&loop, SLOT(quit()));
    rep = manager.get(QNetworkRequest(QUrl(url)));
    loop.exec();

    resp = rep->readAll();

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


    while(1){

        if(QString(getenv("USER"))!="alberto"){

            if(digitalRead(Costant::in1()) || digitalRead(Costant::in2())){

                if(Costant::nfcIdW!="" && Costant::nfcIdM!=""){

                    if(read==0){

                        for(i=0; i < 2;i++){

                            digitalWrite (Costant::led1(), HIGH) ; delay (dl) ;
                            digitalWrite (Costant::led1(), LOW);
                            digitalWrite (Costant::led2(), HIGH) ; delay (dl) ;
                            digitalWrite (Costant::led2(), LOW);


                        }
                        Costant::pCount++;
                        qDebug() << "Conto: " << Costant::pCount;
                        read = 1;
                        lblCnt = "Pezzi: "+QString::number(Costant::pCount);
                        Costant::wLcd->write(0,1,lblCnt.toLatin1().data());

                        url = "http://alucount.al.it/default/json/index/";
                        url + "/mac/"+mac+"/";
                        url + "/cardkeyw/"+Costant::nfcIdW+"/";
                        url + "/cardkeym/"+Costant::nfcIdM+"/";
                        url + "/worker/"+Costant::workers+"/";
                        url + "/mold/"+Costant::molds+"/";

                        manager.get(QNetworkRequest(QUrl(url)));

                    }
                }else{

                    digitalWrite (Costant::led2(), HIGH) ; delay (dl) ;
                    digitalWrite (Costant::led2(), LOW);delay (dl) ;
                    read = 0;
                }

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
