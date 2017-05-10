#include "httpclient.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>
#include <QEventLoop>
#include <QNetworkReply>
#include <QDebug>
#include "lib/costant.h"

HttpClient::HttpClient()
{
    lock = false;

}
QString HttpClient::Get(QString url){

    QNetworkAccessManager manager;
    QEventLoop loop;
    QNetworkReply *rep;
    QString urlDef;

    if(lock==false){

        lock = true;

        urlDef = "http://alucount.al.it"+url;

        QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)),&loop, SLOT(quit()));
        rep = manager.get(QNetworkRequest(QUrl(urlDef)));

        qDebug() << "HttpC Chiamo url: " << urlDef;

        loop.exec();

        QVariant statusCode = rep->attribute( QNetworkRequest::HttpStatusCodeAttribute );

        qDebug() << "HttpC risposta: " << statusCode.toString();

        lock = false;

        if(statusCode.toInt()==200){

            return rep->readAll();

        }else{
            return "";
        }

    }else{
        Costant::http.Get(url);
    }

}
