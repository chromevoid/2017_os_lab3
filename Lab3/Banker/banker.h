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
#include <math.h>

/* There are six types of activity:
 *   initiate task-number resource-type initial-claim
 *   request task-number resource-type number-requested
 *   release task-number resource-type number-released
 *   compute task-number number-of-cycles  unused
 *   terminate task-number unused unused
 *   resource resource-type initial-claim obtained
 *   */
class Activity {
    std::string activity; // the name of the activity
    unsigned int first, second, third; // three fields of the activity
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
    void change_third(unsigned int new_t) { third = new_t; };
    friend std::ostream &operator << (std::ostream & out, const Activity a) {
        out << a.activity << " " << a.first << " " << a.second << " " << a.third;
        return out;
    }
};

/* Get input from a file and store the information */
void get_input(unsigned long & task_number, std::vector<std::queue<Activity>> & T, unsigned long & resource_number, std::vector<int> & R) {
    std::cin >> task_number;
    for (int i = 0; i < task_number; i++) {
        std::queue<Activity> tmp;
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
        T[f-1].push(tmp);
        if (a.compare("compute") == 0) {
            for (int i = 1; i < s; i++) {
                T[f-1].push(tmp);
            }
        }
        if (a.compare("terminate") == 0) {
            terminate_count++;
        }
    }
}

/* Print the stored input information */
void print_input(unsigned long task_number, std::vector<std::queue<Activity>> T, unsigned long resource_number, std::vector<int> R) {
    std::cout << resource_number;
    for (int i = 0; i < resource_number; i++) {
        std::cout << " " << R[i];
    }
    std::cout << std::endl;
    for (int i = 0; i < task_number; i++) {
        std::cout << "task " << i + 1 << ":\n";
        unsigned long T_size = T[i].size();
        for (int j = 0; j < T_size; j++) {
            std::cout << T[i].front() << std::endl;
            T[i].pop();
        }
        std::cout << std::endl;
    }
}

/* Print the result of the method */
void print_result(std::string method, unsigned long task_number, std::vector<int> task_time_taken, std::vector<int> task_waiting_time) {
    int total_time = 0, total_waiting_time = 0;
    std::cout << method << std::endl;
    for (int i = 0; i < task_number; i++) {
        std::ostringstream task;
        task << "Task " << i + 1;
        if (task_time_taken[i] == 0) {
            std::cout << std::left << std::setw(10) << task.str() << "aborted" << std::endl;
            continue;
        }
        std::cout << std::left << std::setw(10) << task.str()
                  << std::setw(6) << task_time_taken[i]
                  << std::setw(6) << task_waiting_time[i]
                  << round(task_waiting_time[i] * 100.0 / task_time_taken[i]) << "%" << std::endl;
        total_time += task_time_taken[i];
        total_waiting_time += task_waiting_time[i];
    }
    std::cout << std::setw(10) << "total"
              << std::setw(6) << total_time
              << std::setw(6)  << total_waiting_time
              << round(total_waiting_time * 100.0 / total_time) << "%" << std::endl;
}

/* Check if all the activities are finished:
 *   if all the activities are done, return true
 *   otherwise, return false */
bool empty_T(std::vector<std::queue<Activity>> T) {
    for (int i = 0; i < T.size(); i++) {
        if (!T[i].empty()) return false;
    }
    return true;
}

/* Check if the current task is already done in the "check blocked tasks"  */
bool check_done(std::vector<unsigned int> task_done_in_check_blocked, unsigned int that) {
    for (int i = 0; i < task_done_in_check_blocked.size(); i++) {
        if (task_done_in_check_blocked[i] == that) {
            return true;
        }
    }
    return false;
}

/* Run FIFO
 *   while (not finished)
 *     check blocked queue and deal with the tasks in the queue
 *     check each task except the ones have been done in blocked queue
 *     check if the task is terminated
 *     restore the resources that are released in this cycle
 *     check deadlock, then solve the deadlock if deadlock exits
 *   */
void FIFO(unsigned long task_number, std::vector<std::queue<Activity>> T, unsigned long resource_number, std::vector<int> R) {
    int count_cycle = 1; // record the cycle number
    std::vector<int> task_time_taken(task_number, 0); // store the time taken for each task
    std::vector<int> task_waiting_time(task_number, 0); // store the waiting time for each task
    std::vector<Activity> blocked; // store the blocked tasks
    std::vector<unsigned int> task_done_in_check_blocked; // store the tasks done in "check blocked tasks"
    std::vector<Activity> temp_vector; // temp vector to initialize resource_obtain_table
    std::vector<std::vector<Activity>> resource_obtain_table(task_number, temp_vector); // store the initial claims and resource arrangement status
    while (!empty_T(T)) { // while at least one activity exits
        int make_a_move = 0; // a variable to detect deadlock
        std::vector<int> release(resource_number, 0);
        // check blocked tasks first
        for (int i = 0; i < blocked.size(); i++) {
            Activity a = blocked[i];
            task_done_in_check_blocked.push_back(a.get_first());
            if (a.get_third() <= R[a.get_second() - 1]) {
                R[a.get_second() - 1] -= a.get_third();
                for (int j = 0; j < resource_obtain_table[i].size(); j++) {
                    if (resource_obtain_table[a.get_first() - 1][j].get_first() == a.get_second()) {
                        resource_obtain_table[a.get_first() - 1][j].change_third(
                                resource_obtain_table[a.get_first() - 1][j].get_third() + a.get_third());
                        break;
                    }
                }
                blocked.erase(blocked.begin() + i);
                i--;
                make_a_move++;
            }
            else {
                task_waiting_time[a.get_first() - 1]++;
            }
        }
        // deal with other tasks
        for (int i = 0; i < task_number; i++) {
            // if the current task doesn't have any activity, then skip this task
            if (T[i].empty()) continue;
            // if the current task is already done in check blocked, then skip this task
            if (check_done(task_done_in_check_blocked, T[i].front().get_first())) continue;
            // otherwise, process current task
            Activity a = T[i].front();
            if (a.get_name().compare("initiate") == 0) {
                // resource resource_type claim_number obtain_number
                Activity tmp("resource", a.get_second(), a.get_third(), 0);
                resource_obtain_table[i].push_back(tmp);
                T[i].pop();
                make_a_move++;
            }
            else if (a.get_name().compare("request") == 0) {
                if (a.get_third() <= R[a.get_second() - 1]) {
                    R[a.get_second() - 1] -= a.get_third();
                    for (int j = 0; j < resource_obtain_table[i].size(); j++) {
                        if (resource_obtain_table[i][j].get_first() == a.get_second()) {
                            resource_obtain_table[i][j].change_third(resource_obtain_table[i][j].get_third() + a.get_third());
                            break;
                        }
                    }
                    make_a_move++;
                }
                else {
                    blocked.push_back(a);
                    task_waiting_time[i]++;
                }
                T[i].pop();
            }
            else if (a.get_name().compare("release") == 0) {
                release[a.get_second() - 1] += a.get_third();
                for (int j = 0; j < resource_obtain_table[i].size(); j++) {
                    if (resource_obtain_table[i][j].get_first() == a.get_second()) {
                        resource_obtain_table[i][j].change_third(resource_obtain_table[i][j].get_third() - a.get_third());
                        break;
                    }
                }
                T[i].pop();
                make_a_move++;
            }
            else if (a.get_name().compare("compute") == 0) {
                T[i].pop();
                make_a_move++;
            }
            a = T[i].front();
            if (a.get_name().compare("terminate") == 0) {
                T[i].pop();
                task_time_taken[i] = count_cycle;
            }
        }
        // restore the release resources
        for (int i = 0; i < resource_number; i++) {
            R[i] += release[i];
        }
        // if deadlock happens, then abort task(s) to solve the deadlock
        if (0 == make_a_move) {
            while(!blocked.empty()) {
                // since the abort order is based on task order
                // find the smallest task index in blocked array
                int index = 0;
                int min_task_index = blocked[0].get_first() - 1;
                for (int i = 1; i < blocked.size(); i++) {
                    if (min_task_index > blocked[i].get_first() - 1) {
                        min_task_index = blocked[i].get_first() - 1;
                        index = i;
                    }
                }
                // once find the task index is found, abort this task and reclaim it's resources
                Activity a = blocked[index];
                for (int j = 0; j < resource_obtain_table[a.get_first() - 1].size(); j++) {
                    R[a.get_second() - 1] += resource_obtain_table[a.get_first() - 1][j].get_third();
                    resource_obtain_table[a.get_first() - 1][j].change_third(0);
                }
                task_time_taken[a.get_first() - 1] = 0;
                while (!T[a.get_first() - 1].empty()) {
                    T[a.get_first() - 1].pop();
                }
                blocked.erase(blocked.begin() + index);
                a = blocked[0];
                // if the deadlock is solved, then break
                if (a.get_third() <= R[a.get_second() - 1]) {
                    break;
                }
            }

        }
        task_done_in_check_blocked.clear();
        count_cycle++;
    }
    print_result("FIFO", task_number, task_time_taken, task_waiting_time);
}

/* Check if all the tasks are finished:
 *   if all the tasks are done, return true
 *   otherwise, return false */
bool empty_ROT(std::vector<std::vector<Activity>> resource_obtain_table) {
    for (int i = 0; i < resource_obtain_table.size(); i++) {
        if (!resource_obtain_table[i].empty()) return false;
    }
    return true;
}

/* Recursively check if a task can be done in each round */
bool is_safe_recursively(std::vector<int> R, std::vector<std::vector<Activity>> resource_obtain_table) {
    if (empty_ROT(resource_obtain_table)) return true;
    for (int i = 0; i < resource_obtain_table.size(); i++) {
        if (resource_obtain_table[i].size() == 0) continue;
        bool task_i_can_finished = true;
        for (int j = 0; j < resource_obtain_table[i].size(); j++) {
            unsigned int resource_type = resource_obtain_table[i][j].get_first();
            unsigned int resource_claim = resource_obtain_table[i][j].get_second();
            unsigned int resource_obtain = resource_obtain_table[i][j].get_third();
            if (R[resource_type - 1] < resource_claim - resource_obtain) {
                task_i_can_finished = false;
                break;
            }
        }
        if (task_i_can_finished) {
            for (int j = 0; j < resource_obtain_table[i].size(); j++) {
                unsigned int resource_type = resource_obtain_table[i][j].get_first();
                unsigned int resource_obtain = resource_obtain_table[i][j].get_third();
                R[resource_type - 1] += resource_obtain;
                resource_obtain_table[i][j].change_third(0);
            }
            resource_obtain_table[i].clear();
            return is_safe_recursively(R, resource_obtain_table);
        }
    };
    return false;
}

/* Check if the state is safe (This is an assist function for Banker)
 *   pretend to grant the resource
 *   check tasks
 *   if all tasks can be completed, then return true, otherwise return false */
bool is_safe(Activity a, std::vector<int> R, std::vector<std::vector<Activity>> resource_obtain_table) {
    for (int i = 0; i < resource_obtain_table[a.get_first() - 1].size(); i++) {
        if (a.get_second() == resource_obtain_table[a.get_first() - 1][i].get_first()) {
            R[a.get_second() - 1] -= a.get_third();
            resource_obtain_table[a.get_first() - 1][i].change_third(
                    resource_obtain_table[a.get_first() - 1][i].get_third() + a.get_third());
            break;
        }
    }
    return is_safe_recursively(R, resource_obtain_table);
}
/* Run Banker
 *   while (not finished)
 *     check blocked queue and deal with the tasks in the queue
 *       check if the state is safe before really grant the request
 *     check each task except the ones have been done in blocked queue
 *       when an activity is request, check if the state is safe before really grant the request
 *     check if the task is terminated
 *     restore the resources that are released in this cycle
 *   */
void Banker(unsigned long task_number, std::vector<std::queue<Activity>> T, unsigned long resource_number, std::vector<int> R) {
    int count_cycle = 1; // record the cycle number
    std::vector<int> task_time_taken(task_number, 0); // store the time taken for each task
    std::vector<int> task_waiting_time(task_number, 0); // store the waiting time for each task
    std::vector<Activity> blocked; // store the blocked tasks
    std::vector<unsigned int> task_done_in_check_blocked; // store the tasks done in "check blocked tasks"
    std::vector<Activity> temp_vector; // temp vector to initialize resource_obtain_table
    std::vector<std::vector<Activity>> resource_obtain_table(task_number, temp_vector); // store the initial claims and resource arrangement status
    while (!empty_T(T)) { // while at least one activity exits
        std::vector<int> release(resource_number, 0);
        // check blocked tasks first
        for (int i = 0; i < blocked.size(); i++) {
            Activity a = blocked[i];
            task_done_in_check_blocked.push_back(a.get_first());
            if (a.get_third() <= R[a.get_second() - 1]) {
                if (is_safe(a, R, resource_obtain_table)) {
                    R[a.get_second() - 1] -= a.get_third();
                    for (int j = 0; j < resource_obtain_table[i].size(); j++) {
                        if (resource_obtain_table[a.get_first() - 1][j].get_first() == a.get_second()) {
                            resource_obtain_table[a.get_first() - 1][j].change_third(
                                    resource_obtain_table[a.get_first() - 1][j].get_third() + a.get_third());
                            break;
                        }
                    }
                    blocked.erase(blocked.begin() + i);
                    i--;
                }
                else {
                    task_waiting_time[a.get_first() - 1]++;
                }
            }
            else {
                task_waiting_time[a.get_first() - 1]++;
            }
        }
        // deal with other tasks
        for (int i = 0; i < task_number; i++) {
            // if the current task doesn't have any activity, then skip this task
            if (T[i].empty()) continue;
            // if the current task is already done in check blocked, then skip this task
            if (check_done(task_done_in_check_blocked, T[i].front().get_first())) continue;
            // otherwise, process current task
            Activity a = T[i].front();
            if (a.get_name().compare("initiate") == 0) {
                // resource resource_type claim_number obtain_number
                if (a.get_third() > R[a.get_second() - 1]) {
                    std::cout << "Banker aborts task " << i + 1 << " before run begins:" << std::endl;
                    std::cout << "\tclaim for resource " << a.get_second()
                              << " (" << a.get_third() << ") exceeds number of units present ("
                              <<  R[a.get_second() - 1] << ")" << std::endl;
                    while(!T[i].empty()) {
                        T[i].pop();
                    }
                    task_time_taken[i] = 0;
                    continue;
                }
                Activity tmp("resource", a.get_second(), a.get_third(), 0);
                resource_obtain_table[i].push_back(tmp);
                T[i].pop();
            }
            else if (a.get_name().compare("request") == 0) {
                bool abort = false;
                for (int j = 0; j < resource_obtain_table[i].size(); j++) {
                    if (resource_obtain_table[i][j].get_first() == a.get_second()) {
                        if (resource_obtain_table[i][j].get_second() < resource_obtain_table[i][j].get_third() + a.get_third()) {
                            std::cout << "During cycle " << count_cycle - 1 << "-" << count_cycle << " of Banker's algorithms" << std::endl;
                            std::cout << "\tTask " << i + 1 << "'s request exceeds its claim; aborted; "
                                      << resource_obtain_table[i][j].get_third() << " units of resource "
                                      << resource_obtain_table[i][j].get_second() << " will be available next cycle" << std::endl;
                            abort = true;
                            break;
                        }
                    }
                }
                if (abort) {
                    for (int j = 0; j < resource_obtain_table[i].size(); j++) {
                        release[resource_obtain_table[i][j].get_first() - 1] += resource_obtain_table[i][j].get_third();
                        resource_obtain_table[i][j].change_third(0);
                    }
                    task_time_taken[i] = 0;
                    while (!T[i].empty()) {
                        T[i].pop();
                    }
                    continue;
                }
                if (a.get_third() <= R[a.get_second() - 1]) {
                    if (is_safe(a, R, resource_obtain_table)) {
                        R[a.get_second() - 1] -= a.get_third();
                        for (int j = 0; j < resource_obtain_table[i].size(); j++) {
                            if (resource_obtain_table[i][j].get_first() == a.get_second()) {
                                resource_obtain_table[i][j].change_third(
                                        resource_obtain_table[i][j].get_third() + a.get_third());
                                break;
                            }
                        }
                    }
                    else {
                        blocked.push_back(a);
                        task_waiting_time[i]++;
                    }
                }
                else {
                    blocked.push_back(a);
                    task_waiting_time[i]++;
                }
                T[i].pop();
            }
            else if (a.get_name().compare("release") == 0) {
                release[a.get_second() - 1] += a.get_third();
                for (int j = 0; j < resource_obtain_table[i].size(); j++) {
                    if (resource_obtain_table[i][j].get_first() == a.get_second()) {
                        resource_obtain_table[i][j].change_third(resource_obtain_table[i][j].get_third() - a.get_third());
                        break;
                    }
                }
                T[i].pop();
            }
            else if (a.get_name().compare("compute") == 0) {
                T[i].pop();
            }
            a = T[i].front();
            if (a.get_name().compare("terminate") == 0) {
                T[i].pop();
                task_time_taken[i] = count_cycle;
            }
        }
        // restore the release resources
        for (int i = 0; i < resource_number; i++) {
            R[i] += release[i];
        }
        task_done_in_check_blocked.clear();
        count_cycle++;
    }
    print_result("Banker", task_number, task_time_taken, task_waiting_time);
}

#endif //BANKER_BANKER_H
