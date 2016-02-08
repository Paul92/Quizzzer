#include "endgame.h"
#include "ui.h"
#include "ui_end.h"
#include "networkHandler.h"
#include <QLabel>
#include <QDebug>


EndGame::EndGame(NetworkHandler &network, QWidget *parent) :
    network(network),
    QDialog(parent),
    ui(new Ui::EndGameWindow)
{


    setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::WindowTitleHint);

    ScoreTable table = network.getScoreTable();

    ui->setupUi(this);
    for (int i = 0; i < table.size(); i++) {
        qDebug() << table.players[i] << " " << table.scores[i];
        QLabel *player = new QLabel(table.players[i]);
        QLabel *score = new QLabel(table.scores[i]);
        this->ui->scoretable->addWidget(player, i, 0, 1, 1);
        this->ui->scoretable->addWidget(score, i, 1, 1, 1);
    }



}

EndGame::~EndGame() {}
