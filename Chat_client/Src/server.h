#ifndef SERVER_H
#define SERVER_H

#ifdef _WIN32
#include<WinSock2.h>
#include<Windows.h>
#else
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#endif

#include <queue>
#include <string>
#include "myCrypt.h"
#include "message.h"
#include "messageprivate.h"

#define PORT 50306

class Server
{
private:
    bool activeFlag = false;
    std::wstring err;

#ifdef _WIN32
    WSADATA wsaData;
    SOCKET clientSocket;
    sockaddr_in serverAddr;
#else
    int clientSocket;
    struct sockaddr_in serverAddr;
#endif

private:
    void sendtask(const std::wstring& username, const std::wstring& task);
    void sendHash(const Hash& hash);
    void sendBool(const bool& flag);
    void sendInt(const int& integer);
    void sendStr(const std::wstring& str);
    void readHash(Hash& hash);
    void readBool(bool& flag);
    void readInt(int& integer);
    void readStr(std::wstring& str);

public:
    Server()= default;
    ~Server();

    std::wstring get_svError();

    bool setConnection();
    void closeConnection();
    void setMsgReaded(const std::wstring& login, const int &pos);
    void sendMessage(const std::wstring& login, const std::wstring& reciever,const  std::wstring& message);
    void rememberMail(const std::wstring& login, const std::wstring& str);
    bool userExists(const std::wstring& login);
    bool checkPass(const std::wstring& login,const Hash& hash);
    void addUser(const std::wstring& login,const Hash& hash);
    void updatePublic(const std::wstring& login, std::vector<Message>& msgArr);
    void updatePrivate(const std::wstring& login, std::vector<MessagePrivate>& msgArr);
};

#endif // SERVER_H
