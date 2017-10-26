#ifndef COSTANT_H
#define COSTANT_H
#include <QString>
#include "lib/writelcd.h"
#include "lib/dao.h"
#include "lib/httpclient.h"
#include <QTimer>

class Costant
{
public:
    Costant();
    static int led1() { return  4;}
    static int led2() { return  5;}
    static int insx()   { return  0;}
    static int indx()   { return  2;}
    static int plc()   { return  3;}
    static int cnt()   { return  12;}
    static QString totSx,totDx,total;
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
    static QString sessId;
    static QTimer *viewDet;
    //static int lcdAddr;
};

#endif // COSTANT_H
