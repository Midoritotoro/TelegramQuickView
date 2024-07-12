#include "telegramAuthorizationChecker.h"
#include <iostream>


DWORD WINAPI TelegramAuthorizationChecker::TelegramCredentialsValidCheck(LPVOID lpParam) {
    const char* PyFunctionName = "isTelegramCredentialsValid";
    const char* pythonFilePath = "D:\\TelegramQuickView\\src\\gui\\settings\\";
    const char* moduleName = "TelegramAuthorization";
    DWORD result = 0;

    LPTelegramCredentials lpTelegramCredentials = static_cast<LPTelegramCredentials>(lpParam);

    PyObject* PySys = nullptr, * PyPath = nullptr, * PyArgs = nullptr, * PyName = nullptr, * PyModule = nullptr, * PyFunc = nullptr, * PyResult = nullptr;

    PySys = PyImport_ImportModule("sys");
    PyPath = PyObject_GetAttrString(PySys, "path");
    PyList_Append(PyPath, PyUnicode_FromString(pythonFilePath));

    Py_ssize_t PyArgumentsTupleSize = 4;

    PyName = PyUnicode_FromString(moduleName);
    if (!PyName)
        return result;

    PyModule = PyImport_Import(PyName);
    if (!PyModule)
        return result;

    PyFunc = PyObject_GetAttrString(PyModule, "asyncCall");
    if (!PyFunc)
        return result;

    if (PyCallable_Check(PyFunc)) {
        PyArgs = PyTuple_Pack(PyArgumentsTupleSize, PyUnicode_FromString(PyFunctionName),
            PyLong_FromLong(long(lpTelegramCredentials->apiId)), PyUnicode_FromString(lpTelegramCredentials->phoneNumber.c_str()), PyUnicode_FromString(lpTelegramCredentials->apiHash.c_str()));

        if (PyArgs != NULL)
            PyResult = PyObject_CallObject(PyFunc, PyArgs);
        if (PyResult != NULL) {
            if (PyBool_Check(PyResult))
                if (PyObject_IsTrue(PyResult))
                    result = 1;
        }
    }

    Py_DECREF(PyResult);
    Py_DECREF(PyFunc);
    Py_DECREF(PyArgs);
    Py_DECREF(PyModule);
    Py_DECREF(PyName);

    Py_DECREF(PyPath);
    Py_DECREF(PySys);

    PyErr_Print();

    return result;
}

DWORD WINAPI TelegramAuthorizationChecker::sendTelegramCode(LPVOID lpParam){
    const char* PyFunctionName = "sendTelegramCode";
    const char* pythonFilePath = "D:\\TelegramQuickView\\src\\gui\\settings\\";
    const char* moduleName = "TelegramAuthorization";
    DWORD result = 0;

    LPTelegramCredentials lpTelegramCredentials = static_cast<LPTelegramCredentials>(lpParam);

    PyObject* PySys = nullptr, * PyPath = nullptr, *PyArgs = nullptr, * PyName = nullptr, * PyModule = nullptr, * PyFunc = nullptr, *PyResult = nullptr;

    PySys = PyImport_ImportModule("sys");
    PyPath = PyObject_GetAttrString(PySys, "path");
    PyList_Append(PyPath, PyUnicode_FromString(pythonFilePath));

    Py_ssize_t PyArgumentsTupleSize = 5;

    PyName = PyUnicode_FromString(moduleName);
    if (!PyName)
        return result;

    PyModule = PyImport_Import(PyName);
    if (!PyModule)
        return result;

    PyFunc = PyObject_GetAttrString(PyModule, "asyncCall");
    if (!PyFunc)
        return result;

    if (PyCallable_Check(PyFunc)) {
        PyArgs = PyTuple_Pack(PyArgumentsTupleSize, PyUnicode_FromString(PyFunctionName),
            PyLong_FromLong(long(lpTelegramCredentials->apiId)), PyUnicode_FromString(lpTelegramCredentials->phoneNumber.c_str()),
            PyUnicode_FromString(lpTelegramCredentials->apiHash.c_str()), PyUnicode_FromString(lpTelegramCredentials->pathToUserSettingsJson.c_str()));

        PyResult = PyObject_CallObject(PyFunc, PyArgs);
        if (PyResult != NULL) {
            if (PyBool_Check(PyResult))
                if (PyObject_IsTrue(PyResult))
                    result = 1;
        }
    }

    Py_DECREF(PyResult);
    Py_DECREF(PyFunc);
    Py_DECREF(PyArgs);
    Py_DECREF(PyModule);
    Py_DECREF(PyName);

    Py_DECREF(PyPath);
    Py_DECREF(PySys);

    PyErr_Print();

    return result;
}

DWORD WINAPI TelegramAuthorizationChecker::TelegramCodeValidCheck(LPVOID lpParam) {
    const char* PyFunctionName = "isTelegramPhoneNumberCodeValid";
    const char* pythonFilePath = "D:\\TelegramQuickView\\src\\gui\\settings\\";
    const char* moduleName = "TelegramAuthorization";
    DWORD result = 0;

    LPTelegramCredentials lpTelegramCredentials = static_cast<LPTelegramCredentials>(lpParam);

    PyObject* PySys = nullptr, * PyPath = nullptr, *PyArgs = nullptr, * PyName = nullptr, * PyModule = nullptr, * PyFunc = nullptr, *PyResult = nullptr;

    PySys = PyImport_ImportModule("sys");
    PyPath = PyObject_GetAttrString(PySys, "path");
    PyList_Append(PyPath, PyUnicode_FromString(pythonFilePath));

    Py_ssize_t PyArgumentsTupleSize = 6;

    PyName = PyUnicode_FromString(moduleName);
    if (!PyName)
        return result;

    PyModule = PyImport_Import(PyName);
    if (!PyModule)
        return result;

    PyFunc = PyObject_GetAttrString(PyModule, "asyncCall");
    if (!PyFunc)
        return result;

    std::cout << lpTelegramCredentials->apiId;

    if (PyCallable_Check(PyFunc)) {
        PyArgs = PyTuple_Pack(PyArgumentsTupleSize, PyUnicode_FromString(PyFunctionName),
            PyLong_FromLong(lpTelegramCredentials->apiId), PyUnicode_FromString(lpTelegramCredentials->phoneNumber.c_str()),
            PyUnicode_FromString(lpTelegramCredentials->apiHash.c_str()), PyLong_FromLong(lpTelegramCredentials->code),
            PyUnicode_FromString(lpTelegramCredentials->codeHash.c_str()));

        PyResult = PyObject_CallObject(PyFunc, PyArgs);

        if (PyResult != NULL) {
            if (PyBool_Check(PyResult))
                if (PyObject_IsTrue(PyResult))
                    result = 1;
        }
    }

    Py_DECREF(PyResult);
    Py_DECREF(PyFunc);
    Py_DECREF(PyArgs);
    Py_DECREF(PyModule);
    Py_DECREF(PyName);

    Py_DECREF(PyPath);
    Py_DECREF(PySys);

    PyErr_Print();

    return result;
}