#include "Tests.h"
#include <iostream>

int main(int argc, char* argv[]) {
    bool step_mode = false;

    if (argc > 1 && std::string(argv[1]) == "--step") {
        step_mode = true;
    }

    Tests::testNPU(step_mode);
    return 0;
}
