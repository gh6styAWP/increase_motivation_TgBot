#include <tgbot/tgbot.h>
#include <chrono>
#include <thread>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <sqlite3.h>
using namespace std;
using namespace TgBot;
using namespace chrono;

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
vector<int64_t> getAllUserChatIds(sqlite3* db) {
    vector<int64_t> chatIds;
    const char* selectQuery = "SELECT chat_id FROM Users;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, selectQuery, -1, &stmt, nullptr);

    while (sqlite3_step(stmt) == SQLITE_ROW)
        chatIds.push_back(sqlite3_column_int64(stmt, 0));

    sqlite3_finalize(stmt);
    return chatIds;
}
//функция отправки случайной цитаты
void sendQuote(const Bot& bot, int64_t chatId, const vector<string>& quotes) {
    int index = rand() % quotes.size();
    bot.getApi().sendMessage(chatId, quotes[index]);
}
//функция отправки цитат по расписанию
void startScheduledQuotes(Bot& bot, sqlite3* db, const vector<string>& quotes) {
    vector<pair<int, int>> schedule = { {10, 0}, {14, 0}, {19, 0} };

    while (true) {
        auto now = system_clock::now();
        auto nowTime = system_clock::to_time_t(now);
        tm* localTime = localtime(&nowTime);

        for (const auto& time : schedule) {
            if (localTime->tm_hour == time.first && localTime->tm_min == time.second) {
                vector<int64_t> chatIds = getAllUserChatIds(db);
                for (int64_t chatId : chatIds) 
                    sendQuote(bot, chatId, quotes);
                this_thread::sleep_for(minutes(1));
            }
        }
        this_thread::sleep_for(seconds(30));
    }
}
int main()
{
    setlocale(LC_ALL, "Ru");

}

