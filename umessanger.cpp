#include "umessanger.h"
#include <iostream>

uMessanger::uMessanger(QTextEdit*& lEdit, QProgressBar*& pBar)
{
    message_ = lEdit;
    message_->setText("");
    progress_ = pBar;
    progress_->setRange(0,100);
    progress_->setValue(0);

    isActive_ = true;
}

uMessanger::uMessanger(uMessanger *messanger)
{
    message_  = messanger->message_;
    progress_ = messanger->progress_;

    isActive_ = true;
}

uMessanger::uMessanger()
{
    isActive_ = false;
}


void uMessanger::sendMessage (QString message, int status)
{
    if (!isActive_)
    {
        std::cout << "I cant' send any message on GUI!\n";
        return;
    }

    message_->clear();

    if (status == uMessageStatus::ohCazzo)
    {
        QFont serifFont("Times", 10, QFont::Bold);
        QColor red (255, 0, 0, 255);
        message_->setTextColor(red);
        message_->setFont(serifFont);
        message_->setText(message);
    }
    else if (status == uMessageStatus::scappellata)
    {
        QFont serifFont("Times", 10, QFont::Bold);
        QColor green (0, 255, 0, 255);
        message_->setTextColor(green);
        message_->setFont(serifFont);
        message_->setText(message);
    }
    else if (status == uMessageStatus::superCazzola)
    {
        QFont serifFont("Times", 10, QFont::Bold);
        QColor blue (0, 0, 255, 255);
        message_->setTextColor(blue);
        message_->setFont(serifFont);
        message_->setText(message);
    }

    message_->show();
}

void uMessanger::setProgress (int progress)
{
    if (!isActive_)
    {
        std::cout << "I cant' send any message on GUI!\n";
        return;
    }

    progress_->setRange(0,100);
    progress_->setValue(progress);
    progress_->show();
}
