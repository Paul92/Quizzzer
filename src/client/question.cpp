#include "question.h"
#include "ui.h"
#include "ui_question.h"

Question::Question(NetworkHandler &network, QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::QuestionWindow),
    network(network)
{
        ui->setupUi(this);

}


Question::~Question()
{
    delete ui;
}
