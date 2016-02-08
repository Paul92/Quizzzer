#include "waitWindow.h"
#include "ui_wait.h"


WaitWindow::WaitWindow(NetworkHandler &network, QWidget *parent) :
    network(network),
    QDialog(parent),
    ui(new Ui::WaitWindow)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::WindowTitleHint);
    connect(&network, SIGNAL(gameStartSignal()), this, SLOT(gameStartSlot()));

    network.waitForGameStart();
}

void WaitWindow::gameStartSlot() {
    qDebug("Game start slot called");
    this->close();
    qDebug("WAIT WINDOW CLOSED");
}

WaitWindow::~WaitWindow()
{
    delete ui;
}
