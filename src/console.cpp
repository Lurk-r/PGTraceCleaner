#include "utils.h"
#include <windows.h>
#include <iostream>
void log(const std::string& msg, LogLevel level) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    switch (level) {
    case LogLevel::INFO:
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        std::cout << "[INFO] ";
        break;
    case LogLevel::WARN:
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
        std::cout << "[WARN] ";
        break;
    case LogLevel::ERR:
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
        std::cout << "[ERROR] ";
        break;
    case LogLevel::SUCCESS:
        SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout << "[SUCCESS] ";
        break;
    }
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    std::cout << msg << std::endl;
}
void clearConsole() {
    system("cls");
}
void setConsoleTitle(const std::string& title) {
    SetConsoleTitleA(title.c_str());
}