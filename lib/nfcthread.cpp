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
    int dl = 500;
    QString id;
    const nfc_modulation nmMifare = {
        .nmt = NMT_ISO14443A,
        .nbr = NBR_106,
      };
    sleep(2);
    while(1){

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
                        if(id=="25d9b1a5"){

                            Costant::nfcId = id;

                            for(i=0; i < 3;i++){

                                digitalWrite (Costant::led1(), HIGH) ; delay (dl) ;
                                digitalWrite (Costant::led1(), LOW); delay (dl) ;

                            }
                            Costant::pCount = 0;
                            Costant::wLcd->clear();
                            Costant::wLcd->write(0,0,"Mario Rossi");
                            Costant::wLcd->write(1,0,"Pezzi: 0");
                        }else{

                            Costant::nfcId = "";

                            for(i=0; i < 3;i++){

                                digitalWrite (Costant::led2(), HIGH) ; delay (dl/4) ;
                                digitalWrite (Costant::led2(), LOW); delay (dl/4) ;

                            }
                            Costant::wLcd->clear();
                            Costant::wLcd->write(0,0,"Carta non");
                            Costant::wLcd->write(1,0,"riconosciuta");
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
