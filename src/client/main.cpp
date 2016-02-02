#include "login.h"
#include "question.h"
#include "networkHandler.h"
#include "error.h"
#include <QApplication>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    NetworkHandler network;
    Login w(network);
//Question w(network);
//Error w("LALA");

    w.show();

    return a.exec();
}
