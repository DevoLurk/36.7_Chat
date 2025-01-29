#include "server.h"

Server::~Server()
{
    closeConnection();
}

bool Server::sv_is_active()
{
    return this->is_active;
}

std::wstring Server::get_svError()
{
    return this->sv_err;
}

void Server::startAndListen()
{
#ifdef _WIN32
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        this->sv_err = L"WSAStartup failed";
        return;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        this->sv_err = L"Socket creation failed!";
        WSACleanup();
        return;
    }

    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_family = AF_INET;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        this->sv_err = L"Socket binding failed!";
        closesocket(serverSocket);
        WSACleanup();
        return;
    }
#else
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        this->sv_err = L"Socket creation failed!";
        close(serverSocket);
        return;
    }

    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_family = AF_INET;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
    {
        this->sv_err = L"Socket binding failed!";
        close(serverSocket);
        return;
    }
#endif

    this->is_active = true;
}

bool Server::acceptClient(int timeout_sec)
{
    if(!this->is_listen)
    {
#ifdef _WIN32
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        this->sv_err = L"Socket is unable to listen for new connections!";
        closesocket(serverSocket);
        WSACleanup();
        return false;
    }
#else
    if (listen(serverSocket, SOMAXCONN) == -1)
    {
        this->sv_err = L"Socket is unable to listen for new connections!";
        close(serverSocket);
        return false;
    }
#endif
    }
    this->is_listen = true;

    fd_set readfds;                      // Создаем набор дескрипторов для чтения
    FD_ZERO(&readfds);                   // Очищаем набор
    FD_SET(serverSocket, &readfds);      // Добавляем серверный сокет в набор

    struct timeval timeout;              // Задаём тайм-аут
    timeout.tv_sec = timeout_sec;        // секунд
    timeout.tv_usec = 0;                 // микросекунд

    // Ожидание активности на сокете
    int activity = select(serverSocket + 1, &readfds, nullptr, nullptr, &timeout);

    if (activity > 0 && FD_ISSET(serverSocket, &readfds)) // входящее подключение
    {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);

#ifdef _WIN32
        if (clientSocket == INVALID_SOCKET)
            return false;
#else
        if (clientSocket == -1)
            return false;
#endif
        return true;                     // Подключение принято
    }

    return false;
}

void Server::closeConnection()
{
#ifdef _WIN32
    closesocket(serverSocket);
    WSACleanup();
#else
    close(serverSocket);
#endif
}

void Server::close_clientSocket()
{
    closesocket(this->clientSocket);

#ifdef _WIN32
    clientSocket = INVALID_SOCKET;
#else
    clientSocket = -1;
#endif
}

void Server::sv_gettask(const int &connection_descriptor, std::wstring &name, std::wstring &task)
{
    sv_readStr(connection_descriptor, name);
    sv_readStr(connection_descriptor, task);
}

void Server::sv_sendHash(const int &connection_descriptor, const Hash &hash)
{
    send(connection_descriptor, (char*)&hash._part1, sizeof(uint), 0);
    send(connection_descriptor, (char*)&hash._part2, sizeof(uint), 0);
    send(connection_descriptor, (char*)&hash._part3, sizeof(uint), 0);
    send(connection_descriptor, (char*)&hash._part4, sizeof(uint), 0);
    send(connection_descriptor, (char*)&hash._part5, sizeof(uint), 0);
}

void Server::sv_sendBool(const int &connection_descriptor, const bool &flag)
{
    send(connection_descriptor, (char*)&flag, sizeof(bool), 0);
}

void Server::sv_sendInt(const int &connection_descriptor, const int &integer)
{
    send(connection_descriptor, (char*)&integer, sizeof(int), 0);
}

void Server::sv_sendStr(const int &connection_descriptor, const std::wstring &str)
{
    int length = static_cast<int>(str.length());
    sv_sendInt(connection_descriptor, length);

    send(connection_descriptor, reinterpret_cast<const char*>(str.c_str()), length * sizeof(wchar_t), 0);
}

void Server::sv_readHash(const int &connection_descriptor, Hash &hash)
{
    recv(connection_descriptor, (char*)&hash._part1, sizeof(uint), 0);
    recv(connection_descriptor, (char*)&hash._part2, sizeof(uint), 0);
    recv(connection_descriptor, (char*)&hash._part3, sizeof(uint), 0);
    recv(connection_descriptor, (char*)&hash._part4, sizeof(uint), 0);
    recv(connection_descriptor, (char*)&hash._part5, sizeof(uint), 0);
}

void Server::sv_readBool(const int &connection_descriptor, bool &flag)
{
    recv(connection_descriptor, (char*)&flag, sizeof(bool), 0);
}

void Server::sv_readInt(const int &connection_descriptor, int &integer)
{
    recv(connection_descriptor, (char*)&integer, sizeof(int), 0);
}

void Server::sv_readStr(const int &connection_descriptor, std::wstring &str)
{
    int length = 0;
    sv_readInt(connection_descriptor, length);

    std::wstring readed(length, L'\0');
    recv(connection_descriptor, reinterpret_cast<char*>(&readed[0]), length * sizeof(wchar_t), 0);
    str = readed;
}
