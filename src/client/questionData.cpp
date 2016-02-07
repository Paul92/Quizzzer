#include "questionData.h"
#include <QtDebug>

QuestionData::QuestionData(QStringList data) {
    qDebug() << data.size();
    this->question = data[0];
    qDebug() << data.size();
    data.removeFirst();
    this->answers = data;
}
