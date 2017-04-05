#ifndef JSONCOMMAND_H
#define JSONCOMMAND_H
#include <QObject>
#include <QTcpSocket>

class JsonCommand
{
public:
    JsonCommand();
    void ElaborateCommand(QTcpSocket *socket);

private:
    void SendResponse(QTcpSocket *socket,QString response,QString addP="");
    QStringList options;


};

#endif // JSONCOMMAND_H
