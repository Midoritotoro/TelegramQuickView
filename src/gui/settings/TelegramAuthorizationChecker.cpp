#include "telegramAuthorizationChecker.h"


DWORD WINAPI TelegramAuthorizationChecker::TelegramCredentialsValidCheck(LPVOID lpParam) {
    const char* PyFunctionName = "isTelegramCredentialsValid";
    const char* pythonFilePath = "D:\\TelegramQuickView\\src\\gui\\settings\\";
    const char* moduleName = "TelegramAuthorization";
    bool BoolResult;

    LPTelegramCredentials lpTelegramCredentials = static_cast<LPTelegramCredentials>(lpParam);

    Py_Initialize();

    PyObject* PyArgs = nullptr;

    PyObject* PySys = PyImport_ImportModule("sys");
    PyObject* PyPath = PyObject_GetAttrString(PySys, "path");
    PyList_Append(PyPath, PyUnicode_FromString(pythonFilePath));

    Py_ssize_t PyArgumentsTupleSize = 4;

    PyObject* PyName = PyUnicode_FromString(moduleName);
    if (!PyName)
        return false;

    PyObject* PyModule = PyImport_Import(PyName);
    if (!PyModule)
        return false;

    PyObject* PyDict = PyModule_GetDict(PyModule);
    if (!PyDict)
        return false;

    PyObject* PyFunc = PyDict_GetItemString(PyDict, "asyncCall");

    if (PyCallable_Check(PyFunc)) {
        PyArgs = PyTuple_New(PyArgumentsTupleSize);

        PyTuple_SetItem(PyArgs, 0, PyUnicode_FromString(PyFunctionName));
        PyTuple_SetItem(PyArgs, 1, PyLong_FromLong(long(lpTelegramCredentials->apiId)));
        PyTuple_SetItem(PyArgs, 2, PyUnicode_FromString(lpTelegramCredentials->phoneNumber));
        PyTuple_SetItem(PyArgs, 3, PyUnicode_FromString(lpTelegramCredentials->apiHash));
        PyObject* PyResult = PyObject_CallObject(PyFunc, PyArgs);
        if (PyResult != NULL) {
            if (PyBool_Check(PyResult))
                if (PyObject_IsTrue(PyResult) == 0)
                    BoolResult = false;
                else
                    BoolResult = true;
                Py_DECREF(PyResult);
        }
    }
    PyErr_Print();

    Py_DECREF(PyArgs);
    Py_DECREF(PyFunc);
    Py_DECREF(PyDict);
    Py_DECREF(PyModule);
    Py_DECREF(PyName);

    Py_DECREF(PyPath);
    Py_DECREF(PySys);

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

    PyObject* PyArgs = nullptr;

    PyObject* PySys = PyImport_ImportModule("sys");
    PyObject* PyPath = PyObject_GetAttrString(PySys, "path");
    PyList_Append(PyPath, PyUnicode_FromString(pythonFilePath));

    Py_ssize_t PyArgumentsTupleSize = 5;

    PyObject* PyName = PyUnicode_FromString(moduleName);
    if (!PyName)
        return false;

    PyObject* PyModule = PyImport_Import(PyName);
    if (!PyModule)
        return false;

    PyObject* PyDict = PyModule_GetDict(PyModule);
    if (!PyDict)
        return false;

    PyObject* PyFunc = PyDict_GetItemString(PyDict, "asyncCall");

    if (PyCallable_Check(PyFunc)) {
        PyArgs = PyTuple_New(PyArgumentsTupleSize);

        PyTuple_SetItem(PyArgs, 0, PyUnicode_FromString(PyFunctionName));
        PyTuple_SetItem(PyArgs, 1, PyLong_FromLong(long(lpTelegramCredentials->apiId)));
        PyTuple_SetItem(PyArgs, 2, PyUnicode_FromString(lpTelegramCredentials->phoneNumber));
        PyTuple_SetItem(PyArgs, 3, PyUnicode_FromString(lpTelegramCredentials->apiHash));
        PyTuple_SetItem(PyArgs, 4, PyUnicode_FromString(lpTelegramCredentials->pathToUserSettingsJson));
        PyObject* PyResult = PyObject_CallObject(PyFunc, PyArgs);
        if (PyResult != NULL) {
            if (PyBool_Check(PyResult))
                if (PyObject_IsTrue(PyResult) == 0)
                    BoolResult = false;
                else
                    BoolResult = true;
            Py_DECREF(PyResult);
        }
    }
    PyErr_Print();

    Py_DECREF(PyArgs);
    Py_DECREF(PyFunc);
    Py_DECREF(PyDict);
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

    PyObject* PyArgs = nullptr;

    PyObject* PySys = PyImport_ImportModule("sys");
    PyObject* PyPath = PyObject_GetAttrString(PySys, "path");
    PyList_Append(PyPath, PyUnicode_FromString(pythonFilePath));

    Py_ssize_t PyArgumentsTupleSize = 6;

    PyObject* PyName = PyUnicode_FromString(moduleName);
    if (!PyName)
        return false;

    PyObject* PyModule = PyImport_Import(PyName);
    if (!PyModule)
        return false;

    PyObject* PyDict = PyModule_GetDict(PyModule);
    if (!PyDict)
        return false;

    PyObject* PyFunc = PyDict_GetItemString(PyDict, "asyncCall");

    if (PyCallable_Check(PyFunc)) {
        PyArgs = PyTuple_New(PyArgumentsTupleSize);

        PyTuple_SetItem(PyArgs, 0, PyUnicode_FromString(PyFunctionName));
        PyTuple_SetItem(PyArgs, 1, PyLong_FromLong(long(lpTelegramCredentials->apiId)));
        PyTuple_SetItem(PyArgs, 2, PyUnicode_FromString(lpTelegramCredentials->phoneNumber));
        PyTuple_SetItem(PyArgs, 3, PyUnicode_FromString(lpTelegramCredentials->apiHash));
        PyTuple_SetItem(PyArgs, 4, PyLong_FromLong(lpTelegramCredentials->code));
        PyTuple_SetItem(PyArgs, 5, PyUnicode_FromString(lpTelegramCredentials->codeHash));
        PyObject* PyResult = PyObject_CallObject(PyFunc, PyArgs);
        if (PyResult != NULL) {
            if (PyBool_Check(PyResult))
                if (PyObject_IsTrue(PyResult) == 0)
                    BoolResult = false;
                else
                    BoolResult = true;
            Py_DECREF(PyResult);
        }
    }
    PyErr_Print();

    Py_DECREF(PyArgs);
    Py_DECREF(PyFunc);
    Py_DECREF(PyDict);
    Py_DECREF(PyModule);
    Py_DECREF(PyName);

    Py_DECREF(PyPath);
    Py_DECREF(PySys);

    Py_Finalize();

    return BoolResult;
}