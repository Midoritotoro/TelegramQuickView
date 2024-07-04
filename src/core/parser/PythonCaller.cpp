﻿#include "PythonCaller.h"


PythonCaller::PythonCaller(const char* apiHash, const char* phoneNumber, long long apiId, const char* pythonFilePath): m_PyApiHash(apiHash), m_PyPhoneNumber(phoneNumber), m_PyApiId(apiId) {
    Py_Initialize();

    PySys = PyImport_ImportModule("sys");
    PyPath = PyObject_GetAttrString(PySys, "path");
    PyList_Append(PyPath, PyUnicode_FromString(pythonFilePath));
};

PythonCaller::~PythonCaller() {
    Py_DECREF(PyClsInstance);
    Py_DECREF(PyArgs);
    Py_DECREF(PyClass);
    Py_DECREF(PyDict);
    Py_DECREF(PyModule);
    Py_DECREF(PyName);

    Py_DECREF(PyPath);
    Py_DECREF(PySys);

    PyErr_Print();
    Py_Finalize();
};

void PythonCaller::CallTelegramParseFunction(const char* mName, const char* pathToSettingsJsonFile, const char* pathToAppRootDirectory) {
    const char* PyFunctionName = "dig";
    const char* PyClassName = "Sleuth";
    Py_ssize_t PyClassArgumentsTupleSize = 5;

    PyName = PyUnicode_FromString(mName);
    if (!PyName)
        return;

    PyModule = PyImport_Import(PyName);
    if (!PyModule)
        return;

    PyDict = PyModule_GetDict(PyModule);
    if (PyDict == NULL)
        return;

    PyClass = PyDict_GetItemString(PyDict, PyClassName);

    if (PyCallable_Check(PyClass)) {
        PyArgs = PyTuple_New(PyClassArgumentsTupleSize);

        PyTuple_SetItem(PyArgs, 0, PyLong_FromLong(long(m_PyApiId)));
        PyTuple_SetItem(PyArgs, 1, PyUnicode_FromString(m_PyApiHash));
        PyTuple_SetItem(PyArgs, 2, PyUnicode_FromString(m_PyPhoneNumber));
        PyTuple_SetItem(PyArgs, 3, PyUnicode_FromString(pathToSettingsJsonFile));
        PyTuple_SetItem(PyArgs, 4, PyUnicode_FromString(pathToAppRootDirectory));

        PyClsInstance = PyObject_CallObject(PyClass, PyArgs);
        PyObject_CallMethod(PyClsInstance, PyFunctionName, NULL);
    }
}