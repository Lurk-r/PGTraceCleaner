#include "utils.h"
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <algorithm>
#include <cctype>
bool isPixelGunRunning() {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return false;
    }
    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    if (Process32FirstW(snapshot, &pe32)) {
        do {
            std::wstring processName(pe32.szExeFile);
            std::transform(processName.begin(), processName.end(), processName.begin(), ::tolower);
            if (processName.find(L"pixel") != std::wstring::npos &&
                processName.find(L"gun") != std::wstring::npos &&
                processName.find(L".exe") != std::wstring::npos) {
                CloseHandle(snapshot);
                return true;
            }
        } while (Process32NextW(snapshot, &pe32));
    }
    CloseHandle(snapshot);
    return false;
}