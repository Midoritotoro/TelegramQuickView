#include "src/Application.h"

#if defined _DLL && defined _MT
    #pragma message("_MT && _DLL in main part of code is defined")
#endif

#ifdef _MT
    #pragma message("_MT in main part of code is defined")
#endif

int main(int argc, char* argv[]) {
    const auto application = Application::Create(argc, argv);
    return application ? application->exec() : 1;
}