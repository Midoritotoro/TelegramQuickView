#include "telegramAuthorizationChecker.h"
#include <iostream>


DWORD WINAPI TelegramAuthorizationChecker::TelegramCredentialsValidCheck(LPVOID lpParam) {
    const char* PyFunctionName = "isTelegramCredentialsValid";
    const char* pythonFilePath = "D:\\TelegramQuickView\\src\\gui\\settings\\";
    const char* moduleName = "TelegramAuthorization";
    bool BoolResult;

    LPTelegramCredentials lpTelegramCredentials = static_cast<LPTelegramCredentials>(lpParam);

    Py_Initialize();

    PyObject* PySys = nullptr, * PyPath = nullptr, * PyArgs = nullptr, * PyName = nullptr, * PyModule = nullptr, * PyFunc = nullptr, * PyResult = nullptr;

    PySys = PyImport_ImportModule("sys");
    PyPath = PyObject_GetAttrString(PySys, "path");
    PyList_Append(PyPath, PyUnicode_FromString(pythonFilePath));

    Py_ssize_t PyArgumentsTupleSize = 4;

    PyName = PyUnicode_FromString(moduleName);
    PyErr_Print();
    if (!PyName)
        return false;

    PyModule = PyImport_Import(PyName);
    PyErr_Print();
    if (!PyModule)
        return false;

    PyFunc = PyObject_GetAttrString(PyModule, "asyncCall");

    PyErr_Print();
    if (!PyFunc)
        return false;

    if (PyCallable_Check(PyFunc)) {
        PyArgs = PyTuple_Pack(PyArgumentsTupleSize, PyUnicode_FromString(PyFunctionName),
            PyLong_FromLong(long(lpTelegramCredentials->apiId)), PyUnicode_FromString(lpTelegramCredentials->phoneNumber.c_str()), PyUnicode_FromString(lpTelegramCredentials->apiHash.c_str()));

        if (PyArgs != NULL)
            PyResult = PyObject_CallObject(PyFunc, PyArgs);
        if (PyResult != NULL) {
            if (PyBool_Check(PyResult))
                if (PyObject_IsTrue(PyResult) == 0)
                    BoolResult = false;
                else
                    BoolResult = true;
        }
    }

    Py_DECREF(PyResult);
    Py_DECREF(PyArgs);
    Py_DECREF(PyFunc);
    Py_DECREF(PyModule);
    Py_DECREF(PyName);

    Py_DECREF(PyPath);
    Py_DECREF(PySys);

    PyErr_Print();
    Py_Finalize();

    return BoolResult;
}

DWORD WINAPI TelegramAuthorizationChecker::sendTelegramCode(LPVOID lpParam){
    const char* PyFunctionName = "sendTelegramCode";
    const char* pythonFilePath = "D:\\TelegramQuickView\\src\\gui\\settings\\";
    const char* moduleName = "TelegramAuthorization";
    bool BoolResult;

    LPTelegramCredentials lpTelegramCredentials = static_cast<LPTelegramCredentials>(lpParam);

    Py_Initialize();

    PyObject* PySys = nullptr, * PyPath = nullptr, *PyArgs = nullptr, * PyName = nullptr, * PyModule = nullptr, *PyDict = nullptr, * PyFunc = nullptr, *PyResult = nullptr;

    PySys = PyImport_ImportModule("sys");
    PyPath = PyObject_GetAttrString(PySys, "path");
    PyList_Append(PyPath, PyUnicode_FromString(pythonFilePath));

    Py_ssize_t PyArgumentsTupleSize = 5;

    PyName = PyUnicode_FromString(moduleName);
    PyErr_Print();
    if (!PyName)
        return false;

    PyModule = PyImport_Import(PyName);
    PyErr_Print();
    if (!PyModule)
        return false;

    PyDict = PyModule_GetDict(PyModule);
    PyErr_Print();
    if (!PyDict)
        return false;

    PyFunc = PyDict_GetItemString(PyDict, "asyncCall");
    PyErr_Print();

    if (PyCallable_Check(PyFunc)) {
        PyArgs = PyTuple_Pack(PyArgumentsTupleSize, PyUnicode_FromString(PyFunctionName),
            PyLong_FromLong(long(lpTelegramCredentials->apiId)), PyUnicode_FromString(lpTelegramCredentials->phoneNumber.c_str()),
            PyUnicode_FromString(lpTelegramCredentials->apiHash.c_str()), PyUnicode_FromString(lpTelegramCredentials->pathToUserSettingsJson.c_str()));

        PyResult = PyObject_CallObject(PyFunc, PyArgs);
        if (PyResult != NULL) {
            if (PyBool_Check(PyResult))
                if (PyObject_IsTrue(PyResult) == 0)
                    BoolResult = false;
                else
                    BoolResult = true;
        }
    }

    PyErr_Print();

    Py_DECREF(PyResult);
    Py_DECREF(PyArgs);
    Py_DECREF(PyFunc);
    Py_DECREF(PyModule);
    Py_DECREF(PyName);

    Py_DECREF(PyPath);
    Py_DECREF(PySys);

    Py_Finalize();

    return BoolResult;
}

DWORD WINAPI TelegramAuthorizationChecker::TelegramCodeValidCheck(LPVOID lpParam) {
    const char* PyFunctionName = "isTelegramPhoneNumberCodeValid";
    const char* pythonFilePath = "D:\\TelegramQuickView\\src\\gui\\settings\\";
    const char* moduleName = "TelegramAuthorization";
    bool BoolResult;

    LPTelegramCredentials lpTelegramCredentials = static_cast<LPTelegramCredentials>(lpParam);

    Py_Initialize();

    PyObject* PySys = nullptr, * PyPath = nullptr, *PyArgs = nullptr, * PyName = nullptr, * PyModule = nullptr, *PyDict = nullptr, * PyFunc = nullptr, *PyResult = nullptr;

    PySys = PyImport_ImportModule("sys");
    PyPath = PyObject_GetAttrString(PySys, "path");
    PyList_Append(PyPath, PyUnicode_FromString(pythonFilePath));
    PyErr_Print();
    Py_ssize_t PyArgumentsTupleSize = 6;

    PyName = PyUnicode_FromString(moduleName);
    PyErr_Print();
    if (!PyName)
        return false;

    PyModule = PyImport_Import(PyName);
    PyErr_Print();
    if (!PyModule)
        return false;

    PyDict = PyModule_GetDict(PyModule);
    PyErr_Print();
    if (!PyDict)
        return false;

    PyFunc = PyDict_GetItemString(PyDict, "asyncCall");
    PyErr_Print();

    if (PyCallable_Check(PyFunc)) {
        PyArgs = PyTuple_Pack(PyArgumentsTupleSize, PyUnicode_FromString(PyFunctionName),
            PyLong_FromLong(long(lpTelegramCredentials->apiId)), PyUnicode_FromString(lpTelegramCredentials->phoneNumber.c_str()),
            PyUnicode_FromString(lpTelegramCredentials->apiHash.c_str()), PyLong_FromLong(lpTelegramCredentials->code),
            PyUnicode_FromString(lpTelegramCredentials->codeHash.c_str()));

        PyResult = PyObject_CallObject(PyFunc, PyArgs);

        if (PyResult != NULL) {
            if (PyBool_Check(PyResult))
                if (PyObject_IsTrue(PyResult) == 0)
                    BoolResult = false;
                else
                    BoolResult = true;
        }
    }

    PyErr_Print();

    Py_DECREF(PyResult);
    Py_DECREF(PyArgs);
    Py_DECREF(PyFunc);
    Py_DECREF(PyModule);
    Py_DECREF(PyName);

    Py_DECREF(PyPath);
    Py_DECREF(PySys);

    Py_Finalize();

    return BoolResult;
}