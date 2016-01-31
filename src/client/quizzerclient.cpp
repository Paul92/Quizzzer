#include "quizzerclient.h"
#include "ui_quizzerclient.h"

QuizzerClient::QuizzerClient(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QuizzerClient),
    network(new NetworkHandler)
{
    ui->setupUi(this);

    connect(ui->registerBtn, SIGNAL(released()), this, SLOT(reg()));
    connect(ui->loginBtn, SIGNAL(released()), this, SLOT(login()));

    connect(this, SIGNAL(regSignal(const QString &, const QString &)), &network, SLOT(registerSlot(const QString &, const QString &)));
    connect(this, SIGNAL(loginSignal(const QString &, const QString &)), &network, SLOT(loginSlot(const QString &, const QString &)));
}

QuizzerClient::~QuizzerClient()
{
    delete ui;
}

bool QuizzerClient::checkUserPass() {
    return ui->usernameLineEdit->text().isEmpty() || ui->passwordLineEdit->text().isEmpty();
}

void QuizzerClient::login() {
    if (this->checkUserPass())
        ui->messageLabel->setText("Introdu userul si parola");
    else
        emit loginSignal(ui->usernameLineEdit->text(), ui->passwordLineEdit->text());
}

void QuizzerClient::reg() {
    if (this->checkUserPass())
        ui->messageLabel->setText("Introdu userul si parola");
    else
        emit regSignal(ui->usernameLineEdit->text(), ui->passwordLineEdit->text());
}

void QuizzerClient::registerUnsucessfull() {
    ui->messageLabel->setText("Inregistrearea nu a avut success");
}

void QuizzerClient::loginUnsucessfull() {
    ui->messageLabel->setText("Logarea nu a avut success");
}

void QuizzerClient::loginSucessfull() {
    // clear window; let the games begin
}

void QuizzerClient::registerSucessfull() {
    ui->messageLabel->setText("Inregistrearea a avut success");
}
