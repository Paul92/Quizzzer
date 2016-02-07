#ifndef WAITWINDOW_H
#define WAITWINDOW_H

#include <QDialog>
#include "ui.h"
#include "networkHandler.h"

class WaitWindow : public QDialog
{
    Q_OBJECT

public:
    explicit WaitWindow(NetworkHandler &network, QWidget *parent = 0);
    void wait();
    ~WaitWindow();

public slots:
    void gameStartSlot();
signals:
    void gameStartSignal();

private:
    Ui::WaitWindow *ui;
    NetworkHandler &network;
};

#endif // WAITWINDOW_H
