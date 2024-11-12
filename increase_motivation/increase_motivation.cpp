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
vector<string>getQuotes(const string& filePath) {
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
//отправка цитаты
void sendQuotes(Bot& bot, sqlite3* db, const vector<string>& quotes) {
    size_t quoteIndex = 0;
    while (true) {
        
        auto now = chrono::system_clock::now();
        time_t time = chrono::system_clock::to_time_t(now);
        tm* parts = localtime(&time);

        if (parts->tm_hour == 10 || parts->tm_hour == 15 || parts->tm_hour == 20) {
            auto users = getUser(db);

            if (!users.empty()) {
                string quote = quotes[quoteIndex];

                for (int64_t chatId : users) 
                    bot.getApi().sendMessage(chatId, quote);
                
                quoteIndex = (quoteIndex + 1) % quotes.size();
            }
            this_thread::sleep_for(chrono::hours(1));
        }
        this_thread::sleep_for(chrono::minutes(1));
    }
}

int main() {
    setlocale(LC_ALL, "Ru");
    
    try {
        string token = getToken("C://Users//gafar//Desktop//tokenChris.txt");
        vector<string> quotes = getQuotes("C://Users//gafar//Desktop//citata.txt");

        Bot bot(token);

        sqlite3* db;
        setupDB(db);

        bot.getEvents().onCommand("start", [&bot, db](Message::Ptr message) {
            addUser(db, message->chat->id);
            bot.getApi().sendMessage(message->chat->id, "You have been subscribed to motivational quotes!");
            });

        thread senderThread(sendQuotes, ref(bot), db, quotes);

        TgLongPoll longPoll(bot);
        while (true) 
            longPoll.start();
        
        senderThread.join();
        sqlite3_close(db);
    }

    catch (exception& e) {
        cerr << "error: " << e.what() << endl;
    }

    return 0;
}
