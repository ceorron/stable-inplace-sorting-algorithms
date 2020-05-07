# stable-in place-sorting-algorithms

Included are implementations of common sorting algorithms as well as new sorting algorithms merge_sweep_sort and zip_sort.

They have the following characteristics.

| Name | Inplace | Stable | Average complexity (Big O) | Worst case complexity (Big O) | Stack memory | Additional memory | time sorting 1000 random numbers | time sorting 80,000 random numbers |
| --- | --- | --- | --- | --- | --- | --- | ---: | ---: |
| bubble_sort | Yes | Yes | O(n<sup>2</sup>) | O(n<sup>2</sup>) | (1) | - | 1040 | 9383652 |
| insertion_sort | Yes | Yes | O(n<sup>2</sup>) | O(n<sup>2</sup>) | (1) | - | 161 | 1038194 |
| zip_sort | Yes | Yes | O(n log n) | O(n log n) | (1) when optimised | - | 89 | 55025 |
| merge_sweep_sort | Yes | Yes | O(n log n) | O(n<sup>2</sup>) | approx (log N) | - | 284 | 30865 |
| stable_quick_sort | Yes | Yes | O(n log n) | O(n<sup>2</sup>) | approx (log N) | (N) | 76 | 7522 |
| quick_sort | Yes | No | O(n log n) | O(n<sup>2</sup>) | approx (log N) | - | 52 | 5824 |
| merge_sort | No | Yes | O(n log n) | O(n log n) | (1) when optimised | (N) | 45 | 5799 |
| std::sort | Yes | No | O(n log n) | O(n log n) | approx (log N) | - | 41 | 5626 |
| std::stable_sort | No | Yes | O(n log n) | O(n log n) | (1) | (N) | 43 | 4921 |
| intro_sort | Yes | No | O(n log n) | O(n log n) | approx (log N) | - | 49 | 4756 |

(All tests with MSVC compiler in release x64)
(times in microseconds)

NOTE: as default zip_sort and merge_sort use the optimised constant stack memory algorithm (1).
Also sweep_sort as since been removed entirely due to redundancy (slower than both zip_sort and merge_sweep_sort).

This is presented for those looking to study some new sorting techniques and who are interested in sorting algorithms in general.

The idea for merge_sweep_sort and zip_sort came from the idea of using std::rotate as part of a recursive decent sorting algorithm, and so merge_sweep_sort and zip_sort make use of the rotate function. zip_sort came later and was added in May 2020.

merge_sweep_sort gives the best overall performance for an in-place, stable sorting algorithm that uses no additional memory.
zip_sort gives the best overall performance for an in-place, stable sorting algorithm that uses no additional memory, as well as giving O(n log n) worst case performance.

merge_sweep_sort could be said to be quick_sort like algorithms.

zip_sort is a merge_sort like algorithm. It's merge function does everything in-place, unlike merge_sort, while also providing excellent speed (minimal moves).

As of May 2020 we introduced intro_sort, this is an implementation similar to std::sort. However our intro_sort algorithm is commonly faster than both std::sort and std::stable_sort in our tests. See table above. (NOTE intro_sort is not a stable sorting algorithm as it builds from quick_sort.)

# Example use - C++

(test in main.cpp)

```C++
#include <vector>
#include <iostream>
#include <chrono>
#include <stdlib.h>
#include <algorithm>

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
        std::cout << "test stable quick sort" << std::endl;
        //test stable quick sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < 700; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::stable_quick_sort(vec.begin(), vec.end());
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
        std::cout << "test zip sort" << std::endl;
        //test zip sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < 700; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::zip_sort(vec.begin(), vec.end());
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
        //test merge sweep sort
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
        //test bubble sort
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
    {
        std::cout << "test insertion sort" << std::endl;
        //test insertion sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < 700; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::insertion_sort(vec.begin(), vec.end());
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
        std::cout << "test intro sort" << std::endl;
        //test intro sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < 700; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::intro_sort(vec.begin(), vec.end());
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
        std::cout << "test std::sort" << std::endl;
        //test std::sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < 700; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            std::sort(&*vec.begin(), &*vec.end());
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
        std::cout << "test std::stable_sort" << std::endl;
        //test std::stable_sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < 700; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            std::stable_sort(&*vec.begin(), &*vec.end());
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

