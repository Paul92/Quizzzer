#include "login.h"
#include "question.h"
#include "networkHandler.h"
#include "error.h"
#include "waitWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    NetworkHandler network;
    Login w(network);


//Question w(network);
//Error w("LALA");

    w.exec();
    WaitWindow x(network);
    x.exec();
    Question q(network);
    q.show();



    return a.exec();
}
