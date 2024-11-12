#include <tgbot/tgbot.h>
#include <fstream>
#include <sqlite3.h>
#include <thread>
#include <chrono>
#include <random>

using namespace std;
using namespace TgBot;
using namespace chrono;

//вытягивание токена из файла
string getToken(const string& filePath) {
    ifstream file(filePath);
    string token;

    if (file)
        getline(file, token);

    return token;
}
//вытягивание цитаты из файла
vector<string>loadQuotes(const string& filePath) {
    ifstream file(filePath);
    vector<string> quotes;
    string line;

    while (getline(file, line))
        quotes.push_back(line);

    return quotes;
}
//инициализация БД
void setupDB(sqlite3*& db) {
    if (sqlite3_open("bot_users.db", &db) != SQLITE_OK)
        throw runtime_error("Failed to open database");

    const char* createTableSQL = "CREATE TABLE IF NOT EXISTS Users (ChatId INTEGER PRIMARY KEY);";

    if (sqlite3_exec(db, createTableSQL, 0, 0, nullptr) != SQLITE_OK)
        throw runtime_error("Failed to create table");
}
//добавление пользователей в БД
void addUser(sqlite3* db, int64_t chatId) {
    string sql = "INSERT OR IGNORE INTO Users (ChatId) VALUES (" + to_string(chatId) + ");";

    if (sqlite3_exec(db, sql.c_str(), 0, 0, nullptr) != SQLITE_OK)
        throw runtime_error("Failed to add user");
}
//вытягивание пользователей из БД
vector<int64_t>getUser(sqlite3* db) {
    vector<int64_t> chatIds;
    const char* sql = "SELECT ChatId FROM Users;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW)
            chatIds.push_back(sqlite3_column_int64(stmt, 0));
    }

    sqlite3_finalize(stmt);
    return chatIds;
}

int main() {
    setlocale(LC_ALL, "Ru");

    
}
