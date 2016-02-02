#ifndef QUESTION_H
#define QUESTION_H

#include <QMainWindow>
#include "ui.h"
#include "networkHandler.h"

class Question : public QMainWindow
{
    Q_OBJECT

public:
    explicit Question(NetworkHandler &network, QWidget *parent = 0);
    ~Question();

public slots:

signals:



private:
    Ui::QuestionWindow *ui;
    NetworkHandler &network;
};


#endif // QUESTION_H
