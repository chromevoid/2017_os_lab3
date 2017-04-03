#include "banker.h"

void process() {
    int task_number;
    std::vector<std::vector<Activity>> T;
    int resource_number;
    std::vector<int> R;

    get_input(task_number, T, resource_number, R);
//    print_input(task_number, T, resource_number, R);

}

int main(int argc, char const * argv[]) {
    // input
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (i == 1) {
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