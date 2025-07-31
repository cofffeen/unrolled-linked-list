#include <iostream>

#include <unrolled_list.h>
#include <list>

int main(int argc, char **argv) {
    std::list<int> std_list;
    unrolled_list<int> a;

    for (int i = 0; i < 5; ++i)
    {
        std_list.push_back(i);
        a.push_back(i);
    }

    for (auto it = a.begin(); it != a.end(); ++it)
    {
        std::cout << *(it) << "\n";
    }
    return 0;
}
