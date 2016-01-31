#include "quizzerclient.h"
#include <QApplication>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    QuizzerClient w;

    w.show();

    return a.exec();
}
