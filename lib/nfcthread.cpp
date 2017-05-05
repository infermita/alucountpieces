#include "nfcthread.h"
#include "lib/costant.h"
#include <wiringPi.h>
#include <QDebug>
#include "lib/writelcd.h"

NfcThread::NfcThread()
{

}
void NfcThread::run(){

    int i;
    int dl = 250;
    QString id;
    QHash<QString, QString> resQ;
    const nfc_modulation nmMifare = {
        .nmt = NMT_ISO14443A,
        .nbr = NBR_106,
      };
    if(QString(getenv("USER"))!="alberto"){
        Costant::wLcd->clear();
        Costant::wLcd->write(0,0,"Attesa badge");
        digitalWrite (Costant::led2(), HIGH) ;
    }

    sleep(2);
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

                                id += QString::number(nt.nti.nai.abtUid[i],16);

                            }
                            qDebug() << "Leggo: " << id;
                            resQ.clear();
                            resQ = Costant::dao.singleRow("cards","cardkey='"+id+"'");

                            if(resQ.count()){

                                if(resQ.value("table")=="workers"){

                                    if(resQ.value("value")!=Costant::workers){

                                        Costant::workers = resQ.value("value");
                                        Costant::wLcd->write(0,0,QString("O:"+Costant::workers).toUtf8().data());
                                        Costant::pCount = 0;
                                        Costant::nfcIdW = id;
                                    }

                                }
                                if(resQ.value("table")=="molds"){

                                    if(resQ.value("value")!=Costant::molds){
                                        Costant::molds = resQ.value("value");
                                        Costant::wLcd->write(0,1,QString("S:"+Costant::molds).toUtf8().data());
                                        Costant::pCount = 0;
                                        Costant::nfcIdM = id;
                                    }

                                }
                                if(resQ.value("table")=="masterkeys"){

                                    if(resQ.value("value")=="operatio"){

                                        if(resQ.value("value")!=Costant::workers){

                                            Costant::workers = resQ.value("value");
                                            Costant::wLcd->write(0,0,QString("O:"+Costant::workers).toUtf8().data());
                                            Costant::pCount = 0;
                                            Costant::nfcIdW = id;
                                        }

                                    }
                                    if(resQ.value("value")=="operatio"){

                                        if(resQ.value("value")!=Costant::molds){

                                            Costant::molds = resQ.value("value");
                                            Costant::wLcd->write(0,1,QString("S:"+Costant::molds).toUtf8().data());
                                            Costant::pCount = 0;
                                            Costant::nfcIdM = id;
                                        }

                                    }

                                }

                            }

                            if(Costant::nfcIdM!="" && Costant::nfcIdW!=""){
                                digitalWrite (Costant::led2(), LOW);
                                digitalWrite (Costant::led1(), HIGH);
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
