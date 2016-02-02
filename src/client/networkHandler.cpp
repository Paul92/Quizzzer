#include "networkHandler.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "error.h"
#include <QMessageBox>
#include <QApplication>

NetworkHandler::NetworkHandler(QObject *parent, QString host, int port) : QObject(parent), host(host), port(port)
{
    struct sockaddr_in serv_addr;
    struct hostent *server;

    this->sockfd = socket(AF_INET, SOCK_STREAM, 0);


    if (this->sockfd > 0) {
        QMessageBox::critical(0, tr("Eroare"), tr("Conectarea la server nu a reusit"));
        exit(0);
    }

    server = gethostbyname(this->host.toLatin1().data());

    if (server == NULL) {
        QMessageBox::critical(0, tr("Eroare"), tr("Conectarea la server nu a reusit"));
        exit(0);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);

    serv_addr.sin_port = htons(port);

    if (::connect(this->sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        QMessageBox::critical(0, tr("Eroare"), tr("Conectarea la server nu a reusit"));
        exit(0);
    }
}

void NetworkHandler::loginSlot(const QString &username, const QString &password) {
    QString loginString = "login " + username + " " + password;
    const char *loginCommand = loginString.toLatin1().data();
    write(this->sockfd, loginCommand, strlen(loginCommand));
    char buffer[64];
    read(this->sockfd, buffer, 64);
    if (strcmp(buffer, "OK") != 0)
        emit loginFailed();
    else
        emit loginSucessfull();
}

void NetworkHandler::registerSlot(const QString &username, const QString &password) {
    QString loginString = "newuser " + username + " " + password;
    const char *registerCommand = loginString.toLatin1().data();
    write(this->sockfd, registerCommand, strlen(registerCommand));
    char buffer[64];
    read(this->sockfd, buffer, 64);
    if (strcmp(buffer, "OK") != 0)
        emit registerFailed();
    else
        emit loginSucessfull();
}
