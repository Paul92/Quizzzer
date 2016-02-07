#include "networkHandler.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "error.h"
#include <QtDebug>
#include <QMessageBox>
#include <QApplication>
#include <QSocketNotifier>

NetworkHandler::NetworkHandler(QObject *parent, QString host, int port) : QObject(parent), host(host), port(port)
{
    struct sockaddr_in serv_addr;
    struct hostent *server;

    this->sockfd = socket(AF_INET, SOCK_STREAM, 0);


    if (this->sockfd < 0) {
        QMessageBox::critical(0, tr("Eroare"), tr("Conectarea la server nu a reusit (socket)"));
        exit(0);
    }

    server = gethostbyname(this->host.toLatin1().data());

    if (server == NULL) {
        QMessageBox::critical(0, tr("Eroare"), tr("Conectarea la server nu a reusit (DNS)"));
        exit(0);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);

    serv_addr.sin_port = htons(port);

    if (::connect(this->sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        QMessageBox::critical(0, tr("Eroare"), tr("Conectarea la server nu a reusit (connect)"));
        exit(0);
    }

    this->notifier = new QSocketNotifier(this->sockfd, QSocketNotifier::Read);
    this->notifier->setEnabled(false);
}

void NetworkHandler::loginSlot(const QString &username, const QString &password) {
    QString loginString = "login " + username + " " + password + "\n";
    const char *loginCommand = loginString.toLatin1().data();
    write(this->sockfd, loginCommand, strlen(loginCommand));
    char buffer[64];
    read(this->sockfd, buffer, 64);

    if (strcmp(buffer, "OK\n") != 0)
        emit loginFailed();
    else
        emit loginSucessfull();
}

void NetworkHandler::registerSlot(const QString &username, const QString &password) {
    QString loginString = "newuser " + username + " " + password + "\n";
    const char *registerCommand = loginString.toLatin1().data();
    write(this->sockfd, registerCommand, strlen(registerCommand));
    char buffer[64];
    read(this->sockfd, buffer, 64);
    if (strcmp(buffer, "OK\n") != 0)
        emit registerFailed();
    else
        emit loginSucessfull();
}

NetworkHandler::~NetworkHandler() {
    char quitString[] = "quit\n";
    write(this->sockfd, quitString, strlen(quitString));
    delete this->notifier;
    close(this->sockfd);
}

void NetworkHandler::waitForGameStart() {
    connect(this->notifier, SIGNAL(activated(int)), this, SLOT(gameStartSlot(int)));
    this->notifier->setEnabled(true);
    qDebug("GameStart Activated\n");
}

void NetworkHandler::waitForQuestionUpdate() {
    connect(this->notifier, SIGNAL(activated(int)), this, SLOT(newQuestionSlot(int)));
    this->notifier->setEnabled(true);
    qDebug("QuestionUpdate Activated\n");
}

void NetworkHandler::gameStartSlot(int sockfd) {
    qDebug("SlotCalled\n");
    char buffer[256];
    read(this->sockfd, buffer, 256);
    if (strcmp(buffer, "game_started\n") == 0) {
        qDebug("Signal Emitted\n");
        this->notifier->setEnabled(false);
        disconnect(this->notifier, 0, this, 0);
        emit gameStartSignal();
    }
}

void NetworkHandler::handleQuestion(QString buffer) {
    qDebug("Got new question\n");
    qDebug() << buffer;
    QStringList params = buffer.split("|");
    params.removeFirst();
    QuestionData question(params);
    qDebug("Fine");
    qDebug("Raising new question signal\n");
    emit newQuestionSignal(question);
}

void NetworkHandler::handleScoreboard(QString buffer) {
    qDebug("Got new score\n");
    QStringList params = buffer.split("|");

    params.removeFirst();
    ScoreTable table(params);
    qDebug("Raising new scoreboard signal\n");
    emit newScoreboardSignal(table);
}

void NetworkHandler::newQuestionSlot(int sockfd) {
    char buffer[1024];
    memset(buffer, 0, 1024 * sizeof(char));
    read(this->sockfd, buffer, 1024);
        qDebug(buffer);
    if (strncmp(buffer, "question", 8) == 0)
        handleQuestion(buffer);
    else if (strncmp(buffer, "scoreboard", 10) == 0)
        handleScoreboard(buffer);
    else
        qCritical("Unexpected command in newQuestionSlot\n");
}

void NetworkHandler::sendAnswer(int answer) {
    QString command = "answer " + QString::number(answer) + "\n";
    const char *answerCommand = command.toLatin1().data();
    write(this->sockfd, answerCommand, strlen(answerCommand));
    qDebug("Answer command sent");
}

