#pragma once
#include <windows.h>
#include <filesystem>
#include <string>
#include <vector>
#include <regex>
#include <utility>
enum class LogLevel {
    INFO,
    WARN,
    ERR,
    SUCCESS
};
void log(const std::string& msg, LogLevel level = LogLevel::INFO);
void clearConsole();
void setConsoleTitle(const std::string& title);
bool isPixelGunRunning();
std::pair<int, int> deleteRegistryValues(HKEY baseKey, const std::string& subKeyPath);
std::vector<std::string> findRegistrySubkeys(HKEY baseKey, const std::string& parentSubkey, const std::string& pattern);
std::filesystem::path findPixelGunFolder();
std::vector<std::filesystem::path> findPixelGunIds(const std::filesystem::path& rootDir, int maxDepth = 3);
bool clearPixelGunLogs(const std::filesystem::path& pgDir);
int clearPixelGunTemp();
extern const std::vector<std::string> WILDCARD_PATTERNS;
extern const std::vector<std::regex> REGEX_PATTERNS;
bool matchesPattern(const std::string& name, const std::vector<std::string>& wildcards, const std::vector<std::regex>& regexes);