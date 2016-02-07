#ifndef SCORETABLE_H
#define SCORETABLE_H

#include <QString>
#include <QStringList>

#define MAX_PLAYERS 2

class ScoreTable
{
public:
    ScoreTable(QStringList data);
    int noOfPlayers;
    QStringList players;
    QStringList scores;
    void addPlayer(QString data);
    int size();
};

#endif // SCORETABLE_H
