#include "utils.h"
#include <iostream>
namespace {
    WORD colorForLevel(LogLevel level) {
        switch (level) {
        case LogLevel::WARN:    return FOREGROUND_RED | FOREGROUND_GREEN;
        case LogLevel::ERR:     return FOREGROUND_RED | FOREGROUND_INTENSITY;
        case LogLevel::SUCCESS: return FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        default:                return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        }
    }
}
void log(const std::string& msg, LogLevel level) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, colorForLevel(level));
    switch (level) {
    case LogLevel::WARN:    std::cout << "[WARN] "; break;
    case LogLevel::ERR:     std::cout << "[ERROR] "; break;
    case LogLevel::SUCCESS: std::cout << "[SUCCESS] "; break;
    default:                std::cout << "[INFO] "; break;
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
