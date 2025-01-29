#ifndef DATABASE_H
#define DATABASE_H

#ifdef _WIN32
#include<Windows.h>
#endif

#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>

#include <queue>
#include <sstream>
#include "myCrypt.h"


#define SQL_TEXT_SIZE 65535

class Database
{
private:
    SQLHANDLE sqlConnHandle{ nullptr };
    SQLHANDLE sqlEnvHandle{ nullptr };
    SQLHANDLE sqlStmtHandle{ nullptr };
    SQLWCHAR retconstring[1024]{};

    bool is_active = false;
    std::wstring config;
    std::wstring sql_err;

private:
    Hash arr_to_Hash(const unsigned char* hash_arr);
    std::wstring hash_to_str(const Hash& hash_class);
    bool db_connect_inner();
    void set_SQLError(unsigned int handleType, const SQLHANDLE& handle);
    void wStringReplacer(std::wstring& inputStr, const std::wstring& to_replace, const std::wstring& replace_with);

public:
    Database() = delete;
    Database(const std::wstring& config);
    ~Database();

    void db_connect();
    void db_disconnect();
    bool db_is_active();
    bool db_create(const std::wstring& db_name);
    std::wstring get_SQLError();

    // from here all functions add logs to logger
    // all bool functions -> return bool as a result of completion of their work
    // all int -> return -1 if falue;

    int db_getUserID(const std::wstring& user);
    int db_getMaxUserID();
    int db_getUsersCount();
    int db_getPublicMsgCnt();
    int db_getPrivateMsgCnt(const std::wstring& username);

    bool db_getUsernames(std::queue<std::wstring>& names_queue, const int& users_cnt);
    bool db_addUser(const Hash& user_hash, const std::wstring& username);
    bool db_getUserHash(Hash& server_hash, const std::wstring& username);
    bool db_setMsgReaded(const std::wstring& username, const int& poz);
    bool db_addPrivateMsg(const std::wstring& sender, const std::wstring& reciever, const std::wstring& mesage);
    bool db_addPublicMsg(const std::wstring& sender, const std::wstring& mesage);
    bool db_userExists(const std::wstring& username, bool& userExists_flag);
    bool db_getAuthrorAndMessage_pub(std::queue<std::wstring>& author_queue, std::queue<std::wstring> &message_queue, const int &client_msg_cnt, const int &server_msg_cnt);
    bool db_getAuthrorAndMessage_priv(std::queue<std::wstring>& sender_queue,
                                      std::queue<std::wstring>& reciever_queue,
                                      std::queue<std::wstring>& message_queue,
                                      std::queue<bool>& flag_queue,
                                      const int &client_msg_cnt,
                                      const int &server_msg_cnt,
                                      const std::wstring& username);
};

#endif // DATABASE_H
