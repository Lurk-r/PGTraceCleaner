#include "utils.h"
#include <shlwapi.h>
#include <algorithm>
#pragma comment(lib, "Shlwapi.lib")
const std::vector<std::string> WILDCARD_PATTERNS = {
    "*ä*", "*acc*", "*afd*", "*anal*", "*auth*", "*clan*", "*cloud*", "*contine*", "*count*", "*crash*", "*data*", "*event*", "*face*", "*key*", "*histor*"
    "*ids*", "*indx*", "*inhouse*", "*item*", "*last*", "*lead*", "*lvi*", "*mail*", "*map*", "*mark*", "*new*", "*notif*", "*offs*", "*old*", "*permi*", "*perso*",
    "*player*", "*sched*", "*send*", "*session*", "*skin*", "*shop*", "*slot*", "*start*", "*stat*", "*steam*", "*terc*", "*time*", "*user*", "*version*"
};
const std::vector<std::regex> REGEX_PATTERNS = {
    std::regex(".*[ ._-]+\\d+[ ._-]+.*", std::regex::icase),
    std::regex(".*\\d+[ ._-]+.*", std::regex::icase)
};
bool matchesPattern(const std::string& name, const std::vector<std::string>& wildcards, const std::vector<std::regex>& regexes) {
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    if (lowerName.find('\xe4') != std::string::npos || // ä
        lowerName.find('\x84') != std::string::npos || // ä
        lowerName.find("ae") != std::string::npos) {   // Replacement
        return true;
    }
    for (const auto& pattern : wildcards) {
        if (PathMatchSpecA(lowerName.c_str(), pattern.c_str())) {
            return true;
        }
    }
    for (const auto& regex : regexes) {
        if (std::regex_match(lowerName, regex)) {
            return true;
        }
    }
    return false;
}
std::pair<int, int> deleteRegistryValues(HKEY baseKey, const std::string& subKeyPath) {
    HKEY hKey;
    LONG result = RegOpenKeyExA(baseKey, subKeyPath.c_str(), 0, KEY_READ | KEY_WRITE, &hKey);
    if (result == ERROR_FILE_NOT_FOUND) {
        log("Registry key '" + subKeyPath + "' not found. Skipping...", LogLevel::INFO);
        return { 0, 0 };
    }
    else if (result != ERROR_SUCCESS) {
        log("Failed to open registry key '" + subKeyPath + "'.", LogLevel::ERR);
        return { 0, 0 };
    }
    std::vector<std::string> toDelete;
    DWORD index = 0;
    wchar_t valueName[16384];
    DWORD valueNameSize;
    while (true) {
        valueNameSize = sizeof(valueName) / sizeof(wchar_t);
        result = RegEnumValueW(hKey, index, valueName, &valueNameSize, NULL, NULL, NULL, NULL);
        if (result == ERROR_NO_MORE_ITEMS) {
            break;
        }
        else if (result == ERROR_SUCCESS) {
            // Convert wide string to UTF-8
            int size_needed = WideCharToMultiByte(CP_UTF8, 0, valueName, -1, NULL, 0, NULL, NULL);
            std::string utf8Name(size_needed - 1, 0);
            WideCharToMultiByte(CP_UTF8, 0, valueName, -1, &utf8Name[0], size_needed, NULL, NULL);
            if (matchesPattern(utf8Name, WILDCARD_PATTERNS, REGEX_PATTERNS)) {
                toDelete.push_back(utf8Name);
            }
            index++;
        }
        else {
            break;
        }
    }
    if (toDelete.empty()) {
        log("Registry traces not found. Skipping...", LogLevel::INFO);
        RegCloseKey(hKey);
        return { 0, 0 };
    }
    log("Found " + std::to_string(toDelete.size()) + " registry traces in '" + subKeyPath + "'. Deleting...", LogLevel::INFO);
    int cleared = 0;
    for (const auto& valueName : toDelete) {
        // Convert back to wide string for deletion
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, valueName.c_str(), -1, NULL, 0);
        std::wstring wValueName(size_needed - 1, 0);
        MultiByteToWideChar(CP_UTF8, 0, valueName.c_str(), -1, &wValueName[0], size_needed);
        if (RegDeleteValueW(hKey, wValueName.c_str()) == ERROR_SUCCESS) {
            cleared++;
        }
        else {
            log("Failed to delete registry value '" + valueName + "'.", LogLevel::ERR);
        }
    }
    log(std::to_string(cleared) + " registry traces deleted.", LogLevel::SUCCESS);
    RegCloseKey(hKey);
    return { static_cast<int>(toDelete.size()), cleared };
}
std::vector<std::string> findRegistrySubkeys(HKEY baseKey, const std::string& parentSubkey, const std::string& pattern) {
    std::vector<std::string> matched;
    HKEY hKey;
    if (RegOpenKeyExA(baseKey, parentSubkey.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        return matched;
    }
    DWORD index = 0;
    char subkeyName[256];
    DWORD subkeyNameSize;
    while (true) {
        subkeyNameSize = sizeof(subkeyName);
        LONG result = RegEnumKeyExA(hKey, index, subkeyName, &subkeyNameSize, NULL, NULL, NULL, NULL);
        if (result == ERROR_NO_MORE_ITEMS) {
            break;
        }
        else if (result == ERROR_SUCCESS) {
            std::string subkey(subkeyName);
            std::transform(subkey.begin(), subkey.end(), subkey.begin(), ::tolower);
            if (PathMatchSpecA(subkey.c_str(), pattern.c_str())) {
                matched.push_back(subkeyName);
            }
            index++;
        }
        else {
            break;
        }
    }
    RegCloseKey(hKey);
    return matched;
}