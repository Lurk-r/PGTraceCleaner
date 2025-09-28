#include "utils.h"
#include <windows.h>
#include <shlobj.h>
#include <queue>
#include <algorithm>
#include <regex>
std::filesystem::path findPixelGunFolder() {
    wchar_t appDataPath[MAX_PATH];
    if (SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, appDataPath) != S_OK) {
        return {};
    }
    std::filesystem::path localLow = std::filesystem::path(appDataPath).parent_path() / "LocalLow";
    try {
        for (const auto& entry : std::filesystem::directory_iterator(localLow)) {
            if (!entry.is_directory()) continue;
            std::string teamName = entry.path().filename().string();
            std::transform(teamName.begin(), teamName.end(), teamName.begin(), ::tolower);
            if (teamName.find("pixel") != std::string::npos &&
                teamName.find("gun") != std::string::npos) {
                for (const auto& pgEntry : std::filesystem::directory_iterator(entry.path())) {
                    if (!pgEntry.is_directory()) continue;
                    std::string pgName = pgEntry.path().filename().string();
                    std::transform(pgName.begin(), pgName.end(), pgName.begin(), ::tolower);
                    if (pgName.find("pixel") != std::string::npos &&
                        pgName.find("gun") != std::string::npos) {
                        return pgEntry.path();
                    }
                }
            }
        }
    }
    catch (const std::exception& e) {
        log(std::string("Error finding Pixel Gun folder: ") + e.what(), LogLevel::WARN);
    }
    return {};
}
std::vector<std::filesystem::path> findPixelGunIds(const std::filesystem::path& rootDir, int maxDepth) {
    std::vector<std::filesystem::path> userDirs;
    std::queue<std::pair<std::filesystem::path, int>> queue;
    queue.push({ rootDir, 0 });
    while (!queue.empty()) {
        auto [currentPath, depth] = queue.front();
        queue.pop();
        if (depth > maxDepth) continue;
        try {
            for (const auto& entry : std::filesystem::directory_iterator(currentPath)) {
                if (!entry.is_directory()) continue;
                std::string dirName = entry.path().filename().string();
                if (dirName.substr(0, 4) == "user") {
                    userDirs.push_back(entry.path());
                }
                else {
                    queue.push({ entry.path(), depth + 1 });
                }
            }
        }
        catch (const std::exception& e) {
            log("Error accessing " + currentPath.string() + ": " + e.what(), LogLevel::WARN);
        }
    }
    return userDirs;
}
bool clearPixelGunLogs(const std::filesystem::path& pgDir) {
    if (pgDir.empty()) {
        log("Pixel Gun logs folder not found. Skipping...", LogLevel::INFO);
        return false;
    }
    try {
        std::filesystem::remove_all(pgDir.parent_path());
        log("Pixel Gun logs deleted.", LogLevel::SUCCESS);
        return true;
    }
    catch (const std::filesystem::filesystem_error& e) {
        if (e.code() == std::errc::permission_denied) {
            log("Permission denied deleting Pixel Gun logs.", LogLevel::ERR);
        }
        else {
            log(std::string("Failed to delete Pixel Gun logs: ") + e.what(), LogLevel::ERR);
        }
        return false;
    }
}
int clearPixelGunTemp() {
    wchar_t tempPath[MAX_PATH];
    if (GetTempPathW(MAX_PATH, tempPath) == 0) {
        log("Unable to get TEMP path.", LogLevel::ERR);
        return 0;
    }
    std::filesystem::path tempDir(tempPath);
    int deletedCount = 0;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(tempDir)) {
            if (!entry.is_directory()) continue;
            std::string folderName = entry.path().filename().string();
            std::transform(folderName.begin(), folderName.end(), folderName.begin(), ::tolower);
            if (folderName.find("pixel") != std::string::npos &&
                folderName.find("gun") != std::string::npos) {
                log("Found Pixel Gun temp folder '" + entry.path().string() + "'. Deleting...", LogLevel::INFO);
                try {
                    std::filesystem::remove_all(entry.path());
                    log("Pixel Gun temp folder deleted.", LogLevel::SUCCESS);
                    deletedCount++;
                }
                catch (const std::filesystem::filesystem_error& e) {
                    if (e.code() == std::errc::permission_denied) {
                        log("Permission denied deleting '" + entry.path().string() + "'.", LogLevel::ERR);
                    }
                    else {
                        log("Error deleting '" + entry.path().string() + "': " + e.what(), LogLevel::ERR);
                    }
                }
            }
        }
    }
    catch (const std::exception& e) {
        log(std::string("Error accessing temp directory: ") + e.what(), LogLevel::ERR);
    }
    if (deletedCount == 0) {
        log("Pixel Gun temp folder not found. Skipping...", LogLevel::INFO);
    }
    return deletedCount;
}