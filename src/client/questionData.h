#ifndef QUESTIONDATA_H
#define QUESTIONDATA_H

#include <QString>
#include <QStringList>

class QuestionData
{
public:
    QuestionData(QStringList);
    QString question;
    QStringList answers;
};

#endif // QUESTIONDATA_H
