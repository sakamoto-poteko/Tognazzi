#ifndef UMESSANGER_H
#define UMESSANGER_H

#include "QTextEdit"
#include "QProgressBar"
#include "qobject.h"

namespace uMessageStatus {
enum
{
    ohCazzo,
    scappellata,
    superCazzola
};
}

class uMessanger //: public QObject
{
//        Q_OBJECT

public:
    uMessanger();
    uMessanger(QTextEdit*& lEdit, QProgressBar*& pBar);
    uMessanger (uMessanger * messanger);

protected:

    bool isActive_;
    QTextEdit * message_;
    QProgressBar * progress_;

//signals:
    virtual void sendMessage (QString message, int status);
    virtual void setProgress (int progress);

};

#endif // UMESSANGER_H
