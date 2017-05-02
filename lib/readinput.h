#ifndef READINPUT_H
#define READINPUT_H
#include <QThread>
#include <QNetworkReply>

class ReadInput : public QThread
{
    Q_OBJECT
public:
    ReadInput();
public slots:
    void replyFinished (QNetworkReply *reply);
private:
    void run();
};

#endif // READINPUT_H
