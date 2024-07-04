#include "PythonCaller.h"


PythonCaller::PythonCaller(const char* apiHash, const char* phoneNumber, long long apiId, const char* pythonFilePath): m_PyApiHash(apiHash), m_PyPhoneNumber(phoneNumber), m_PyApiId(apiId), _pythonFilePath(pythonFilePath) {
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
    const char* PyFunctionName = "dig";
    const char* PyClassName = "Sleuth";
    Py_ssize_t PyClassArgumentsTupleSize = 5;

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

bool PythonCaller::callTelegramAuthorizeCheck() {
    const char* PyFunctionName = "isUserAuthorized";
    const char* pythonFilePath = "D:\\TelegramQuickView\\src\\gui\\settings\\";
    const char* moduleName = "TelegramAuthorization";

    PySys = PyImport_ImportModule("sys");
    PyPath = PyObject_GetAttrString(PySys, "path");
    PyList_Append(PyPath, PyUnicode_FromString(_pythonFilePath));

    Py_ssize_t PyArgumentsTupleSize = 5;
   
    PyName = PyUnicode_FromString(moduleName);
    if (!PyName)
        return false;

    PyModule = PyImport_Import(PyName);
    if (!PyModule)
        return false;

    PyDict = PyModule_GetDict(PyModule);
    if (PyDict == NULL)
        return false;

    PyFunc = PyDict_GetItemString(PyDict, PyFunctionName);

    if (PyCallable_Check(PyClass)) {
        PyArgs = PyTuple_New(PyArgumentsTupleSize);

        PyTuple_SetItem(PyArgs, 0, PyLong_FromLong(long(m_PyApiId)));
        PyTuple_SetItem(PyArgs, 1, PyUnicode_FromString(m_PyPhoneNumber));
        PyTuple_SetItem(PyArgs, 2, PyUnicode_FromString(m_PyApiHash));
        PyObject* PyResult = PyObject_CallObject(PyFunc, PyArgs);
        if (PyResult != NULL) {
            if (PyBool_Check(PyResult)) {
                bool BoolResult = PyObject_IsTrue(PyResult);
                Py_DECREF(PyResult);
                Py_DECREF(PyFunc);
                return BoolResult;
            }
        }
    }
    return false;
}