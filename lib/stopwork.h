#ifndef STOPWORK_H
#define STOPWORK_H
#include <QThread>

class Stopwork : public QThread
{
    Q_OBJECT

public:
    Stopwork();
private:
    void run();
};

#endif // STOPWORK_H
