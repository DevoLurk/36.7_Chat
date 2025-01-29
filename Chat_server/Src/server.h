#ifndef SERVER_H
#define SERVER_H

#ifdef _WIN32
#include<WinSock2.h>
#include<Windows.h>
#else
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#endif

#include "myCrypt.h"

#define PORT 50306

class Server
{
private:
    bool is_active = false;
    bool is_listen = false;
    std::wstring sv_err;

#ifdef _WIN32
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    sockaddr_in serverAddr, clientAddr;
    int clientAddrSize = sizeof(clientAddr);
#else
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
#endif

private:
    void sv_gettask(const int& connection_descriptor, std::wstring& name, std::wstring& task);
    void sv_sendHash(const int& connection_descriptor, const Hash& hash);
    void sv_sendBool(const int& connection_descriptor, const bool& flag);
    void sv_sendInt(const int& connection_descriptor, const int& integer);
    void sv_sendStr(const int& connection_descriptor, const std::wstring& str);
    void sv_readHash(const int& connection_descriptor, Hash& hash);
    void sv_readBool(const int& connection_descriptor, bool& flag);
    void sv_readInt(const int& connection_descriptor, int& integer);
    void sv_readStr(const int& connection_descriptor, std::wstring& str);

public:
    Server() = default;
    ~Server();
    bool sv_is_active();
    std::wstring get_svError();

    void startAndListen();
    bool acceptClient(int timeout_sec = 10);
    void closeConnection();
    void close_clientSocket();

    void gettask(std::wstring& name, std::wstring& task) {sv_gettask(this->clientSocket, name, task);};
    void sendHash(const Hash& hash) {sv_sendHash(this->clientSocket, hash);};
    void sendBool(const bool& flag) {sv_sendBool(this->clientSocket,flag);};
    void sendInt(const int& integer) {sv_sendInt(this->clientSocket,integer);};
    void sendStr(const std::wstring& str) {sv_sendStr(this->clientSocket,str);};
    void readHash(Hash& hash) {sv_readHash(this->clientSocket,hash);};
    void readBool(bool& flag) {sv_readBool(this->clientSocket,flag);};
    void readInt(int& integer) {sv_readInt(this->clientSocket,integer);};
    void readStr(std::wstring& str) {sv_readStr(this->clientSocket,str);};
};

#endif // SERVER_H
