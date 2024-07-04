#include "PythonCaller.h"
#include <iostream>


PythonCaller::PythonCaller(const char* apiHash, const char* phoneNumber, long long apiId, const char* pythonFilePath): m_PyApiHash(apiHash), m_PyPhoneNumber(phoneNumber), m_PyApiId(apiId) {
    Py_Initialize();
    PySys = PyImport_ImportModule("sys");
    PyPath = PyObject_GetAttrString(PySys, "path");
    std::cout << pythonFilePath;
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

int PythonCaller::CallTelegramParseFunction(const char* mName, const char* pathToSettingsJsonFile, const char* pathToAppRootDirectory)
{
    const char* fName = "dig";
    const char* clsName = "Sleuth";

    PyName = PyUnicode_FromString(mName);
    if (!PyName)
        return NULL;

    PyModule = PyImport_Import(PyName);
    if (!PyModule)
        return NULL;

    PyDict = PyModule_GetDict(PyModule);
    if (PyDict == NULL)
        return NULL;

    PyClass = PyDict_GetItemString(PyDict, clsName);

    if (PyCallable_Check(PyClass)) {
        Py_ssize_t tupleSize = 5;
        PyArgs = PyTuple_New(tupleSize);
        PyTuple_SetItem(PyArgs, 0, PyLong_FromLong(long(m_PyApiId)));
        PyTuple_SetItem(PyArgs, 1, PyUnicode_FromString(m_PyApiHash));
        PyTuple_SetItem(PyArgs, 2, PyUnicode_FromString(m_PyPhoneNumber));
        PyTuple_SetItem(PyArgs, 3, PyUnicode_FromString(pathToSettingsJsonFile));
        PyTuple_SetItem(PyArgs, 4, PyUnicode_FromString(pathToAppRootDirectory));
        PyClsInstance = PyObject_CallObject(PyClass, PyArgs);
        PyObject_CallMethod(PyClsInstance, fName, NULL);
    }
    return 0;
}