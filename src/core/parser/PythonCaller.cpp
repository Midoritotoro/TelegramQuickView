﻿#include "PythonCaller.h"
#include <thread>


PythonCaller::PythonCaller(const char* pythonFilePath): _pythonFilePath(pythonFilePath) {
    Py_Initialize();
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

void PythonCaller::CallTelegramParseFunction(const char* moduleName, const char* pathToSettingsJsonFile, const char* pathToAppRootDirectory) {
    const char* PyFunctionName = "start";
    const char* PyClassName = "Sleuth";
    Py_ssize_t PyClassArgumentsTupleSize = 2;

    PySys = PyImport_ImportModule("sys");
    PyPath = PyObject_GetAttrString(PySys, "path");
    PyList_Append(PyPath, PyUnicode_FromString(_pythonFilePath));

    PyName = PyUnicode_FromString(moduleName);
    if (!PyName)
        return;

    PyModule = PyImport_Import(PyName);
    if (!PyModule)
        return;

    PyDict = PyModule_GetDict(PyModule);
    if (PyDict == NULL)
        return;

    PyClass = PyDict_GetItemString(PyDict, PyClassName);
    PyObject* asyncio_module = PyImport_ImportModule("asyncio");
    PyObject* loop = PyObject_CallMethod(asyncio_module, "new_event_loop", NULL);

    PyObject_CallMethod(asyncio_module, "set_event_loop", "(O)", loop);

    if (PyCallable_Check(PyClass)) {
        PyArgs = PyTuple_New(PyClassArgumentsTupleSize);

        PyTuple_SetItem(PyArgs, 0, PyUnicode_FromString(pathToSettingsJsonFile));
        PyTuple_SetItem(PyArgs, 1, PyUnicode_FromString(pathToAppRootDirectory));

        PyClsInstance = PyObject_CallObject(PyClass, PyArgs);
        PyObject_CallMethod(PyClsInstance, PyFunctionName, NULL);
    }
}