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
//функция отправки случайной цитаты
void sendRandomQuote(const Bot& bot, int64_t chatId, const vector<string>& quote)


int main()
{
    setlocale(LC_ALL, "Ru");

}

