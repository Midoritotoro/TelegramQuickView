#pragma once

#define PY_SSIZE_T_CLEAN

#include <Python.h>

class PythonCaller
{
private:
    PyObject* PyName = nullptr, * PyModule = nullptr, * PyDict = nullptr, * PyFunc = nullptr,
        * PyArgs = nullptr, * PySys = nullptr, * PyPath = nullptr, * PyClass = nullptr, * PyClsInstance = nullptr;
    const char* _pythonFilePath = nullptr;
public:
    PythonCaller(const char* pythonFilePath);
    ~PythonCaller();

    void CallTelegramParseFunction(const char* moduleName, const char* pathToSettingsJsonFile, const char* pathToAppRootDirectory);
};
