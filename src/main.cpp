#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <regex>
#include "utils.h"
int main() {
    clearConsole();
    setConsoleTitle("PGTC - Pixel Gun Trace Cleaner");
    std::cout << "> Pixel Gun Trace Cleaner | by .lurk_r <+>\n";
    std::cout << "- PGTC reduces ban chances on new account.\n";
    std::cout << "- Run this before creating/playing on new account.\n";
    std::cout << "- Join discord.gg/GYSdwzQtD3\n\n";
    if (isPixelGunRunning()) {
        log("Pixel Gun is running. Please close it before running this script.", LogLevel::ERR);
        std::cout << "Press Enter to exit.";
        std::cin.get();
        return 1;
    }
    auto pgDir = findPixelGunFolder();
    std::string accIdStr = "N/A";
    std::vector<std::filesystem::path> userDirs;
    if (!pgDir.empty()) {
        log("Found Pixel Gun logs folder '" + pgDir.string() + "'. Deleting...", LogLevel::INFO);
        userDirs = findPixelGunIds(pgDir, 3);
    }
    std::vector<int> accIds;
    std::regex pattern("user.*?(\\d+)");
    for (const auto& dir : userDirs) {
        std::string dirName = dir.filename().string();
        std::smatch match;
        if (std::regex_match(dirName, match, pattern)) {
            try {
                accIds.push_back(std::stoi(match[1]));
            }
            catch (...) {
                continue;
            }
        }
    }
    if (!accIds.empty()) {
        accIdStr = "";
        for (size_t i = 0; i < accIds.size(); ++i) {
            if (i > 0) accIdStr += ", ";
            accIdStr += std::to_string(accIds[i]);
        }
    }
    else {
        log("Pixel Gun ID not found. Skipping...", LogLevel::INFO);
    }
    int logsCleared = 0;
    if (!pgDir.empty() && std::filesystem::exists(pgDir.parent_path())) {
        if (clearPixelGunLogs(pgDir)) {
            logsCleared = 1;
        }
    }
    else {
        log("Pixel Gun logs folder not found. Skipping...", LogLevel::INFO);
    }
    int tempDeleted = clearPixelGunTemp();
    int registryCleared = 0;
    const std::string parentRegPath = "Software\\Pixel Gun Team";
    std::vector<std::string> subkeyPatterns = { "pixel*gun*", "pg*" };
    std::set<std::string> matchingSubkeys;
    for (const auto& pattern : subkeyPatterns) {
        auto subkeys = findRegistrySubkeys(HKEY_CURRENT_USER, parentRegPath, pattern);
        matchingSubkeys.insert(subkeys.begin(), subkeys.end());
    }
    if (!matchingSubkeys.empty()) {
        int totalMatching = 0;
        int totalDeleted = 0;
        for (const auto& subkey : matchingSubkeys) {
            auto [matching, deleted] = deleteRegistryValues(HKEY_CURRENT_USER, parentRegPath + "\\" + subkey);
            totalMatching += matching;
            totalDeleted += deleted;
        }
        if (totalDeleted > 0) {
            registryCleared = 1;
        }
    }
    else {
        log("Registry key not found. Skipping...", LogLevel::INFO);
    }
    int completed = logsCleared + (tempDeleted > 0 ? 1 : 0) + registryCleared;
    std::cout << std::endl;
    log("Found ID: " + accIdStr, LogLevel::INFO);
    std::cout << "[" << completed << " / 3] Cleanup completed. Press Enter to exit.";
    std::cin.get();
    return 0;
}