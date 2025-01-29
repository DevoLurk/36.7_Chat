#include "server.h"

Server::~Server()
{
    closeConnection();
}

bool Server::setConnection()
{
#ifdef _WIN32
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        this->err = L"WSAStartup failed";
        return false;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        this->err = L"Creation of Socket failed!";
        WSACleanup();
        return false;
    }

    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_family = AF_INET;

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        this->err = L"Connection with the server failed!";
        closesocket(clientSocket);
        WSACleanup();
        return false;
    }
#else
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1)
    {
        this->err = L"Creation of Socket failed!";
        return false;
    }

    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_family = AF_INET;

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
    {
        this->err = L"Connection with the server failed!";
        close(clientSocket);
        return false;
    }
#endif
    this->activeFlag = true;
    return true;
}

void Server::closeConnection()
{
#ifdef _WIN32
    closesocket(clientSocket);
    WSACleanup();
#else
    close(clientSocket);
#endif
}

void Server::sendtask(const std::wstring &username, const std::wstring &task)
{
    sendStr(username);
    sendStr(task);
}

void Server::sendHash(const Hash &hash)
{
    send(this->clientSocket, (char*)&hash._part1, sizeof(uint), 0);
    send(this->clientSocket, (char*)&hash._part2, sizeof(uint), 0);
    send(this->clientSocket, (char*)&hash._part3, sizeof(uint), 0);
    send(this->clientSocket, (char*)&hash._part4, sizeof(uint), 0);
    send(this->clientSocket, (char*)&hash._part5, sizeof(uint), 0);
}

void Server::sendBool(const bool &flag)
{
    send(this->clientSocket, (char*)&flag, sizeof(bool), 0);
}

void Server::sendInt(const int &integer)
{
    send(this->clientSocket, (char*)&integer, sizeof(int), 0);
}

void Server::sendStr(const std::wstring &str)
{
    int length = static_cast<int>(str.length());
    sendInt(length);

    send(this->clientSocket, reinterpret_cast<const char*>(str.c_str()), length * sizeof(wchar_t), 0);
}

void Server::readHash(Hash &hash)
{
    recv(this->clientSocket, (char*)&hash._part1, sizeof(uint), 0);
    recv(this->clientSocket, (char*)&hash._part2, sizeof(uint), 0);
    recv(this->clientSocket, (char*)&hash._part3, sizeof(uint), 0);
    recv(this->clientSocket, (char*)&hash._part4, sizeof(uint), 0);
    recv(this->clientSocket, (char*)&hash._part5, sizeof(uint), 0);
}

void Server::readBool(bool &flag)
{
    recv(this->clientSocket, (char*)&flag, sizeof(bool), 0);
}

void Server::readInt(int &integer)
{
    recv(this->clientSocket, (char*)&integer, sizeof(int), 0);

}

void Server::readStr(std::wstring &str)
{
    int length = 0;
    readInt(length);

    std::wstring readed(length, L'\0');
    recv(this->clientSocket, reinterpret_cast<char*>(&readed[0]), length * sizeof(wchar_t), 0);
    str = readed;
}

void Server::setMsgReaded(const std::wstring& login, const int &pos)
{
    std::wstring task = L"setMsgReaded";

    sendtask(login, task);
    sendInt(pos);
}

void Server::sendMessage(const std::wstring& login,const std::wstring &reciever,const std::wstring &message)
{
    std::wstring task = L"sendMessage";
    sendtask(login, task);
    sendStr(reciever);
    sendStr(message);
}

bool Server::userExists(const std::wstring &login)
{
    bool user_exists;
    std::wstring task = L"userExists";

    sendtask(login, task);
    readBool(user_exists);

    return user_exists;
}

bool Server::checkPass(const std::wstring &login, const Hash &hash)
{
    bool isPass;
    std::wstring task = L"checkPass";

    sendtask(login, task);
    sendHash(hash);
    readBool(isPass);

    return isPass;
}

void Server::addUser(const std::wstring &login, const Hash &hash)
{
    std::wstring task = L"addUser";
    sendtask(login, task);
    sendHash(hash);
}

void Server::rememberMail(const std::wstring& login, const std::wstring &str)
{
    std::wstring task = L"rememberPUBMail";
    sendtask(login, task);
    sendStr(str);
}

void Server::updatePublic(const std::wstring &login, std::vector<Message>& msgArr)
{
    std::wstring task = L"updatePublic";
    sendtask(login, task);
    sendInt(msgArr.size());

    int messages_to_recieve;
    readInt(messages_to_recieve);

    if(messages_to_recieve <= 0)
        return;

    bool send_flag;
    readBool(send_flag);

    if(!send_flag)
        return; // server db Error

    for(auto i{0}; i < messages_to_recieve; i++)
    {
        std::wstring message;
        std::wstring author;

        readStr(message);
        readStr(author);

        msgArr.emplace_back(message, author);
    }
}

void Server::updatePrivate(const std::wstring &login, std::vector<MessagePrivate> &msgArr)
{
    std::wstring task = L"updatePrivate";
    sendtask(login, task);
    sendInt(msgArr.size());

    int messages_to_recieve;
    readInt(messages_to_recieve);

    if(messages_to_recieve <= 0)
        return;

    bool send_flag;
    readBool(send_flag);

    if(!send_flag)
        return; // server db Error


    for(auto i{0}; i < messages_to_recieve; i++)
    {
        std::wstring sender;
        std::wstring reciever;
        std::wstring message;
        bool flag;

        readStr(sender);
        readStr(reciever);
        readStr(message);
        readBool(flag);

        MessagePrivate msg( message, sender,reciever);
        if(flag) msg.setReaded();

        msgArr.push_back(msg);
    }
}




