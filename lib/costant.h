#ifndef COSTANT_H
#define COSTANT_H
#include <QString>
#include "lib/writelcd.h"
#include "lib/dao.h"

class Costant
{
public:
    Costant();
    static int led1() { return  4;}
    static int led2() { return  5;}
    static int in1()   { return  0;}
    static int in2()   { return  2;}
    static QString nfcIdW;
    static QString nfcIdM;
    static WriteLcd *wLcd;
    static int pCount;
    static Dao dao;
    static QString workers;
    static QString molds;
    static bool config;
    //static int lcdAddr;
};

#endif // COSTANT_H
