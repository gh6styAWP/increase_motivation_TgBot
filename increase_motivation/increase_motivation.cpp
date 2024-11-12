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


int main() {
    setlocale(LC_ALL, "Ru");

    
}
