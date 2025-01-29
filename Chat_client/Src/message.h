#ifndef MESSAGE_H
#define MESSAGE_H

#include<string>

class Message
{
private:
    std::wstring message;
    std::wstring author;
    bool is_readed;

public:
    Message(const std::wstring& msg, const std::wstring& name);

    Message(const Message& other);
    Message(Message&& other) noexcept;
    Message& operator=(const Message& other);
    Message& operator=(Message&& other) noexcept;

    std::wstring getMsg();
    std::wstring getAuthor();
    bool getFlag();
    void setReaded();
};

#endif // MESSAGE_H
