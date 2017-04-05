#include "banker.h"

void process() {
    unsigned long task_number; // record the number of the tasks
    std::vector<std::queue<Activity>> T; // store the Activities of each task
    unsigned long resource_number; // record the number of the resource types
    std::vector<int> R; // store the resource type and the units it has

    get_input(task_number, T, resource_number, R);
//    print_input(task_number, T, resource_number, R);

    FIFO(task_number, T, resource_number, R);
    std::cout << std::endl;
    Banker(task_number, T, resource_number, R);
    std::cout << std::endl;
}

int main(int argc, char const * argv[]) {
    // input
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (i != 0) {
                std::cin.clear();
                std::cin.sync();
                freopen(argv[i], "r", stdin);
                process();
            }
        }
    }
    else {
        std::cout << "usage: ./banker file [file ...]";
    }

    return 0;
}
