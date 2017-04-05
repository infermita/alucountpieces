#include "readinput.h"
#include "wiringPi.h"
#include "lib/costant.h"
#include <QDebug>
#include <QNetworkInterface>
#include <QNetworkAccessManager>

ReadInput::ReadInput()
{

}
void ReadInput::run(){

    int dl = 50,i,read=0;
    QString lblCnt;
    QString ip,url;
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
                    Costant::wLcd->write(1,0,lblCnt.toLatin1().data());

                    ip = QNetworkInterface::interfaceFromName("wlan0").addressEntries().first().ip().toString();

                    QNetworkAccessManager *manager = new QNetworkAccessManager();
                    QNetworkRequest request;

                    url = "http://alucount.al.it/defaul/json/index/ip/"+ip+"/cardkey"+Costant::nfcId+"/pezzi/"+Costant::pCount;

                    request.setUrl(QUrl(url));
                    manager->get(request);

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
