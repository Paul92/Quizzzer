#ifndef ENDGAME_H
#define ENDGAME_H

#include <QDialog>
#include "ui.h"
#include "networkHandler.h"

class EndGame : public QDialog
{
    Q_OBJECT

public:
    explicit EndGame(NetworkHandler &network, QWidget *parent = 0);
    ~EndGame();

public slots:

signals:


private:
    Ui::EndGameWindow *ui;
    NetworkHandler &network;

};

#endif // ENDGAME_H
