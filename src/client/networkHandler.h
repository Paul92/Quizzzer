#ifndef SLOTTESTER_H
#define SLOTTESTER_H

#include <QObject>

class NetworkHandler : public QObject
{
    Q_OBJECT
public:
    explicit NetworkHandler(QObject *parent = 0, QString host = "localhost", int port = 9898);

public slots:
    void loginSlot(const QString &username, const QString &password);
    void registerSlot(const QString &username, const QString &password);

signals:
    void loginFailed();
    void registerFailed();
    void loginSucessfull();
    void registerSucessfull();

private:
    QString host;
    int port;
    int sockfd;
};

#endif // SLOTTESTER_H
