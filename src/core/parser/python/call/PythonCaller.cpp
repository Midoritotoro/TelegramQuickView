#include "PythonCaller.h"

PythonCaller::PythonCaller(const char* apiHash, const char* phoneNumber, const char* downloadPath, long long apiId, const char* pythonFilePath) : m_PyApiHash(apiHash), m_PyPhoneNumber(phoneNumber), m_PyApiId(apiId), m_PyDownloadPath(downloadPath) {
    Py_Initialize();
    PySys = PyImport_ImportModule("sys");
    PyPath = PyObject_GetAttrString(PySys, "path");
    PyList_Append(PyPath, PyUnicode_FromString(pythonFilePath));
    PyErr_Print();
};

PythonCaller::~PythonCaller() {
    Py_XDECREF(PyClsInstance);
    Py_XDECREF(PyClass);
    Py_XDECREF(PyArgs);
    Py_XDECREF(PyFunc);
    Py_XDECREF(PyDict);
    Py_XDECREF(PyModule);
    Py_XDECREF(PyName);

    Py_XDECREF(PyPath);
    Py_XDECREF(PySys);
    PyErr_Print();
    Py_Finalize();
};

int PythonCaller::CallTelegramParseFunction(const char* mName, const char* username, const char* startDate, const char* endDate)
{
    const char* fName = "start";
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
        Py_ssize_t tupleSize = 7;
        PyArgs = PyTuple_New(tupleSize);
        PyTuple_SetItem(PyArgs, 0, PyLong_FromLong(long(m_PyApiId)));
        PyTuple_SetItem(PyArgs, 1, PyUnicode_FromString(m_PyApiHash));
        PyTuple_SetItem(PyArgs, 2, PyUnicode_FromString(m_PyPhoneNumber));
        PyTuple_SetItem(PyArgs, 3, PyUnicode_FromString(username));
        PyTuple_SetItem(PyArgs, 4, PyUnicode_FromString(startDate));
        PyTuple_SetItem(PyArgs, 5, PyUnicode_FromString(endDate));
        PyTuple_SetItem(PyArgs, 6, PyUnicode_FromString(m_PyDownloadPath));
        PyClsInstance = PyObject_CallObject(PyClass, PyArgs);
        PyObject_CallMethod(PyClsInstance, fName, NULL);
        PyErr_Print();
    }
    else
        PyErr_Print();
    PyErr_Print();
    return 0;
}