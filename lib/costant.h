#ifndef COSTANT_H
#define COSTANT_H
#include <QString>


class Costant
{
public:
    Costant();
    static int led1() { return  4;}
    static int led2() { return  5;}
    static int in()   { return  6;}
    static QString nfcId;
};

#endif // COSTANT_H
