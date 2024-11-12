#include <tgbot/tgbot.h>
#include <fstream>
#include <sqlite3.h>
#include <thread>
#include <chrono>
#include <random>
#include <mutex>

using namespace std;
using namespace TgBot;
using namespace chrono;

// мьютекс для синхронизации доступа к базе данных
mutex dbMutex;

//вытягивание токена из файла
string getToken(const string& filePath) {
    ifstream file(filePath);
    string token;

    if (file)
        getline(file, token);

    return token;
}

//вытягивание цитаты из файла
vector<string> getQuotes(const string& filePath) {
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
    lock_guard<mutex> lock(dbMutex);
    string sql = "INSERT OR IGNORE INTO Users (ChatId) VALUES (" + to_string(chatId) + ");";

    if (sqlite3_exec(db, sql.c_str(), 0, 0, nullptr) != SQLITE_OK)
        throw runtime_error("Failed to add user");
}

//вытягивание пользователей из БД
vector<int64_t> getUser(sqlite3* db) {
    lock_guard<mutex> lock(dbMutex);
    vector<int64_t> chatIds;
    const char* sql = "SELECT ChatId FROM Users;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW)
            chatIds.push_back(sqlite3_column_int64(stmt, 0));
    }
    else {
        throw runtime_error("Failed to execute select statement");
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

        tm parts;
        localtime_s(&parts, &time);

        if (parts.tm_hour == 10 || parts.tm_hour == 15 || parts.tm_hour == 20) {
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

    // Путь к файлу с токеном
    string filePath = "C:\\Users\\gafar\\Desktop\\tokenChris.txt";
    string token;

    try {
        // Попытка загрузки токена
        token = getToken(filePath);
        if (token.empty()) {
            throw runtime_error("Файл с токеном пустой или токен не найден.");
        }
        cerr << "Токен загружен успешно: " << token << endl;
    }
    catch (const exception& e) {
        cerr << "Ошибка при загрузке токена: " << e.what() << endl;
        return 1; // Завершить программу с ошибкой
    }

    try {
        // Инициализация бота
        Bot bot(token);
        cerr << "Бот подключен к Telegram API." << endl;

        // Дополнительно загрузить цитаты и инициализировать базу данных
        vector<string> quotes = getQuotes("C:\\Users\\gafar\\Desktop\\citata.txt");
        if (quotes.empty()) {
            throw runtime_error("Цитаты не найдены или файл пустой");
        }
        cerr << "Цитаты загружены успешно. Количество цитат: " << quotes.size() << endl;

        sqlite3* db;
        setupDB(db);
        cerr << "База данных инициализирована." << endl;

        // Команда для подписки на мотивационные цитаты
        bot.getEvents().onCommand("start", [&bot, db](Message::Ptr message) {
            try {
                addUser(db, message->chat->id);
                bot.getApi().sendMessage(message->chat->id, "Вы подписаны на мотивационные цитаты!");
                cerr << "Пользователь добавлен: " << message->chat->id << endl;
            }
            catch (const TgBot::TgException& e) {
                cerr << "Ошибка TgException при добавлении пользователя: " << e.what() << endl;
            }
            catch (const exception& e) {
                cerr << "Ошибка при добавлении пользователя: " << e.what() << endl;
            }
            });

        cerr << "Запуск планировщика сообщений..." << endl;
        thread senderThread(sendQuotes, ref(bot), db, quotes);

        cerr << "Запуск опроса сообщений..." << endl;
        TgLongPoll longPoll(bot);
        while (true) {
            try {
                longPoll.start();
            }
            catch (const TgBot::TgException& e) {
                cerr << "Ошибка в LongPoll: " << e.what() << endl;
            }
            catch (const exception& e) {
                cerr << "Неожиданная ошибка в LongPoll: " << e.what() << endl;
            }
        }

        senderThread.join();
        sqlite3_close(db);
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }

    return 0;
}

