//
// Created by Chrome on 3/29/17.
//

#ifndef BANKER_BANKER_H
#define BANKER_BANKER_H
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <string>
#include <sstream> // read file as standard input
#include <vector>
#include <queue>
#include <iostream>

class Activity {
    std::string activity;
    unsigned int first, second, third;
public:
    Activity(std::string n = "null", unsigned int f = 0, unsigned int s = 0, unsigned int t = 0) {
        activity = n;
        first = f;
        second = s;
        third = t;
    }
    std::string get_name() const { return activity; };
    unsigned int get_first() const { return first; };
    unsigned int get_second() const { return second; };
    unsigned int get_third() const { return third; };
    friend std::ostream &operator << (std::ostream & out, const Activity a) {
        out << a.activity << " " << a.first << " " << a.second << " " << a.third;
        return out;
    }
};

void get_input(int & task_number, std::vector<std::vector<Activity>> & T, int & resource_number, std::vector<int> & R) {
    std::cin >> task_number;
    for (int i = 0; i < task_number; i++) {
        std::vector<Activity> tmp;
        T.push_back(tmp);
    }
    std::cin >> resource_number;
    for (int i = 0; i < resource_number; i++) {
        int unit_number;
        std::cin >> unit_number;
        R.push_back(unit_number);
    }
    int terminate_count = 0;
    while (terminate_count != task_number) {
        std::string a;
        unsigned int f,s,t;
        std::cin >> a >> f >> s >> t;
        Activity tmp(a, f, s, t);
        T[f-1].push_back(tmp);
        if (a.compare("terminate") == 0) {
            terminate_count++;
        }
    }
}

void print_input(int & task_number, std::vector<std::vector<Activity>> & T, int & resource_number, std::vector<int> & R) {
    for (int i = 0; i < task_number; i++) {
        std::cout << "task" << i + 1 << ":\n";
        for (int j = 0; j < T[i].size(); j++) {
            std::cout << T[i][j] << std::endl;
        }
        std::cout << std::endl;
    }
}


#endif //BANKER_BANKER_H
