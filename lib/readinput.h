#ifndef READINPUT_H
#define READINPUT_H
#include <QThread>

class ReadInput : public QThread
{
    Q_OBJECT
public:
    ReadInput();

private:
    void run();
};

#endif // READINPUT_H
