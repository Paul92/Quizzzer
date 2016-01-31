#ifndef QUIZZERCLIENT_H
#define QUIZZERCLIENT_H

#include <QMainWindow>
#include "networkHandler.h"

namespace Ui {
class QuizzerClient;
}

class QuizzerClient : public QMainWindow
{
    Q_OBJECT

public:
    explicit QuizzerClient(QWidget *parent = 0);
    ~QuizzerClient();

public slots:
    void login();
    void reg();
    void loginUnsucessfull();
    void registerUnsucessfull();
    void loginSucessfull();
    void registerSucessfull();

signals:
    void loginSignal(const QString &, const QString &);
    void regSignal(const QString &, const QString &);


private:
    Ui::QuizzerClient *ui;
    NetworkHandler network;
    bool checkUserPass();
};

#endif // QUIZZERCLIENT_H
