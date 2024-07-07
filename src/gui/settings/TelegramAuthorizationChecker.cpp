#include "telegramAuthorizationChecker.h"
#include <iostream>

TelegramAuthorizationChecker::TelegramAuthorizationChecker() {
    Py_Initialize();
}

TelegramAuthorizationChecker::~TelegramAuthorizationChecker() {
    Py_DECREF(PyArgs);
    Py_DECREF(PyFunc);
    Py_DECREF(PyDict);
    Py_DECREF(PyModule);
    Py_DECREF(PyName);

    Py_DECREF(PyPath);
    Py_DECREF(PySys);
    PyErr_Print();
    Py_Finalize();
}

bool TelegramAuthorizationChecker::TelegramCredentialsValidCheck(const char* apiHash, const char* phoneNumber, long long apiId) {
    const char* PyFunctionName = "isTelegramCredentialsValid";
    const char* pythonFilePath = "D:\\TelegramQuickView\\src\\gui\\settings\\";
    const char* moduleName = "TelegramAuthorization";
    bool BoolResult = false;

    PySys = PyImport_ImportModule("sys");
    PyPath = PyObject_GetAttrString(PySys, "path");
    PyList_Append(PyPath, PyUnicode_FromString(pythonFilePath));

    Py_ssize_t PyArgumentsTupleSize = 4;

    PyName = PyUnicode_FromString(moduleName);
    if (!PyName)
        return false;

    PyModule = PyImport_Import(PyName);
    if (!PyModule)
        return false;

    PyDict = PyModule_GetDict(PyModule);
    if (!PyDict)
        return false;

    PyFunc = PyDict_GetItemString(PyDict, "asyncCall");

    if (PyCallable_Check(PyFunc)) {
        PyArgs = PyTuple_New(PyArgumentsTupleSize);

        PyTuple_SetItem(PyArgs, 0, PyUnicode_FromString(PyFunctionName));
        PyTuple_SetItem(PyArgs, 1, PyLong_FromLong(long(apiId)));
        PyTuple_SetItem(PyArgs, 2, PyUnicode_FromString(phoneNumber));
        PyTuple_SetItem(PyArgs, 3, PyUnicode_FromString(apiHash));
        PyResult = PyObject_CallObject(PyFunc, PyArgs);
        if (PyResult != NULL) {
            if (PyBool_Check(PyResult))
                BoolResult = PyObject_IsTrue(PyResult);
                Py_DECREF(PyResult);
        }
    }
    PyErr_Print();
    return BoolResult;
}

bool TelegramAuthorizationChecker::sendTelegramCode(const char* apiHash, const char* phoneNumber, long long apiId, const char* pathToUserSettingsJson){
    const char* PyFunctionName = "sendTelegramCode";
    const char* pythonFilePath = "D:\\TelegramQuickView\\src\\gui\\settings\\";
    const char* moduleName = "TelegramAuthorization";
    bool BoolResult = false;

    std::cout << apiHash << phoneNumber << apiId << pathToUserSettingsJson;

    PySys = PyImport_ImportModule("sys");
    PyPath = PyObject_GetAttrString(PySys, "path");
    PyList_Append(PyPath, PyUnicode_FromString(pythonFilePath));

    Py_ssize_t PyArgumentsTupleSize = 5;

    PyName = PyUnicode_FromString(moduleName);
    if (!PyName)
        return false;

    PyModule = PyImport_Import(PyName);
    if (!PyModule)
        return false;

    PyDict = PyModule_GetDict(PyModule);
    if (!PyDict)
        return false;

    PyFunc = PyDict_GetItemString(PyDict, "asyncCall");

    if (PyCallable_Check(PyFunc)) {
        PyArgs = PyTuple_New(PyArgumentsTupleSize);

        PyTuple_SetItem(PyArgs, 0, PyUnicode_FromString(PyFunctionName));
        PyTuple_SetItem(PyArgs, 1, PyLong_FromLong(long(apiId)));
        PyTuple_SetItem(PyArgs, 2, PyUnicode_FromString(phoneNumber));
        PyTuple_SetItem(PyArgs, 3, PyUnicode_FromString(apiHash));
        PyTuple_SetItem(PyArgs, 4, PyUnicode_FromString(pathToUserSettingsJson));
        PyResult = PyObject_CallObject(PyFunc, PyArgs);
        if (PyResult != NULL) {
            if (PyBool_Check(PyResult))
                BoolResult = PyObject_IsTrue(PyResult);
            Py_DECREF(PyResult);
        }
    }
    PyErr_Print();
    return BoolResult;
}

bool TelegramAuthorizationChecker::TelegramCodeValidCheck(const char* apiHash, const char* phoneNumber, long long apiId, long code, const char* codeHash) {
    const char* PyFunctionName = "isTelegramPhoneNumberCodeValid";
    const char* pythonFilePath = "D:\\TelegramQuickView\\src\\gui\\settings\\";
    const char* moduleName = "TelegramAuthorization";
    bool BoolResult = false;

    PySys = PyImport_ImportModule("sys");
    PyPath = PyObject_GetAttrString(PySys, "path");
    PyList_Append(PyPath, PyUnicode_FromString(pythonFilePath));

    Py_ssize_t PyArgumentsTupleSize = 6;

    PyName = PyUnicode_FromString(moduleName);
    if (!PyName)
        return false;

    PyModule = PyImport_Import(PyName);
    if (!PyModule)
        return false;

    PyDict = PyModule_GetDict(PyModule);
    if (!PyDict)
        return false;

    PyFunc = PyDict_GetItemString(PyDict, "asyncCall");

    if (PyCallable_Check(PyFunc)) {
        PyArgs = PyTuple_New(PyArgumentsTupleSize);

        PyTuple_SetItem(PyArgs, 0, PyUnicode_FromString(PyFunctionName));
        PyTuple_SetItem(PyArgs, 1, PyLong_FromLong(long(apiId)));
        PyTuple_SetItem(PyArgs, 2, PyUnicode_FromString(phoneNumber));
        PyTuple_SetItem(PyArgs, 3, PyUnicode_FromString(apiHash));
        PyTuple_SetItem(PyArgs, 4, PyLong_FromLong(code));
        PyTuple_SetItem(PyArgs, 5, PyUnicode_FromString(codeHash));
        PyResult = PyObject_CallObject(PyFunc, PyArgs);
        if (PyResult != NULL) {
            if (PyBool_Check(PyResult))
                BoolResult = PyObject_IsTrue(PyResult);
            Py_DECREF(PyResult);
        }
    }
    PyErr_Print();
    return BoolResult;
}