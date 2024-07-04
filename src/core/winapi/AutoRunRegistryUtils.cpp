#include "AutoRunRegistryUtils.h"

bool IsWindowsGreaterThen(int version)
{
    OSVERSIONINFOEXW osvi = {};
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    DWORDLONG const dwlConditionMask = VerSetConditionMask(
        VerSetConditionMask(
            VerSetConditionMask(
                0, VER_MAJORVERSION, VER_GREATER_EQUAL),
            VER_MINORVERSION, VER_GREATER_EQUAL),
        VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

    osvi.dwMajorVersion = HIBYTE(version);
    osvi.dwMinorVersion = LOBYTE(version);
    osvi.wServicePackMajor = 0;

    return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, dwlConditionMask) != FALSE;
}

BOOL addParserToRegistryAutoRun()
{
    TCHAR szExeName[MAX_PATH];
    TCHAR fileName[12] = L"\\parser.exe";
    rsize_t stringSize = MAX_PATH;
    GetModuleFileName(NULL, szExeName, stringSize);

    if (FAILED(PathCchRemoveFileSpec(szExeName, stringSize)))
        return FALSE;

    _tcscat_s(szExeName, stringSize, fileName);
    return SetRegistryAutoRunKey(szExeName);

}

BOOL SetRegistryAutoRunKey(LPWSTR path)
{
    LPCWSTR lpSubKey = TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
    LONG result = NULL;
    DWORD pResult = NULL;
    HKEY hKey = NULL;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if ((RegOpenKeyEx(HKEY_CURRENT_USER, lpSubKey, 0, KEY_READ, &hKey)) != ERROR_SUCCESS) {
        ReportError(VIEWER_REGISTRY_OPEN_ERROR_DESCRIPTION, VIEWER_REGISTRY_OPEN_ERROR);
        RegCloseKey(hKey);
        return FALSE;
    }

    if ((RegCreateKeyEx(HKEY_CURRENT_USER, lpSubKey, 0, REG_OPTION_NON_VOLATILE, NULL, KEY_ALL_ACCESS, NULL, &hKey, &pResult)) != ERROR_SUCCESS) {
        ReportError(VIEWER_REGISTRY_CREATE_KEY_ERROR_DESCRIPTION, VIEWER_REGISTRY_CREATE_KEY_ERROR);
        RegCloseKey(hKey);
        return FALSE;
    }
    result = RegSetValueEx(hKey, TEXT("TelegramViewer"), 0, REG_SZ, (PBYTE)(path), ((LPBYTE)(path), (lstrlen(path) * sizeof(TCHAR) + 1)));
    if (result == ERROR_SUCCESS && pResult != REG_OPENED_EXISTING_KEY) {
        PrintMsg(hOut, TEXT("Приложение успешно добавлено в автозагрузку.\n"));
    }
    else if (result == ERROR_SUCCESS && pResult == REG_OPENED_EXISTING_KEY) {
        ReportError(REGISTRY_ALREADY_TAKEN_AUTO_LOAD_VALUES_WARNING_DESCRIPTION, REGISTRY_ALREADY_TAKEN_AUTO_LOAD_VALUES_WARNING);
    }
    else
    {
        ReportError(VIEWER_REGISTRY_SET_VALUE_ERROR_DESCRIPTION, VIEWER_REGISTRY_SET_VALUE_ERROR);
    }
    RegCloseKey(hKey);
    return TRUE;
}