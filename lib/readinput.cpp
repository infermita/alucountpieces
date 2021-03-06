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
#include <QSettings>

ReadInput::ReadInput()
{



}
void ReadInput::run(){

    int dl = 50,i,read=0,input;
    QString lblCnt;
    QString ip,url,mac;
    QString resp;
    bool ipcheck = true;
    QString lcd,repeat = " ";
    QJsonParseError *error = new QJsonParseError();
    QJsonDocument d;
    //int man = 0;


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

        sleep(5);

        url = "/default/json/updatest/mac/"+mac+"/ip/"+ip;
        Costant::http.Get(url);

        url = "/default/json/getdb/";

        resp = Costant::http.Get(url);;

        d = QJsonDocument::fromJson(resp.toUtf8(),error);

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
            Costant::wLcd->write(0,0,"Attesa badge    ");
        }

        QSettings settings("/etc/alucount/conf.ini", QSettings::IniFormat);
        settings.beginGroup("nfc");

        if(settings.value("moldid").toString()!="0" && settings.value("workerid").toString()!="0"){

            Costant::workers = settings.value("worker").toString();
            Costant::nfcIdW = settings.value("workerid").toString();
            Costant::molds = settings.value("mold").toString();
            Costant::nfcIdM = settings.value("moldid").toString();

            if(settings.value("maintenance",0).toInt()==0){

                lcd = "O:"+Costant::workers;
                lcd = lcd+repeat.repeated(16 - lcd.length());
                Costant::wLcd->write(0,0,lcd.toUtf8().data());

                lcd = "S:"+Costant::molds;
                lcd = lcd+repeat.repeated(16 - lcd.length());
                Costant::wLcd->write(0,1,lcd.replace("\\","/").toUtf8().data());

                digitalWrite (Costant::led2(), LOW);
                digitalWrite (Costant::led1(), HIGH);
                //Costant::viewDet->start(2000);
                emit StartTimer();
            }else{
                Costant::wLcd->clear();
                lcd = "IN MANUTENZIONE ";
                Costant::wLcd->write(0,0,lcd.toUtf8().data());
                Costant::maintenance = true;
                digitalWrite (Costant::led1(), LOW);
                digitalWrite (Costant::led2(), HIGH);
                digitalWrite (Costant::plc(), HIGH);
            }


        }



    }else if(foot=="dx"){
        input = Costant::indx();
    }else if(foot=="cnt"){
        input = Costant::cnt();
    }else{
        input = Costant::sct();
    }

    pullUpDnControl(input,PUD_DOWN);

    while(1){

        //if(QString(getenv("USER"))!="alberto"){

            if(digitalRead(input)){

                if(Costant::nfcIdW!="" && Costant::nfcIdM!="" && Costant::maintenance==false){

                    //man = 0;

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

                        resp = Costant::http.Get(url);

                        qDebug() << "Ricevo: " << resp;

                        if(resp=="STOP" || resp=="NOCAL"){

                            digitalWrite (Costant::plc(), HIGH) ;

                            QSettings settings("/etc/alucount/conf.ini", QSettings::IniFormat);
                            settings.beginGroup("nfc");
                            settings.setValue("worker","0");
                            settings.setValue("workerid","0");
                            settings.setValue("mold","0");
                            settings.setValue("moldid","0");
                            settings.sync();

                            Costant::wLcd->clear();

                            lcd = "FINE PRODUZIONE ";

                            if(resp=="NOCAL")
                                lcd = "CARICARE ORDINE ";

                            Costant::wLcd->write(0,0,lcd.toUtf8().data());

                            //if(!viewStop->isActive())
                            Costant::vieStop =  1;
                            qDebug() << "Parte timer: ";
                        }else{

                            Costant::vieStop = -1;

                            d = QJsonDocument::fromJson(resp.toUtf8(),error);

                            if(error->error==QJsonParseError::NoError){

                                if(d.object().contains("status")){

                                    if(d.object().value("status").toString()=="OK"){

                                        Costant::totSx = d.object().value("totsx").toString();
                                        Costant::totDx = d.object().value("totdx").toString();
                                        Costant::total = d.object().value("total").toString();

                                    }

                                }

                            }

                        }
                        usleep(250000);
                    }
                }else if(Costant::maintenance){

                    qDebug() << "In manutenzione";


                }else{

                    Costant::wLcd->clear();
                    lcd = "MANCA BADGE     ";

                    Costant::wLcd->write(0,0,lcd.toUtf8().data());

                    lcd = "OPERATORE       ";

                    if(Costant::nfcIdW!=""){
                        lcd = "STAMPO          ";
                    }
                    Costant::wLcd->write(0,1,lcd.toUtf8().data());
                    //digitalWrite (Costant::led2(), HIGH) ; delay (dl) ;
                    //digitalWrite (Costant::led2(), LOW);delay (dl) ;
                    //read = 0;
                    //digitalWrite (Costant::led1(), HIGH);
                    digitalWrite (Costant::plc(), HIGH);
                }

            }else{
                read = 0;
            }

        //}
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

