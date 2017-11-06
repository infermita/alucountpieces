#ifndef NFCTHREAD_H
#define NFCTHREAD_H

#include <QObject>
#include <nfc/nfc.h>
#include <QThread>
#include <QTimer>

class NfcThread : public QThread
{
    Q_OBJECT
public:
    NfcThread();
private:
    void run();
    nfc_target nt;
    nfc_context *context;
    nfc_device *pnd;
    QTimer *viewDet;
    QString lcd,repeat;
    int viewDetCnt;
public slots:
    void ViewDetTimer();
    void StartTimer();
};

#endif // NFCTHREAD_H
