#include "scoreTable.h"

ScoreTable::ScoreTable(QStringList data) {
    for (int i = 0; i < data.size(); i++)
        this->addPlayer(data[i]);
}

void ScoreTable::addPlayer(QString data) {
    QStringList dataList = data.split(":");
    this->players += dataList[0];
    this->scores  += dataList[1];
}

int ScoreTable::size() {
    return this->players.size();
}

