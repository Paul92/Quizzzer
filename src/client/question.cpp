#include "question.h"
#include "ui.h"
#include "ui_question.h"
#include <QtDebug>

Question::Question(NetworkHandler &network, QWidget *parent):
    QDialog(parent),
    ui(new Ui::QuestionWindow),
    network(network)
{

    setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::WindowTitleHint);
    ui->setupUi(this);


    connect(&network, SIGNAL(newScoreboardSignal(ScoreTable)), this, SLOT(newScoreboardSlot(ScoreTable)));
    connect(&network, SIGNAL(newQuestionSignal(QuestionData)), this, SLOT(newQuestionSlot(QuestionData)));
    connect(&network, SIGNAL(gameEndSignal()), this, SLOT(gameEndSlot()));

    connect(this->ui->Abtn, SIGNAL(released()), this, SLOT(answer_A()));
    connect(this->ui->Bbtn, SIGNAL(released()), this, SLOT(answer_B()));
    connect(this->ui->Cbtn, SIGNAL(released()), this, SLOT(answer_C()));
    connect(this->ui->Dbtn, SIGNAL(released()), this, SLOT(answer_D()));


    network.waitForQuestionUpdate();

    network.newQuestionSlot(0);
    network.newQuestionSlot(0);
}

void Question::gameEndSlot() {
    qDebug("GAME END SLOT");
    this->close();
}

void Question::newScoreboardSlot(ScoreTable table) {
    qDebug() << "Called scoreboard slot" << table.size();
    if (this->ui->scoreBoardLayout->count() == 0) {
        for (int i = 0; i < table.size(); i++) {
            this->scoreboardLabel[i] = new QLabel();
            this->scoreboardLabel[i]->setText(table.players[i] + ": " + table.scores[i]);
            this->ui->scoreBoardLayout->addWidget(this->scoreboardLabel[i]);
        }
    } else {
        for (int i = 0; i < table.size(); i++) {
            this->scoreboardLabel[i]->setText(table.players[i] + ": " + table.scores[i]);
        }
    }
}

void Question::newQuestionSlot(QuestionData question) {
    this->ui->question->setText(question.question);
    this->ui->Alabel->setText(question.answers[0]);
    this->ui->Blabel->setText(question.answers[1]);
    this->ui->Clabel->setText(question.answers[2]);
    this->ui->Dlabel->setText(question.answers[3]);
    this->enableButtons();
}

void Question::enableButtons() {
    this->ui->Abtn->setEnabled(true);
    this->ui->Bbtn->setEnabled(true);
    this->ui->Cbtn->setEnabled(true);
    this->ui->Dbtn->setEnabled(true);
}

void Question::disableButtons() {
    this->ui->Abtn->setEnabled(false);
    this->ui->Bbtn->setEnabled(false);
    this->ui->Cbtn->setEnabled(false);
    this->ui->Dbtn->setEnabled(false);
}

void Question::answer_A() {
    this->network.sendAnswer(0);
    this->disableButtons();
}

void Question::answer_B() {
    this->network.sendAnswer(1);
    this->disableButtons();
}

void Question::answer_C() {
    this->network.sendAnswer(2);
    this->disableButtons();
}

void Question::answer_D() {
    this->network.sendAnswer(3);
    this->disableButtons();
}

Question::~Question() {
    delete ui;
}
