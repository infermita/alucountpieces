#include "readinput.h"
#include "wiringPi.h"
#include "lib/costant.h"
#include <QDebug>

ReadInput::ReadInput()
{

}
void ReadInput::run(){

    int dl = 50,i,cnt = 0,read=0;

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
                    cnt++;
                    qDebug() << "Conto: " << cnt;
                    read = 1;
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
