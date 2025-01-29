#include "Message.h"

Message::Message(const std::wstring& msg, const std::wstring& name)
{
    message = msg;
    author = name;
    is_readed = false;
}

Message::Message(const Message& other)
{
    message = other.message;
    author = other.author;
    is_readed = other.is_readed;
}

Message::Message(Message&& other) noexcept
{
    message = other.message;
    author = other.author;
    is_readed = other.is_readed;

    other.author = L"";
    other.message = L"";
    other.is_readed = false;
}

Message& Message::operator=(const Message& other)
{
    message = other.message;
    author = other.author;
    is_readed = other.is_readed;

    return *this;
}

Message& Message::operator=(Message&& other) noexcept
{
    message = other.message;
    author = other.author;
    is_readed = other.is_readed;

    other.author = L"";
    other.message = L"";
    other.is_readed = false;

    return *this;
}

std::wstring Message::getAuthor()
{
    return author;
}

bool Message::getFlag()
{
    return is_readed;
}

void Message::setReaded()
{
    is_readed = true;
}

std::wstring Message::getMsg()
{
    return message;
}
