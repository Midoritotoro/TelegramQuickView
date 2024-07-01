#pragma once

#define PY_SSIZE_T_CLEAN

#include <Python.h>

class PythonCaller
{
private:
    PyObject* PyName = nullptr, * PyModule = nullptr, * PyDict = nullptr, * PyFunc = nullptr,
        * PyArgs = nullptr, *PySys = nullptr, *PyPath = nullptr, *PyClass = nullptr, *PyClsInstance = nullptr;
    const char* m_PyApiHash = nullptr, * m_PyDownloadPath = nullptr, * m_PyPhoneNumber = nullptr;
    long long m_PyApiId = 0;
public:
    PythonCaller(const char* apiHash, const char* phoneNumber, const char* downloadPath, long long apiId, const char* pythonFilePath);
    ~PythonCaller();
    int CallTelegramParseFunction(const char* mName, const char* username, const char* startDate, const char* endDate);
};


//  const char* PyModuleName = "TelegramRobber";
//  const char* TelegramUsername = "antifishechki";
//  const char* apiHash = "019edf3f20c8460b741fb94114e6fec0";
//  const char* phoneNumber = "+375292384917";
//  const char* downloadPath = "D:\\Media";
//  long long apiId = 13711370;
//  PythonCaller pythonCaller(apiHash, phoneNumber, downloadPath, apiId);
//  pythonCaller.CallTelegramParseFunction(PyModuleName, TelegramUsername, "2024-05-04", "2024-05-05");