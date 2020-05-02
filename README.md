# stable-in place-sorting-algorithms

Included are implementations of common sorting algorithms as well as new sorting algorithms sweep_sort, merge_sweep_sort and zip_sort.

They have the following characteristics.

| Name | Inplace | Stable | Average Complexity (Big O) | Worst Case Complexity (Big O) | Additional memeory | time sorting 1000 random numbers | time sorting 80,000 random numbers |
| --- | --- | --- | --- | --- | --- | ---: | ---: |
| bubble_sort | Yes | Yes | O(n<sup>2</sup>) | O(n<sup>2</sup>) | - | 1039 | 9387560 |
| insertion_sort | Yes | Yes | O(n<sup>2</sup>) | O(n<sup>2</sup>) | - | 203 | 1068614 |
| sweep_sort | Yes | Yes | O(n log n) | O(n<sup>2</sup>) | - | 195 | 744087 |
| zip_sort | Yes | Yes | O(n log n) | O(n log n) | - | 91 | 136853 |
| merge_sweep_sort | Yes | Yes | O(n log n) | O(n<sup>2</sup>) | - | 230 | 31555 |
| stable_quick_sort | Yes | Yes | O(n log n) | O(n<sup>2</sup>) | (N) | 63 | 8837 |
| merge_sort | No | Yes | O(n log n) | O(n log n) | (N) | 56 | 7033 |
| quick_sort | Yes | No | O(n log n) | O(n<sup>2</sup>) | - | 50 | 5892 |
| std::sort | Yes | No | O(n log n) | O(n<sup>2</sup>) | - | 43 | 5463 |
| std::stable_sort | No | Yes | O(n log n) | O(n log n) | (N) | 36 | 4943 |

(All tests with MSVC compiler in release x64)
(times in microseconds)

This is presented for those looking to study some new sorting techniques and who are interested in sorting algorithms in general.

The idea for sweep_sort, merge_sweep_sort and zip_sort came from the idea of using std::rotate as part of a recursive decent sorting algorithm, and so both sweep_sort, merge_sweep_sort and zip_sort make use of the rotate function. zip_sort came later and was added in May 2020.

merge_sweep_sort gives the best overall performance for an in-place, stable sorting algorithm that uses no additional memory.
zip_sort gives the best overall performance for an in-place, stable sorting algorithm that uses no additional memory, as well as giving O(n log n) worst case performance.

sweep_sort and merge_sweep_sort could be said to be quick_sort like algorithms.
zip_sort is a merge_sort like algorithm. It's merge function does everything in-place unlike merge_sort while also providing excellent speed (minimal moves).

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
        std::cout << "test sweep sort" << std::endl;
        //test sweep sort
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
        vvector<uint32_t> vec;
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

