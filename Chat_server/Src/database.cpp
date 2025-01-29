#include "database.h"


Database::Database(const std::wstring &config)
{
    this->config = config;
}

Database::~Database()
{
    db_disconnect();
}

Hash Database::arr_to_Hash(const unsigned char *hash_arr)
{
    Hash result;
    unsigned int newArr[5];

    for (int i = 0; i < 5; i++)
    {
        newArr[i] = (hash_arr[i * 4] << 24) |
                    (hash_arr[i * 4 + 1] << 16) |
                    (hash_arr[i * 4 + 2] << 8) |
                     hash_arr[i * 4 + 3];
    }

    result._part1 = newArr[0];
    result._part2 = newArr[1];
    result._part3 = newArr[2];
    result._part4 = newArr[3];
    result._part5 = newArr[4];

    return result;
}

std::wstring Database::hash_to_str(const Hash &hash_class)
{
    std::wstringstream ss;
    ss << std::hex << hash_class._part1
       << hash_class._part2
       << hash_class._part3
       << hash_class._part4
       << hash_class._part5;

    return ss. str();
}

bool Database::db_connect_inner()
{
    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlEnvHandle))
    {
        db_disconnect();
        return false;
    }

    if (SQL_SUCCESS != SQLSetEnvAttr(sqlEnvHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0))
    {
        db_disconnect();
        return false;
    }

    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_DBC, sqlEnvHandle, &sqlConnHandle))
    {
        db_disconnect();
        return false;
    }

    switch (SQLDriverConnect(sqlConnHandle,
                             GetDesktopWindow(),
                             (SQLWCHAR*)(this->config.c_str()),
                             SQL_NTS,
                             retconstring,
                             1024,
                             NULL,
                             SQL_DRIVER_COMPLETE))
    {
    case SQL_INVALID_HANDLE:
        set_SQLError(SQL_HANDLE_DBC, sqlConnHandle);
        break;
    case SQL_ERROR:
        set_SQLError(SQL_HANDLE_DBC, sqlConnHandle);
        db_disconnect();
        return false;
    default:
        break;
    }

    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle))
        return false;

    return true;
}

void Database::set_SQLError(unsigned int handleType, const SQLHANDLE& handle)
{
    SQLWCHAR SQLState[1024];
    SQLWCHAR message[1024];

    if (SQL_SUCCESS == SQLGetDiagRec(handleType, handle, 1, SQLState, NULL, message, 1024, NULL))
    {
        this->sql_err.clear();

        this->sql_err += L"SQL driver message: ";
        this->sql_err += (std::wstring)message;
        this->sql_err += L" SQL state: ";
        this->sql_err += (std::wstring)SQLState;
        this->sql_err += L"!";
    }
}

void Database::wStringReplacer(std::wstring &inputStr, const std::wstring &to_replace, const std::wstring &replace_with)
{
    size_t pos = inputStr.find(to_replace);
    inputStr.replace(pos, to_replace.size(), replace_with);
}

std::wstring Database::get_SQLError()
{
    return this->sql_err;
}

void Database::db_connect()
{
    this->is_active = db_connect_inner();
}

void Database::db_disconnect()
{
    SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
    SQLDisconnect(sqlConnHandle);
    SQLFreeHandle(SQL_HANDLE_DBC, sqlConnHandle);
    SQLFreeHandle(SQL_HANDLE_ENV, sqlEnvHandle);
}

bool Database::db_is_active()
{
    return this->is_active;
}

bool Database::db_create(const std::wstring& db_name)
{
    std::wstring wcommand1 = L"create database --db_name--;";
    wStringReplacer(wcommand1, L"--db_name--", db_name);

    std::wstring wcommand2 = L"use --db_name--;";
    wStringReplacer(wcommand2, L"--db_name--", db_name);

    std::wstring wcommand3 = LR"(create table users(
                                    id integer auto_increment primary key,
                                    login varchar(30) unique);)";

    std::wstring wcommand4 = LR"(create table pass(
                                    id_user integer,
                                    hash BINARY(20) not null,
                                    FOREIGN key (id_user) references users(id));)";

    std::wstring wcommand5 = LR"(create table public_msg(
                                    id integer auto_increment primary key,
                                    message text,
                                    id_sender integer,
                                    FOREIGN key (id_sender) references users(id));)";

    std::wstring wcommand6 = LR"(create table private_msg(
                                    id integer auto_increment primary key,
                                    id_sender integer,
                                    id_reciever integer,
                                    message text,
                                    flag bool,
                                    FOREIGN key (id_sender) references users(id),
                                    FOREIGN key (id_reciever) references users(id));)";

    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wcommand1.c_str(), SQL_NTS))
        return false;
    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wcommand2.c_str(), SQL_NTS))
        return false;
    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wcommand3.c_str(), SQL_NTS))
        return false;
    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wcommand4.c_str(), SQL_NTS))
        return false;
    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wcommand5.c_str(), SQL_NTS))
        return false;
    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wcommand6.c_str(), SQL_NTS))
        return false;

    return true;
}

int Database::db_getUserID(const std::wstring &user)
{
    SQLINTEGER users_id;
    std::wstring command = LR"( select id
                                from users
                                where login = '--username--';)";

    wStringReplacer(command, L"--username--", user);

    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);

    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)command.c_str(), SQL_NTS))
        return -1;

    SQLBindCol(sqlStmtHandle, 1, SQL_INTEGER, &users_id, sizeof(users_id), NULL);
    SQLFetch(sqlStmtHandle);

    return (int)users_id;
}

int Database::db_getMaxUserID()
{
    SQLINTEGER user_max_id;
    std::wstring command = LR"( select count(id)
                                from users; )";

    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)command.c_str(), SQL_NTS))
        return -1;

    SQLBindCol(sqlStmtHandle, 1, SQL_INTEGER, &user_max_id, sizeof(user_max_id), NULL);
    SQLFetch(sqlStmtHandle);

    return (int)user_max_id;
}

bool Database::db_addUser(const Hash &user_hash, const std::wstring &username)
{
    std::wstring userhash = hash_to_str(user_hash);

    std::wstring command1 = L"insert into users(login) values('--username--');";
    std::wstring command2 = L"insert into pass(id_user, hash) values(--usernid--, 0x--usergash--);";
    wStringReplacer(command1, L"--username--", username);
    wStringReplacer(command2, L"--usernid--", std::to_wstring(db_getMaxUserID() + 1));
    wStringReplacer(command2, L"--usergash--", userhash);

    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)command1.c_str(), SQL_NTS))
        return false;

    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)command2.c_str(), SQL_NTS))
        return false;

    return true;
}

bool Database::db_getUserHash(Hash &server_hash, const std::wstring &username)
{
    SQLCHAR hash[20]; // SQLCHAR is unsigned char
    std::wstring command = LR"(  with userid as
                                (
                                    select id
                                    from users
                                    where login = '--username--'
                                )
                                select pass.hash
                                from pass
                                join userid on pass.id_user = userid.id)";

    wStringReplacer(command, L"--username--", username);

    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)command.c_str(), SQL_NTS))
        return false;

    SQLBindCol(sqlStmtHandle, 1, SQL_C_BINARY, &hash, sizeof(hash), NULL);
    SQLFetch(sqlStmtHandle);

    server_hash = arr_to_Hash(hash);
    return true;
}

int Database::db_getPrivateMsgCnt(const std::wstring &username)
{
    SQLINTEGER messages_cnt;
    std::wstring command = LR"( with userid as
                                (
                                    select id
                                    from users
                                    where login = '--username--'
                                )
                                select count(private_msg.id)
                                from private_msg
                                join userid on
                                            private_msg.id_reciever = userid.id or
                                            private_msg.id_sender = userid.id;)";

    wStringReplacer(command, L"--username--", username);

    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)command.c_str(), SQL_NTS))
        return -1;

    SQLBindCol(sqlStmtHandle, 1, SQL_INTEGER, &messages_cnt, sizeof(messages_cnt), NULL);
    SQLFetch(sqlStmtHandle);

    return (int)messages_cnt;
}

bool Database::db_getAuthrorAndMessage_pub(std::queue<std::wstring> &author_queue, std::queue<std::wstring> &message_queue, const int &client_msg_cnt, const int &server_msg_cnt)
{
    SQLCHAR author[30];
    SQLCHAR message[SQL_TEXT_SIZE];
    SQLLEN indicator;

    std::wstring command = LR"( select
                                    users.login,
                                    public_msg.message
                                from
                                    public_msg
                                join users on public_msg.id_sender = users.id;)";

    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)command.c_str(), SQL_NTS))
        return false;

    SQLBindCol(sqlStmtHandle, 1, SQL_CHAR, &author, sizeof(author), &indicator);
    SQLBindCol(sqlStmtHandle, 2, SQL_CHAR, &message, sizeof(message), &indicator);

    for (auto i{0}; i < client_msg_cnt; i++)
    {
        SQLFetch(sqlStmtHandle);
    }

    for (auto i{ client_msg_cnt }; i < server_msg_cnt; i++)
    {
        SQLFetch(sqlStmtHandle);

        std::string str_author(reinterpret_cast<char*>(author));
        std::string str_message(reinterpret_cast<char*>(message));

        author_queue.push(std::wstring(str_author.begin(), str_author.end()));
        message_queue.push(std::wstring(str_message.begin(), str_message.end()));
    }

    return true;
}

bool Database::db_getAuthrorAndMessage_priv(std::queue<std::wstring> &sender_queue, std::queue<std::wstring> &reciever_queue, std::queue<std::wstring> &message_queue, std::queue<bool> &flag_queue, const int &client_msg_cnt, const int &server_msg_cnt, const std::wstring &username)
{
    std::wstring command = LR"( select
                                sender.login AS sender_login,
                                receiver.login AS receiver_login,
                                private_msg.message,
                                private_msg.flag
                                from private_msg
                                join users AS sender ON private_msg.id_sender = sender.id
                                join users AS receiver ON private_msg.id_reciever = receiver.id
                                where sender.login = '--user--' or  receiver.login = '--user--';)";

    wStringReplacer(command, L"--user--", username);
    wStringReplacer(command, L"--user--", username);

    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)command.c_str(), SQL_NTS))
        return false;

    SQLCHAR sender[30];
    SQLCHAR reciever[30];
    SQLCHAR message[SQL_TEXT_SIZE];
    SQLCHAR msgFlag;
    SQLLEN indicator;

    SQLBindCol(sqlStmtHandle, 1, SQL_CHAR, &sender, sizeof(sender), &indicator);
    SQLBindCol(sqlStmtHandle, 2, SQL_CHAR, &reciever, sizeof(reciever), &indicator);
    SQLBindCol(sqlStmtHandle, 3, SQL_CHAR, &message, sizeof(message), &indicator);
    SQLBindCol(sqlStmtHandle, 4, SQL_C_UTINYINT, &msgFlag, sizeof(msgFlag), &indicator);

    for (auto i{0}; i < client_msg_cnt; i++)
    {
        SQLFetch(sqlStmtHandle);
    }

    for (auto i{ client_msg_cnt }; i < server_msg_cnt; i++)
    {
        SQLFetch(sqlStmtHandle);

        std::string str_sender(reinterpret_cast<char*>(sender));
        std::string str_reciever(reinterpret_cast<char*>(reciever));
        std::string str_message(reinterpret_cast<char*>(message));

        sender_queue.push(std::wstring(str_sender.begin(), str_sender.end()));
        reciever_queue.push(std::wstring(str_reciever.begin(), str_reciever.end()));
        message_queue.push(std::wstring(str_message.begin(), str_message.end()));
        flag_queue.push((bool)msgFlag);
    }
    return true;
}

bool Database::db_getUsernames(std::queue<std::wstring> &names_queue, const int& users_cnt)
{
    std::wstring command = LR"( select login
                                from users;)";

    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)command.c_str(), SQL_NTS))
        return false;

    SQLCHAR buffer[30];
    SQLLEN indicator;
    SQLBindCol(sqlStmtHandle, 1, SQL_CHAR, &buffer, sizeof(buffer), &indicator);

    for (auto i{ 0 }; i < users_cnt; i++)
    {
        SQLFetch(sqlStmtHandle);

        std::string str(reinterpret_cast<char*>(buffer));
        names_queue.push(std::wstring(str.begin(), str.end()));
    }

    return true;
}

bool Database::db_setMsgReaded(const std::wstring &username, const int &poz)
{
    std::wstring command = LR"( update private_msg
                                set flag = true
                                where private_msg.id =
                                    (
                                        select msg_for_user.id
                                        from(
                                                select ROW_NUMBER() over(order by id_sender) as total, private_msg.*
                                                from private_msg
                                                join(
                                                    select id
                                                    from users
                                                    where login = '--username--'
                                                    ) as userid
                                                    on userid.id = private_msg.id_reciever
                                                ) as msg_for_user
                                        where msg_for_user.total = --msgpoz--
                                    );)";

    wStringReplacer(command, L"--username--", username);
    wStringReplacer(command, L"--msgpoz--", std::to_wstring(poz));

    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)command.c_str(), SQL_NTS))
        return false;
    else
        return true;
}

int Database::db_getUsersCount()
{
    SQLINTEGER users_cnt;
    std::wstring command = LR"( select count(id)
                                from users;)";

    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)command.c_str(), SQL_NTS))
        return -1;

    SQLBindCol(sqlStmtHandle, 1, SQL_INTEGER, &users_cnt, sizeof(users_cnt), NULL);
    SQLFetch(sqlStmtHandle);

    return (int)users_cnt;
}

bool Database::db_addPrivateMsg(const std::wstring &sender, const std::wstring &reciever, const std::wstring &mesage)
{
    std::wstring command = LR"(insert into private_msg(id_sender, id_reciever, message, flag)
                                    values(--user_1_id--, --user_2_id--, '--message--', --flag--);)";

    wStringReplacer(command, L"--user_1_id--", std::to_wstring(db_getUserID(sender)));
    wStringReplacer(command, L"--user_2_id--", std::to_wstring(db_getUserID(reciever)));
    wStringReplacer(command, L"--message--", mesage);
    if(sender == reciever)
        wStringReplacer(command, L"--flag--", L"true");
    else
        wStringReplacer(command, L"--flag--", L"false");


    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)command.c_str(), SQL_NTS))
        return false;
    else
        return true;
}

bool Database::db_addPublicMsg(const std::wstring &sender, const std::wstring &mesage)
{
    std::wstring command = LR"(insert into public_msg(message, id_sender)
                                    values('--message--', --sender--);)";

    wStringReplacer(command, L"--message--", mesage);
    wStringReplacer(command, L"--sender--", std::to_wstring(db_getUserID(sender)));

    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)command.c_str(), SQL_NTS))
        return false;
    else
        return true;
}

bool Database::db_userExists(const std::wstring &username, bool& userExists_flag)
{
    SQLINTEGER users_cnt;
    std::wstring command = LR"( select count(id)
                                from users
                                where login = '--username--';)";

    wStringReplacer(command, L"--username--", username);

    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)command.c_str(), SQL_NTS))
        return false;

    SQLBindCol(sqlStmtHandle, 1, SQL_INTEGER, &users_cnt, sizeof(users_cnt), NULL);
    SQLFetch(sqlStmtHandle);

    userExists_flag = (users_cnt == 0) ? false : true;

    return true;
}

int Database::db_getPublicMsgCnt()
{
    SQLINTEGER messages_cnt;
    std::wstring command = LR"( select count(id)
                                from public_msg;)";

    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)command.c_str(), SQL_NTS))
        return -1;

    SQLBindCol(sqlStmtHandle, 1, SQL_INTEGER, &messages_cnt, sizeof(messages_cnt), NULL);
    SQLFetch(sqlStmtHandle);

    return (int)messages_cnt;
}
