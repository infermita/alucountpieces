#ifndef COSTANT_H
#define COSTANT_H
#include <QString>
#include "lib/writelcd.h"
#include "lib/dao.h"
#include "lib/httpclient.h"

class Costant
{
public:
    Costant();
    static int led1() { return  4;}
    static int led2() { return  5;}
    static int insx()   { return  0;}
    static int indx()   { return  2;}
    static QString nfcIdW;
    static QString nfcIdM;
    static WriteLcd *wLcd;
    static int pCount;
    static Dao dao;
    static QString workers;
    static QString molds;
    static bool config;
    static bool maintenance;
    static HttpClient http;
    //static int lcdAddr;
};

#endif // COSTANT_H
