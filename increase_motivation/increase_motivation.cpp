#include <tgbot/tgbot.h>
#include <chrono>
#include <thread>
#include <fstream>
#include <vector>
#include <cstdlib>
using namespace std;

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

int main()
{
    setlocale(LC_ALL, "Ru");

}

