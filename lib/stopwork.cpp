#include "stopwork.h"
#include <QDebug>
#include <wiringPi.h>
#include "lib/costant.h"

Stopwork::Stopwork()
{

}
void Stopwork::run(){

    while(1){

        //qDebug() << "Ciclo timer stop:" << Costant::vieStop;

        if(Costant::vieStop==0){
            Costant::vieStop = 1;
            digitalWrite (Costant::led2(), HIGH);
            digitalWrite (Costant::led1(), LOW);
        }else if(Costant::vieStop==1){
            Costant::vieStop = 0;
            digitalWrite (Costant::led1(), HIGH);
            digitalWrite (Costant::led2(), LOW);
        }


        sleep(1);

    }

}
