#include "login.h"
#include "ui_login.h"

#include <QMessageBox>

#include  "error.h"

Login::Login(NetworkHandler &network, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginWindow),
    network(network)
{
    ui->setupUi(this);

    connect(ui->registerBtn, SIGNAL(released()), this, SLOT(reg()));
    connect(ui->loginBtn, SIGNAL(released()), this, SLOT(login()));

    connect(this, SIGNAL(regSignal(const QString &, const QString &)), &network, SLOT(registerSlot(const QString &, const QString &)));
    connect(this, SIGNAL(loginSignal(const QString &, const QString &)), &network, SLOT(loginSlot(const QString &, const QString &)));

    connect(&network, SIGNAL(socketFailureSignal()), this, SLOT(socketFailureSlot()));
    connect(&network, SIGNAL(DNSFailureSignal()), this, SLOT(DNSFailureSlot()));
    connect(&network, SIGNAL(connectionFailureSignal()), this, SLOT(connectionFailureSlot()));
}

Login::~Login()
{
    delete ui;
}

bool Login::checkUserPass() {
    return ui->usernameLineEdit->text().isEmpty() || ui->passwordLineEdit->text().isEmpty();
}

void Login::login() {
    if (this->checkUserPass())
        ui->messageLabel->setText("Introdu userul si parola");
    else
        emit loginSignal(ui->usernameLineEdit->text(), ui->passwordLineEdit->text());
}

void Login::reg() {
    if (this->checkUserPass())
        ui->messageLabel->setText("Introdu userul si parola");
    else
        emit regSignal(ui->usernameLineEdit->text(), ui->passwordLineEdit->text());
}

void Login::registerUnsucessfull() {
    ui->messageLabel->setText("Inregistrearea nu a avut success");
}

void Login::loginUnsucessfull() {
    ui->messageLabel->setText("Logarea nu a avut success");
}

void Login::loginSucessfull() {
    // clear window; let the games begin
}

void Login::registerSucessfull() {
    ui->messageLabel->setText("Inregistrearea a avut success");
}

void Login::socketFailureSlot() {
    QMessageBox::warning(this, tr("ok"), tr("ok"));
}

void Login::connectionFailureSlot() {
    QMessageBox::warning(this, tr("ok"), tr("ok"));
}

void Login::DNSFailureSlot() {
    QMessageBox::warning(this, tr("ok"), tr("ok"));
}

