#ifndef MESSAGEPRIVATE_H
#define MESSAGEPRIVATE_H

#include "message.h"

class MessagePrivate : public Message
{

private:
    std::wstring reciever;

public:
    std::wstring getReciever() {return reciever;}

    MessagePrivate(const std::wstring& msg, const std::wstring& sender, const std::wstring& reciever)
        : Message(msg, sender), reciever(reciever) {}

    MessagePrivate(const MessagePrivate& other)
        : Message(other), reciever(other.reciever) {}

    MessagePrivate(MessagePrivate&& other) noexcept
        : Message(std::move(other)), reciever(std::move(other.reciever)) {}

    MessagePrivate& operator=(const MessagePrivate& other)
    {
        if (this != &other)
        {
            Message::operator=(other);
            reciever = other.reciever;
        }
        return *this;
    }

    MessagePrivate& operator=(MessagePrivate&& other) noexcept
    {
        if (this != &other)
        {
            Message::operator=(std::move(other));
            reciever = std::move(other.reciever);
        }
        return *this;
    }
};

#endif // MESSAGEPRIVATE_H
