# stable-in place-sorting-algorithms

Included are implementations of quick_sort and merge_sort as well as new sorting algorithms sweep_sort and merge_sweep_sort.

They have the following characteristics.

| Name | Inplace | Stable | Average Complexity (Big O) | Worst Case Complexity (Big O) | Additional memeory | time sorting 1000 random numbers | time sorting 80,000 random numbers |
| --- | --- | --- | --- | --- | --- | --- | --- |
| quick_sort | Yes | No | O(n log n) | O(n<sup>2</sup>) | No | 119 | 10128 |
| merge_sort | No | Yes | O(n log n) | O(n log n) | (N) | 57 | 7848 |
| sweep_sort | Yes | Yes | O(n log n) | O(n<sup>2</sup>) | No | 284 | 769621 |
| merge_sweep_sort | Yes | Yes | O(n log n) | O(n<sup>2</sup>) | No | 339 | 41920 |
| bubble_sort | Yes | Yes | O(n<sup>2</sup>) | O(n<sup>2</sup>) | No | 1026 | 9596190 |

(All tests with MSVC compiler in release x64)
(times in microseconds)

This is presented for those looking to study some new sorting techniques and who are interested in sorting algorithms in general.

The idea for sweep_sort and merge_sweep_sort came from the idea of using std::rotate as part of a recursive decent sorting algorithm, and so both sweep_sort and merge_sweep_sort make use of the rotate function.

# Example use - C++

(test in main.cpp)

```C++
#include <vector>
#include <iostream>
#include <chrono>
#include <stdlib.h>

#include "sort.hpp"

struct timer {
    std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
    timer() {
        m_StartTimepoint = std::chrono::high_resolution_clock::now();
    }
    ~timer() {
        auto endTimepoint = std::chrono::high_resolution_clock::now();

        long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
        long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

        std::cout << "running time : " << end - start << std::endl;;
    }
};

int main() {
    //just pick a random seed
    srand(time(NULL));

    //different sorting tests - sort 700 random numbers
    {
        std::cout << "test quick sort" << std::endl;
        //test quick sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < 700; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::quick_sort(vec.begin(), vec.end());
        }

        std::cout << "[" << std::endl;
        for(uint32_t i = 0; i < 700; ++i) {
            std::cout << "[ " << vec[i] << "], ";
            if(i > 0 && i % 5 == 0)
                std::cout << std::endl;
        }
        std::cout << "]" << std::endl;

        std::cout << "sorted : " << stlib::is_sorted(vec.begin(), vec.end()) << std::endl;
    }
    {
        std::cout << "test merge sort" << std::endl;
        //test merge sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < 700; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::merge_sort(vec.begin(), vec.end());
        }

        std::cout << "[" << std::endl;
        for(uint32_t i = 0; i < 700; ++i) {
            std::cout << "[ " << vec[i] << "], ";
            if(i > 0 && i % 5 == 0)
                std::cout << std::endl;
        }
        std::cout << "]" << std::endl;

        std::cout << "sorted : " << stlib::is_sorted(vec.begin(), vec.end()) << std::endl;
    }
    {
        std::cout << "test sweep sort" << std::endl;
        //test stable quick sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < 700; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::sweep_sort(vec.begin(), vec.end());
        }

        std::cout << "[" << std::endl;
        for(uint32_t i = 0; i < 700; ++i) {
            std::cout << "[ " << vec[i] << "], ";
            if(i > 0 && i % 5 == 0)
                std::cout << std::endl;
        }
        std::cout << "]" << std::endl;

        std::cout << "sorted : " << stlib::is_sorted(vec.begin(), vec.end()) << std::endl;
    }
    {
        std::cout << "test merge sweep sort" << std::endl;
        //test sweep merge sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < 700; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::merge_sweep_sort(vec.begin(), vec.end());
        }

        std::cout << "[" << std::endl;
        for(uint32_t i = 0; i < 700; ++i) {
            std::cout << "[ " << vec[i] << "], ";
            if(i > 0 && i % 5 == 0)
                std::cout << std::endl;
        }
        std::cout << "]" << std::endl;

        std::cout << "sorted : " << stlib::is_sorted(vec.begin(), vec.end()) << std::endl;
    }
    {
        std::cout << "test bubble sort" << std::endl;
        //test sweep merge sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < 700; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::bubble_sort(vec.begin(), vec.end());
        }

        std::cout << "[" << std::endl;
        for(uint32_t i = 0; i < 700; ++i) {
            std::cout << "[ " << vec[i] << "], ";
            if(i > 0 && i % 5 == 0)
                std::cout << std::endl;
        }
        std::cout << "]" << std::endl;

        std::cout << "sorted : " << stlib::is_sorted(vec.begin(), vec.end()) << std::endl;
    }

    return 0;
}
```

Please use and let me know what you think.

Thanks

ceorron

aka

Richard Cookman

