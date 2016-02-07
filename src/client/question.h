#ifndef QUESTION_H
#define QUESTION_H

#include <QDialog>
#include <QLabel>
#include "ui.h"
#include "networkHandler.h"

#include "questionData.h"
#include "scoreTable.h"

class Question : public QDialog
{
    Q_OBJECT

public:
    explicit Question(NetworkHandler &network, QWidget *parent = 0);
    ~Question();

public slots:

    void newScoreboardSlot(ScoreTable);
    void newQuestionSlot(QuestionData);

    void answer_A();
    void answer_B();
    void answer_C();
    void answer_D();

signals:



private:
    Ui::QuestionWindow *ui;
    NetworkHandler &network;

    void enableButtons();
    void disableButtons();

    QLabel *scoreboardLabel[4];
};


#endif // QUESTION_H
