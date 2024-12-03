#include "src/Application.h"

int main(int argc, char* argv[]) {
    const auto application = Application::Create(argc, argv);
    return application ? application->exec() : 1;
}