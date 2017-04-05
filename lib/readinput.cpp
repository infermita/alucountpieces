#include "readinput.h"
#include "wiringPi.h"
#include "lib/costant.h"
#include <QDebug>
#include <QNetworkInterface>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>

ReadInput::ReadInput()
{

}
void ReadInput::run(){

    int dl = 50,i,read=0;
    QString lblCnt;
    QString ip,url;
    QNetworkAccessManager manager;
    QNetworkRequest request;


    while(1){


        if(digitalRead(Costant::in())){

            if(Costant::nfcId!=""){

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

                    ip = QNetworkInterface::interfaceFromName("wlan0").addressEntries().first().ip().toString();

                    url = "http://alucount.al.it/default/json/index/ip/"+ip+"/cardkey/"+Costant::nfcId+"/pezzi/"+QString::number(Costant::pCount);

                    request.setUrl(QUrl(url));
                    manager.get(request);

                }
            }else{

                digitalWrite (Costant::led2(), HIGH) ; delay (dl) ;
                digitalWrite (Costant::led2(), LOW);delay (dl) ;
                read = 0;
            }

        }else{
            read = 0;
        }
        usleep(1000);

    }

}
