#ifndef SLOTTESTER_H
#define SLOTTESTER_H

#include <QObject>
#include <QSocketNotifier>
#include "questionData.h"
#include "scoreTable.h"

#define MAX_QUESTION_FIELDS 6

class NetworkHandler : public QObject
{
    Q_OBJECT
public:
    explicit NetworkHandler(QObject *parent = 0, QString host = "localhost", int port = 9898);
    ~NetworkHandler();
    void waitForGameStart();
    void waitForQuestionUpdate();

    void sendAnswer(int answer);
    ScoreTable getScoreTable();

    void quit();
    void logout();

public slots:
    void loginSlot(const QString &username, const QString &password);
    void registerSlot(const QString &username, const QString &password);
    void gameStartSlot(int sockfd);
    void newQuestionSlot(int sockfd);

signals:
    void loginFailed();
    void registerFailed();
    void loginSucessfull();
    void registerSucessfull();
    void socketFailureSignal();
    void DNSFailureSignal();
    void connectionFailureSignal();
    void gameStartSignal();
    void gameEndSignal();

    void newQuestionSignal(QuestionData question);
    void newScoreboardSignal(ScoreTable score);


private:
    QString host;
    int port;
    int sockfd;
    QSocketNotifier *notifier;

    void handleQuestion(QString);
    void handleScoreboard(QString);
    void handleEndgame(QString);
};

#endif // SLOTTESTER_H
