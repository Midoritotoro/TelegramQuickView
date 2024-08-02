#pragma once

#define PY_SSIZE_T_CLEAN

#pragma push_macro("slots")
#undef slots
#include "Python.h"
#pragma pop_macro("slots")


class TelegramRobber
{
private:
    PyObject* PyName = nullptr, * PyModule = nullptr, * PyDict = nullptr, * PyFunc = nullptr,
        * PyArgs = nullptr, * PySys = nullptr, * PyPath = nullptr, * PyClass = nullptr, * PyClsInstance = nullptr;
    const char* _pythonFilePath = nullptr;
public:
    TelegramRobber(const char* pythonFilePath);
    ~TelegramRobber();

    void CallTelegramParseFunction(const char* moduleName, const char* pathToSettingsJsonFile, const char* pathToAppRootDirectory);
};
