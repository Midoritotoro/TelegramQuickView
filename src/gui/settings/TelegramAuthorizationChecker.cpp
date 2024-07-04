#include "telegramAuthorizationChecker.h"

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

bool TelegramAuthorizationChecker::callTelegramAuthorizeCheck(const char* apiHash, const char* phoneNumber, long long apiId) {
    const char* PyFunctionName = "isUserAuthorized";
    const char* pythonFilePath = "D:\\TelegramQuickView\\src\\gui\\settings\\";
    const char* moduleName = "TelegramAuthorization";
    bool BoolResult = false;

    PySys = PyImport_ImportModule("sys");
    PyPath = PyObject_GetAttrString(PySys, "path");
    PyList_Append(PyPath, PyUnicode_FromString(pythonFilePath));

    Py_ssize_t PyArgumentsTupleSize = 3;

    PyName = PyUnicode_FromString(moduleName);
    if (!PyName)
        return false;

    PyModule = PyImport_Import(PyName);
    if (!PyModule)
        return false;

    PyDict = PyModule_GetDict(PyModule);
    if (!PyDict)
        return false;

    PyFunc = PyDict_GetItemString(PyDict, PyFunctionName);

    if (PyCallable_Check(PyFunc)) {
        PyArgs = PyTuple_New(PyArgumentsTupleSize);

        PyTuple_SetItem(PyArgs, 0, PyLong_FromLong(long(apiId)));
        PyTuple_SetItem(PyArgs, 1, PyUnicode_FromString(phoneNumber));
        PyTuple_SetItem(PyArgs, 2, PyUnicode_FromString(apiHash));
        PyResult = PyObject_CallObject(PyFunc, PyArgs);
        if (PyResult != NULL) {
            if (PyBool_Check(PyResult))
                BoolResult = PyObject_IsTrue(PyResult);
                Py_DECREF(PyResult);
        }
    }
    return BoolResult;
}