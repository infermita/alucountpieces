#include "nfcthread.h"
#include "lib/costant.h"
#include <wiringPi.h>
#include <QDebug>
#include "lib/writelcd.h"
#include "lib/httpclient.h"

NfcThread::NfcThread()
{

}
void NfcThread::run(){

    int i;
    int dl = 250;
    QString id,url;
    QHash<QString, QString> resQ;
    QString lcd,repeat = " ";

    const nfc_modulation nmMifare = {
        .nmt = NMT_ISO14443A,
        .nbr = NBR_106,
      };

    if(QString(getenv("USER"))!="alberto"){
        digitalWrite (Costant::led2(), HIGH) ;
        Costant::wLcd->clear();
        Costant::wLcd->write(0,0,"Attesa rete");
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

                            qDebug() << "Nfc letto";
                            id = "";
                            for(i = 0; i < nt.nti.nai.szUidLen;i++){

                                QString hex;
                                id += hex.sprintf("%02x",nt.nti.nai.abtUid[i]).toUpper();

                            }
                            qDebug() << "Leggo: " << id;

                            if(Costant::config){

                                url = "/default/json/newcard/key/"+id;
                                Costant::http.Get(url);

                            }else{

                                resQ.clear();
                                resQ = Costant::dao.singleRow("cards","cardkey='"+id+"'");

                                if(resQ.count()){

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

                                            lcd = "O:"+Costant::workers;
                                            lcd = lcd+repeat.repeated(16 - lcd.length());
                                            Costant::wLcd->write(0,0,lcd.toUtf8().data());

                                            lcd = "S:"+Costant::molds;
                                            lcd = lcd+repeat.repeated(16 - lcd.length());
                                            Costant::wLcd->write(0,1,lcd.toUtf8().data());


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
                                    if(resQ.value("table")=="masterkeys" && resQ.value("value")=="manutenzione"){
                                        /*
                                        if(resQ.value("value")=="operaio"){

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

                                                lcd = "O:"+Costant::workers;
                                                lcd = lcd+repeat.repeated(16 - lcd.length());
                                                Costant::wLcd->write(0,0,lcd.toUtf8().data());

                                                lcd = "S:"+Costant::molds;
                                                lcd = lcd+repeat.repeated(16 - lcd.length());
                                                Costant::wLcd->write(0,1,lcd.toUtf8().data());


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
                                        if(resQ.value("value")=="stampo"){

                                            if(resQ.value("value")!=Costant::molds){

                                                Costant::molds = resQ.value("value");

                                                lcd = "S:"+Costant::molds;
                                                lcd = lcd+repeat.repeated(16 - lcd.length());

                                                Costant::wLcd->write(0,1,lcd.toUtf8().data());
                                                Costant::pCount = 0;
                                                Costant::nfcIdM = id;
                                            }

                                        }
                                        */
                                        //if(resQ.value("value")=="manutenzione"){

                                            Costant::wLcd->clear();

                                            lcd = "IN MANUTENZIONE";

                                            Costant::wLcd->write(0,0,lcd.toUtf8().data());

                                            Costant::maintenance = true;
                                            url = "/default/json/maintenance/cardkeyw/"+Costant::nfcIdW+"/cardkeym/"+Costant::nfcIdM+"/cardman/"+id;
                                            Costant::http.Get(url);
                                            digitalWrite (Costant::led1(), LOW);
                                            digitalWrite (Costant::led2(), HIGH);

                                        //}

                                    }

                                }

                                if(Costant::nfcIdM!="" && Costant::nfcIdW!="" && Costant::maintenance==false){
                                    digitalWrite (Costant::led2(), LOW);
                                    digitalWrite (Costant::led1(), HIGH);
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
            sleep(1);
        }

    }

}
