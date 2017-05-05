#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QString>
#include <QObject>

class HttpClient
{
public:
    HttpClient();
    QString Get(QString url);
};

#endif // HTTPCLIENT_H
