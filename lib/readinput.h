#ifndef READINPUT_H
#define READINPUT_H
#include <QThread>
#include <QNetworkReply>

class ReadInput : public QThread
{
    Q_OBJECT
public:
    ReadInput();
    QString foot;
public slots:
    void replyFinished (QNetworkReply *reply);

private:
    void run();

signals:
    void StartTimer();
};

#endif // READINPUT_H
