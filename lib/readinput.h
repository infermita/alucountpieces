#ifndef READINPUT_H
#define READINPUT_H
#include <QThread>
#include <QNetworkReply>
#include <QTimer>

class ReadInput : public QThread
{
    Q_OBJECT
public:
    ReadInput();
    QString foot;
public slots:
    void replyFinished (QNetworkReply *reply);
    void ViewStopTimer();
private:
    void run();
    int led1;
    QTimer *viewStop;
signals:
    void StartTimer();
};

#endif // READINPUT_H
