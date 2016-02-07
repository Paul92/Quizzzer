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
    this->close();
}

WaitWindow::~WaitWindow()
{
    delete ui;
}
