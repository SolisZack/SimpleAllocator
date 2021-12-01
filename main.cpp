#include <iostream>
#include <vector>
#include <ctime>
#include <iostream>
#include "List.hpp"
#include "memory_pool.h"

#define REPS 1000000

template <class T>
void test_list(List<int, T>* list) {
    for (int i = 0; i < REPS; i ++) {
        list->push_front(i);
    }
    for (int i = 0; i < REPS; i ++) {
        list->pop_front();
    }
}

int main() {
    List<int, Memory_pool<int>> list_diy;
    List<int> list_stl;
    std::vector<int, Memory_pool<int>> vec_diy;
    std::vector<int> vec_stl;

    std::cout << "Testing using stl allocator" << std::endl;
    clock_t start = clock();
    test_list<std::allocator<int>>(&list_stl);
    std::cout << "Time Used:" << (((double)clock() - (double)start) / CLOCKS_PER_SEC) << "\n\n";

    std::cout << "Testing using diy allocator" << std::endl;
    start = clock();
    test_list<Memory_pool<int>>(&list_diy);
    std::cout << "Time Used:" << (((double)clock() - (double)start) / CLOCKS_PER_SEC) << "\n\n";
    return 0;
}