#include <tgbot/tgbot.h>
#include <chrono>
#include <thread>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <sqlite3.h>
using namespace std;
using namespace TgBot;

//функция вытягивания цитат из текстового файла
vector<string> loadQuotesFromFile(const string& filePath) {
    vector<string> quotes;
    ifstream file(filePath);
    string line;

    while (getline(file, line)) {
        if (!line.empty())
            quotes.push_back(line);
    }
    return quotes;
}
//функция вытягивания токена из файла
string getToken(const string& filePath) {
    ifstream file(filePath);
    string token;

    if (file.is_open())
        getline(file, token);
    return token;
}
//функция инициализации БД и создания таблицы, если её нет.
void initDatabase(sqlite3*& db) {
    sqlite3_open("users.db", &db);
    const char* createTableQuery = "CREATE TABLE IF NOT EXISTS Users (chat_id INTEGER PRIMARY KEY);";
    sqlite3_exec(db, createTableQuery, nullptr, nullptr, nullptr);
}
//функция добавления chat_id пользователя в БД.
void registerUser(sqlite3* db, int64_t chatId) {
    string insertQuery = "INSERT OR IGNORE INTO Users (chat_id) VALUES (" + to_string(chatId) + ");";
    sqlite3_exec(db, insertQuery.c_str(), nullptr, nullptr, nullptr);
}
//функция получения всех chat_id из БД
vector<int64_t> getAllUsersChatIds(sqlite3* db) {
    vector<int64_t> chatIds;
    const char* selectQuery = "SELECT chat_id FROM Users;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, selectQuery, -1, &stmt, nullptr);

    while (sqlite3_step(stmt) == SQLITE_ROW)
        chatIds.push_back(sqlite3_column_int64(stmt, 0));

    sqlite3_finalize(stmt);
    return chatIds;
}

int main()
{
    setlocale(LC_ALL, "Ru");

}

