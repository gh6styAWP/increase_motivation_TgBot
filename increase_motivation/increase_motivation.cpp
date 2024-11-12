#include <tgbot/tgbot.h>
#include <fstream>
#include <sqlite3.h>
#include <thread>
#include <chrono>
#include <random>

using namespace std;
using namespace TgBot;
using namespace chrono;

//функция вытягивания токена из файла
string getToken(const string& filePath) {
    ifstream file(filePath);
    string token;

    if (file)
        getline(file, token);
    return token;
}
//функция вытягивания цитаты
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

int main() {
    setlocale(LC_ALL, "Ru");

    
}
