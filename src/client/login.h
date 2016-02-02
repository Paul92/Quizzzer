#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include "networkHandler.h"
#include "ui.h"

class Login : public QMainWindow
{
    Q_OBJECT

public:
    explicit Login(NetworkHandler &, QWidget *parent = 0);
    ~Login();

public slots:
    void login();
    void reg();
    void loginUnsucessfull();
    void registerUnsucessfull();
    void loginSucessfull();
    void registerSucessfull();
    void socketFailureSlot();
    void DNSFailureSlot();
    void connectionFailureSlot();

signals:
    void loginSignal(const QString &, const QString &);
    void regSignal(const QString &, const QString &);


private slots:
    void on_registerBtn_released();

private:
    Ui::LoginWindow *ui;
    NetworkHandler &network;
    bool checkUserPass();
};

#endif // LOGIN_H
