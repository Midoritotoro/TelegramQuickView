#include "telegramAuthorizationChecker.h"
#include <pybind11/pybind11.h>

TelegramAuthorizationChecker::TelegramAuthorizationChecker() {
    
}

TelegramAuthorizationChecker::~TelegramAuthorizationChecker() {
   
}

bool TelegramAuthorizationChecker::TelegramCredentialsValidCheck(const char* apiHash, const char* phoneNumber, long long apiId) {
    const char* PyFunctionName = "isTelegramCredentialsValid";
    const char* pythonFilePath = "D:\\TelegramQuickView\\src\\gui\\settings\\";
    const char* moduleName = "TelegramAuthorization";
    bool BoolResult = false;

    pybind11::object py_module = pybind11::module::import(pythonFilePath + *moduleName);
    pybind11::object result = py_module.attr(PyFunctionName)(25);
    std::cout << pybind11::cast<bool>(result);
    return BoolResult;
}

bool TelegramAuthorizationChecker::sendTelegramCode(const char* apiHash, const char* phoneNumber, long long apiId){
    const char* PyFunctionName = "sendTelegramCode";
    const char* pythonFilePath = "D:\\TelegramQuickView\\src\\gui\\settings\\";
    const char* moduleName = "TelegramAuthorization";
    bool BoolResult = false;

    
    return BoolResult;
}