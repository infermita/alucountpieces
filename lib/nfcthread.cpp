#include "nfcthread.h"
#include "lib/costant.h"
#include <wiringPi.h>
#include <QDebug>
#include "lib/writelcd.h"
#include "lib/httpclient.h"
#include <QSettings>
#include <QNetworkInterface>

NfcThread::NfcThread()
{

    goTimer = -1;
    viewDet = new QTimer();
    connect(viewDet, SIGNAL(timeout()),
              this, SLOT(ViewDetTimer()),Qt::DirectConnection);
    viewDet->start(2000);


}
void NfcThread::run(){

    int i,pin1,pin2;
    int dl = 250;
    QString id,url;
    QHash<QString, QString> resQ;
    QString mac;
    mid = 0;

    repeat = " ";
    viewDetCnt = 0;

    const nfc_modulation nmMifare = {
        .nmt = NMT_ISO14443A,
        .nbr = NBR_106,
      };

    if(QString(getenv("USER"))!="alberto"){
        digitalWrite (Costant::led2(), HIGH) ;
        Costant::wLcd->clear();
        lcd = "Attesa rete";
        lcd = lcd+repeat.repeated(16 - lcd.length());
        Costant::wLcd->write(0,0,lcd.toUtf8().data());
    }

    while(1){

        if(QString(getenv("USER"))!="alberto"){

            nfc_init(&context);
            if (context == NULL) {
                qDebug() << "Unable to init libnfc (malloc)";

            }else{

                qDebug() << "Context inizializzato";

                pnd = nfc_open(context, NULL);
                if (pnd == NULL) {
                    qDebug() << "ERROR: %s. Unable to open NFC device.";

                }else{

                    qDebug() << "Nfc aperto";

                    if (nfc_initiator_init(pnd) < 0) {
                        qDebug() << "nfc_initiator_init";
                        nfc_close(pnd);
                        nfc_exit(context);

                    }else{

                        qDebug() << "Nfc iniator";
                        if(nfc_initiator_select_passive_target(pnd, nmMifare, NULL, 0, &nt) > 0){


                            if(viewDet->isActive()){
                                qDebug() << "Stop timer viewdet";
                                //viewDet->stop();
                                goTimer = 0;
                            }

                            Costant::vieStop = -1;

                            qDebug() << "Nfc letto";

                            pin1 = digitalRead(Costant::led1());
                            pin2 = digitalRead(Costant::led2());

                            id = "";
                            for(i = 0; i < nt.nti.nai.szUidLen;i++){

                                QString hex;
                                id += hex.sprintf("%02x",nt.nti.nai.abtUid[i]).toUpper();

                            }
                            qDebug() << "Leggo: " << id;

                            digitalWrite (Costant::led2(), LOW);
                            digitalWrite (Costant::led1(), LOW);

                            if(Costant::config){

                                url = "/default/json/newcard/key/"+id+"/sessionid/1";
                                Costant::http.Get(url);

                                for(i=0; i < 2;i++){

                                    digitalWrite (Costant::led1(), HIGH) ; delay (100) ;
                                    digitalWrite (Costant::led1(), LOW);delay (100) ;


                                }
                                digitalWrite (Costant::led2(), pin2);
                                digitalWrite (Costant::led1(), pin1);

                            }else{

                                resQ.clear();
                                resQ = Costant::dao.singleRow("cards","cardkey='"+id+"'");

                                if(resQ.count()){

                                    for(i=0; i < 2;i++){

                                        digitalWrite (Costant::led1(), HIGH) ; delay (100) ;
                                        digitalWrite (Costant::led1(), LOW);delay (100) ;


                                    }
                                    digitalWrite (Costant::led2(), pin2);
                                    digitalWrite (Costant::led1(), pin1);

                                    if(resQ.value("table")=="workers" || (resQ.value("table")=="masterkeys" && resQ.value("value")=="operaio")){

                                        if(resQ.value("value")!=Costant::workers){

                                            Costant::workers = resQ.value("value");
                                            lcd = "O:"+Costant::workers;
                                            lcd = lcd+repeat.repeated(16 - lcd.length());
                                            Costant::wLcd->write(0,0,lcd.toUtf8().data());
                                            Costant::pCount = 0;
                                            Costant::nfcIdW = id;

                                        }else if(Costant::maintenance){

                                            Costant::maintenance = false;
                                            digitalWrite (Costant::led2(), LOW);
                                            digitalWrite (Costant::led1(), HIGH);

                                            Costant::wLcd->clear();

                                            lcd = "O:"+Costant::workers;
                                            lcd = lcd+repeat.repeated(16 - lcd.length());
                                            Costant::wLcd->write(0,0,lcd.toUtf8().data());

                                            lcd = "S:"+Costant::molds;
                                            lcd = lcd+repeat.repeated(16 - lcd.length());
                                            Costant::wLcd->write(0,1,lcd.toUtf8().data());

                                            mac = QNetworkInterface::interfaceFromName("wlan0").hardwareAddress();
                                            url = "/default/json/maintenance/mac/"+mac+"/cardkeyw/"+Costant::nfcIdW+"/cardkeym/"+Costant::nfcIdM+"/cardman/"+id+"/closed/1";
                                            Costant::http.Get(url);

                                        }else{

                                            Costant::workers = "";
                                            Costant::nfcIdW = "";
                                            digitalWrite (Costant::led1(), LOW);
                                            digitalWrite (Costant::led2(), HIGH);
                                            lcd = "FINE TURNO";
                                            lcd = lcd+repeat.repeated(16 - lcd.length());
                                            Costant::wLcd->write(0,0,lcd.toUtf8().data());
                                        }

                                    }
                                    if(resQ.value("table")=="molds" || (resQ.value("table")=="masterkeys" && resQ.value("value")=="stampo")){

                                        if(resQ.value("value")!=Costant::molds){
                                            Costant::molds = resQ.value("value");

                                            lcd = "S:"+Costant::molds;
                                            lcd = lcd+repeat.repeated(16 - lcd.length());

                                            Costant::wLcd->write(0,1,lcd.replace("\\","/").toUtf8().data());
                                            Costant::pCount = 0;
                                            Costant::nfcIdM = id;
                                        }

                                    }
                                    if(resQ.value("table")=="masterkeys" && resQ.value("value").compare("manutenzione")>=0){


                                                Costant::wLcd->clear();

                                                QStringList man = resQ.value("value").split("|");

                                                lcd = "IN MANUTENZIONE";
                                                lcd = lcd+repeat.repeated(16 - lcd.length());

                                                Costant::wLcd->write(0,0,lcd.toUtf8().data());

                                                if(man.length()> 1){

                                                    mid = 15;
                                                    lcd = man.at(1);
                                                    lcd = lcd = repeat.repeated(15)+lcd+"."+repeat.repeated(15);
                                                    Costant::wLcd->write(0,1,lcd.mid(mid,16).replace("\\","/").toUtf8().data());
                                                }

                                                Costant::maintenance = true;
                                                mac = QNetworkInterface::interfaceFromName("wlan0").hardwareAddress();
                                                url = "/default/json/maintenance/mac/"+mac+"/cardkeyw/"+Costant::nfcIdW+"/cardkeym/"+Costant::nfcIdM+"/cardman/"+id+"/closed/0";
                                                Costant::http.Get(url);
                                                digitalWrite (Costant::led1(), LOW);
                                                digitalWrite (Costant::led2(), HIGH);
                                                goTimer = 2;
                                                QSettings settings("/etc/alucount/conf.ini", QSettings::IniFormat);
                                                settings.beginGroup("nfc");
                                                settings.setValue("maintenance",1);
                                                settings.sync();



                                        }

                                }else{

                                    for(i=0; i < 2;i++){

                                        digitalWrite (Costant::led2(), HIGH) ; delay (100) ;
                                        digitalWrite (Costant::led2(), LOW);delay (100) ;


                                    }
                                    digitalWrite (Costant::led2(), pin2);
                                    digitalWrite (Costant::led1(), pin1);

                                }

                                if(Costant::nfcIdM!="" && Costant::nfcIdW!="" && Costant::maintenance==false){

                                    Costant::wLcd->clear();
                                    lcd = "O:"+Costant::workers;
                                    lcd = lcd+repeat.repeated(16 - lcd.length());
                                    Costant::wLcd->write(0,0,lcd.toUtf8().data());

                                    lcd = "S:"+Costant::molds;
                                    lcd = lcd+repeat.repeated(16 - lcd.length());
                                    Costant::wLcd->write(0,1,lcd.replace("\\","/").toUtf8().data());

                                    digitalWrite (Costant::led2(), LOW);
                                    digitalWrite (Costant::led1(), HIGH);
                                    digitalWrite (Costant::plc(), LOW);
                                    QSettings settings("/etc/alucount/conf.ini", QSettings::IniFormat);
                                    settings.beginGroup("nfc");
                                    settings.setValue("worker",Costant::workers);
                                    settings.setValue("workerid",Costant::nfcIdW);
                                    settings.setValue("mold",Costant::molds);
                                    settings.setValue("moldid",Costant::nfcIdM);
                                    settings.setValue("maintenance",0);
                                    settings.sync();
                                    StartTimer();

                                }
                            }

                            while(!nfc_initiator_target_is_present(pnd,&nt)){
                                sleep(1);
                            }
                            nfc_close(pnd);
                            nfc_exit(context);



                        }

                    }

                }
            }

        }
        sleep(1);

    }

}
void NfcThread::StartTimer(){

    goTimer = 1;
    qDebug() << "Start timer viewdet";

}
void NfcThread::ViewDetTimer(){

    if(goTimer==1){

        if(viewDet->interval()!=2000)
            viewDet->setInterval(2000);

        if(viewDetCnt>=4)
            viewDetCnt = 0;

        switch(viewDetCnt){

            case 0:
                lcd = "S:"+Costant::molds;
                lcd = lcd+repeat.repeated(16 - lcd.length());
                viewDetCnt++;
                break;
            case 1:
                lcd = "SX:"+Costant::totSx;
                lcd = lcd+repeat.repeated(16 - lcd.length());
                viewDetCnt++;
                break;
            case 2:
                lcd = "DX:"+Costant::totDx;
                lcd = lcd+repeat.repeated(16 - lcd.length());
                viewDetCnt++;
                break;
            case 3:
                lcd = "TOT:"+Costant::total;
                lcd = lcd+repeat.repeated(16 - lcd.length());
                viewDetCnt++;
                break;
            default:
                break;

        }
        //qDebug() << "Timer scrivo " << lcd;

        Costant::wLcd->write(0,1,lcd.replace("\\","/").toUtf8().data());

    }else if(goTimer==2){

        if(viewDet->interval()!=500)
            viewDet->setInterval(500);

        if(lcd.mid(mid,1) == "."){
            mid= 0;

        }

        Costant::wLcd->write(0,1,lcd.mid(mid,16).replace("\\","/").toUtf8().data());
        mid++;

    }else if(goTimer==0){
        lcd = "S:"+Costant::molds;
        lcd = lcd+repeat.repeated(16 - lcd.length());
        Costant::wLcd->write(0,1,lcd.replace("\\","/").toUtf8().data());
        goTimer = -1;
    }



}
