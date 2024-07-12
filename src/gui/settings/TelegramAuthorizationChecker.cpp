#include "telegramAuthorizationChecker.h"


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
    PyList_Append(PyPath, PyUnicode_FromString("."));

    Py_ssize_t PyArgumentsTupleSize = 4;

    PyName = PyUnicode_FromString(moduleName);
    if (!PyName)
        return false;

    PyModule = PyImport_Import(PyName);
    if (!PyModule)
        return false;

    PyFunc = PyObject_GetAttrString(PyModule, "asyncCall");
    if (!PyFunc)
        return false;

    if (PyCallable_Check(PyFunc)) {
        PyArgs = PyTuple_Pack(PyArgumentsTupleSize, PyUnicode_FromString(PyFunctionName),
            PyLong_FromLong(long(lpTelegramCredentials->apiId)), PyUnicode_FromString(lpTelegramCredentials->phoneNumber), PyUnicode_FromString(lpTelegramCredentials->apiHash));

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
    //Py_DECREF(PyName);

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

    Py_InitializeEx(0);

    PyObject* PySys = nullptr, * PyPath = nullptr, *PyArgs = nullptr, * PyName = nullptr, * PyModule = nullptr, *PyDict = nullptr, * PyFunc = nullptr, *PyResult = nullptr;

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
    Py_DECREF(PyDict);

    if (PyCallable_Check(PyFunc)) {
        PyArgs = PyTuple_New(PyArgumentsTupleSize);

        PyTuple_SetItem(PyArgs, 0, PyUnicode_FromString(PyFunctionName));
        PyTuple_SetItem(PyArgs, 1, PyLong_FromLong(long(lpTelegramCredentials->apiId)));
        PyTuple_SetItem(PyArgs, 2, PyUnicode_FromString(lpTelegramCredentials->phoneNumber));
        PyTuple_SetItem(PyArgs, 3, PyUnicode_FromString(lpTelegramCredentials->apiHash));
        PyTuple_SetItem(PyArgs, 4, PyUnicode_FromString(lpTelegramCredentials->pathToUserSettingsJson));

        PyResult = PyObject_CallObject(PyFunc, PyArgs);
        if (PyResult != NULL) {
            if (PyBool_Check(PyResult))
                if (PyObject_IsTrue(PyResult) == 0)
                    BoolResult = false;
                else
                    BoolResult = true;
            Py_DECREF(PyResult);
        }
        else {
            Py_DECREF(PyResult);
        }
        Py_DECREF(PyFunc);
    }

    PyErr_Print();

    Py_DECREF(PyResult);
    Py_DECREF(PyArgs);
    Py_DECREF(PyModule);
    Py_DECREF(PyName);

    Py_DECREF(PyPath);
    Py_DECREF(PySys);

    Py_FinalizeEx();

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
    Py_DECREF(PyDict);
    
    if (PyCallable_Check(PyFunc)) {
        PyArgs = PyTuple_New(PyArgumentsTupleSize);


        PyTuple_SetItem(PyArgs, 0, PyUnicode_FromString(PyFunctionName));
        PyTuple_SetItem(PyArgs, 1, PyLong_FromLong(long(lpTelegramCredentials->apiId)));
        PyTuple_SetItem(PyArgs, 2, PyUnicode_FromString(lpTelegramCredentials->phoneNumber));
        PyTuple_SetItem(PyArgs, 3, PyUnicode_FromString(lpTelegramCredentials->apiHash));
        PyTuple_SetItem(PyArgs, 4, PyLong_FromLong(lpTelegramCredentials->code));
        PyTuple_SetItem(PyArgs, 5, PyUnicode_FromString(lpTelegramCredentials->codeHash));


        PyResult = PyObject_CallObject(PyFunc, PyArgs);

        if (PyResult != NULL) {
            if (PyBool_Check(PyResult))
                if (PyObject_IsTrue(PyResult) == 0)
                    BoolResult = false;
                else
                    BoolResult = true;
            Py_DECREF(PyResult);
        }
        else {
            Py_DECREF(PyResult);
        }
        Py_DECREF(PyFunc);
    }

    PyErr_Print();

    Py_DECREF(PyResult);
    Py_DECREF(PyArgs);
    Py_DECREF(PyModule);
    Py_DECREF(PyName);

    Py_DECREF(PyPath);
    Py_DECREF(PySys);

    Py_FinalizeEx();

    return BoolResult;
}