# stable-in place-sorting-algorithms

Included are implementations of common sorting algorithms as well as new sorting algorithms merge_sweep_sort and zip_sort.

They have the following characteristics.

| Name | Inplace | Stable | Average complexity (Big O) | Worst case complexity (Big O) | Stack memory | Additional memory | time sorting 1000 random numbers | time sorting 80,000 random numbers |
| --- | --- | --- | --- | --- | --- | --- | ---: | ---: |
| selection_sort | Yes | Yes | O(n<sup>2</sup>) | O(n<sup>2</sup>) | (1) | - | 1860 | 9042992 |
| bubble_sort | Yes | Yes | O(n<sup>2</sup>) | O(n<sup>2</sup>) | (1) | - | 735 | 8395577 |
| cocktail_shaker_sort | Yes | Yes | O(n<sup>2</sup>) | O(n<sup>2</sup>) | (1) | - | 884 | 5872503 |
| double_selection_sort | Yes | Yes | O(n<sup>2</sup>) | O(n<sup>2</sup>) | (1) | - | 941 | 4546028 |
| inplace_merge_sort | Yes | Yes | O(n log n) | O(n log n) | (1) when optimised | - | 260 | 946125 |
| hybrid_inplace_merge_sort | Yes | Yes | O(n log n) | O(n log n) | (1) when optimised | - | 212 | 841530 |
| insertion_sort | Yes | Yes | O(n<sup>2</sup>) | O(n<sup>2</sup>) | (1) | - | 177 | 758073 |
| binary_insertion_sort | Yes | Yes | O(n log n) | O(n log n) | (1) | - | 216 | 636183 |
| zip_sort | Yes | Yes | O(n log n) | O(n log n) | (1) when optimised | - | 91 | 53456 |
| hybrid_zip_sort | Yes | Yes | O(n log n) | O(n log n) | (1) when optimised | - | 73 | 51774 |
| merge_sweep_sort | Yes | Yes | O(n log n) | O(n<sup>2</sup>) | approx (log N) | - | 252 | 30865 |
| new_zip_sort | Yes | Yes<sup>(optional)</sup> | O(n log n) | O(n log n) | approx (log N)<sup>(optionally 1)</sup> | - | 205 | 25388 |
| hybrid_new_zip_sort | Yes | Yes<sup>(optional)</sup> | O(n log n) | O(n log n) | approx (log N)<sup>(optionally 1)</sup> | - | 92 | 22012 |
| rotate_merge_sort | Yes | Yes | O(n log n) | O(n log n) | approx (log N) | - | 176 | 21205 |
| hybrid_rotate_merge_sort | Yes | Yes | O(n log n) | O(n log n) | approx (log N) | - | 104 | 18744 |
| stable_quick_sort | Yes | Yes | O(n log n) | O(n<sup>2</sup>) | approx (log N) | (N) | 66 | 7012 |
| quick_sort | Yes | No | O(n log n) | O(n<sup>2</sup>) | approx (log N) | - | 52 | 6908 |
| adaptive_stable_quick_sort | Yes | Yes | O(n log n) | O(n<sup>2</sup>) | approx (log N) | (N) | 74 | 6714 |
| adaptive_stable_intro_sort | Yes | Yes | O(n log n) | O(n log n) | approx (log N) | (N) | 57 | 6048 |
| merge_sort | No | Yes | O(n log n) | O(n log n) | (1) when optimised | (N) | 45 | 6041 |
| std::sort | Yes | No | O(n log n) | O(n log n) | approx (log N) | - | 44 | 5421 |
| hybrid_merge_sort | No | Yes | O(n log n) | O(n log n) | (1) when optimised | (N) | 37 | 5228 |
| intro_sort | Yes | No | O(n log n) | O(n log n) | approx (log N) | - | 36 | 4894 |
| adaptive_intro_sort | Yes | No | O(n log n) | O(n log n) | approx (log N) | - | 37 | 4755 |
| std::stable_sort | No | Yes | O(n log n) | O(n log n) | (1) | (N) | 36 | 4738 |

(All tests with MSVC compiler in release x64)
(times in microseconds, times are only indicitive as they are somewhat data dependant)

NOTE: as default zip_sort and merge_sort (and hybird_zip_sort, hybird_merge_sort) use the optimised constant stack memory algorithm (1).
Also sweep_sort has since been removed entirely due to redundancy (slower than both zip_sort and merge_sweep_sort).

This is presented for those looking to study some new sorting techniques and who are interested in sorting algorithms in general.

The idea for merge_sweep_sort and zip_sort came from the idea of using std::rotate as part of a recursive decent sorting algorithm, and so merge_sweep_sort and zip_sort make use of the rotate function. zip_sort came later and was added in May 2020.

hybrid_rotate_merge_sort gives the best overall performance for an in-place, stable sorting algorithm that uses no additional memory.
zip_sort gives the best overall performance for an in-place, stable sorting algorithm that uses constant memory space, as well as giving O(n log n) worst case performance. As of June 2020 a new much faster version of zip_sort was created, named new_zip_sort, this is new a version of the original idea but instead of appending to the end of the middle buffer new_zip_sort simply keeps track of the contents of the middle buffer as it is built, this gives new_zip_sort a different memory/performance profile than zip_sort that can be seen in the table above.

merge_sweep_sort could be said to be quick_sort like algorithm.

zip_sort is a merge_sort like algorithm. It's merge function does everything in-place, unlike merge_sort, while also providing excellent speed (minimal moves).

A video of zipsort at work can be found here: https://www.youtube.com/watch?v=P9qWTsUvMP8

See below for an indepth description of both algorithms.

As of May 2020 we introduced intro_sort, this is an implementation similar to std::sort. However our intro_sort algorithm is commonly faster than both std::sort and std::stable_sort in our tests. See table above. (NOTE intro_sort is not a stable sorting algorithm as it builds from quick_sort.)

Adaptive version of stable_quick_sort and intro_sort use a more complex function for finding a pivot, this has a minor, but non-zero, overhead. This is to avoid the probable worst case performance that occurs in quick_sort when the input data is flat (aka many items in the input list are equal) or when the input is already sorted/partially sorted. Meaning these versions of algorithms stable_quick_sort and intro_sort perform much better in scenarios where there are some equal/sorted items in the input list, and so should be prefered, but are otherwise identical to those algorithms.

hybrid_rotate_merge_sort, hybrid_zip_sort, hybrid_new_zip_sort and hybrid_merge_sort are hybird sorting algorithms, combining insertion_sort with their respective algorithms, and as a result are faster variations of rotate_merge_sort, zip_sort, new_zip_sort and merge_sort respectively.

binary_insertion_sort is a re-thought insertion_sort that searches in the sorted part of the list using binary search to find the insertion point. The algorithm is still does O(n<sup>2</sup>) writes, best and worst case, but now only does O(n log n) comparisons and so is faster in some cases. NOTE binary_insertion_sort is still slower than insertion_sort when sorting a small number of items.

rotate_merge_sort (and hybrid_rotate_merge_sort), are merge sort like algorithms that use binary search and rotate to merge 2 sorted lists in-place. They are both faster than zip_sort/hybrid_zip_sort/new_zip_sort/hybrid_new_zip_sort and merge_sweep_sort.
These implementation were inspired by this video https://www.youtube.com/watch?v=AgnSL2ohk2M&t=971s but are simpler implementations of this idea.

inplace_merge_sort and hybrid_inplace_merge_sort were added for comparison with zip_sort and merge_sweep_sort. As the most common in-place merge sort algorithms in use. They have O(n log n) comparisons/time complexity, but perform many more swaps/moves to be efficient when compared with those algorithms.

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
    srand(time(NULL))
    unsigned count = 700;
    bool verbose = false;

    //different sorting tests - sort 700 random numbers
    {
        std::cout << "test quick sort" << std::endl;
        //test quick sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < count; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::quick_sort(vec.begin(), vec.end());
        }

        if(verbose) {
            std::cout << "[" << std::endl;
            for(uint32_t i = 0; i < count; ++i) {
                std::cout << "[ " << vec[i] << "], ";
                if(i > 0 && i % 5 == 0)
                    std::cout << std::endl;
            }
            std::cout << "]" << std::endl;
        }

        std::cout << "sorted : " << stlib::is_sorted(vec.begin(), vec.end()) << std::endl;
    }
    {
        std::cout << "test stable quick sort" << std::endl;
        //test stable quick sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < count; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::stable_quick_sort(vec.begin(), vec.end());
        }

        if(verbose) {
            std::cout << "[" << std::endl;
            for(uint32_t i = 0; i < count; ++i) {
                std::cout << "[ " << vec[i] << "], ";
                if(i > 0 && i % 5 == 0)
                    std::cout << std::endl;
            }
            std::cout << "]" << std::endl;
        }

        std::cout << "sorted : " << stlib::is_sorted(vec.begin(), vec.end()) << std::endl;
    }
    {
        std::cout << "test adaptive stable quick sort" << std::endl;
        //test adaptive stable quick sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < count; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::adaptive_stable_quick_sort(vec.begin(), vec.end());
        }

        if(verbose) {
            std::cout << "[" << std::endl;
            for(uint32_t i = 0; i < count; ++i) {
                std::cout << "[ " << vec[i] << "], ";
                if(i > 0 && i % 5 == 0)
                    std::cout << std::endl;
            }
            std::cout << "]" << std::endl;
        }

        std::cout << "sorted : " << stlib::is_sorted(vec.begin(), vec.end()) << std::endl;
    }
    {
        std::cout << "test merge sort" << std::endl;
        //test merge sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < count; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::merge_sort(vec.begin(), vec.end());
        }

        if(verbose) {
            std::cout << "[" << std::endl;
            for(uint32_t i = 0; i < count; ++i) {
                std::cout << "[ " << vec[i] << "], ";
                if(i > 0 && i % 5 == 0)
                    std::cout << std::endl;
            }
            std::cout << "]" << std::endl;
        }

        std::cout << "sorted : " << stlib::is_sorted(vec.begin(), vec.end()) << std::endl;
    }
    {
        std::cout << "test hybrid merge sort" << std::endl;
        //test hybrid merge sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < count; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::hybrid_merge_sort(vec.begin(), vec.end());
        }

        if(verbose) {
            std::cout << "[" << std::endl;
            for(uint32_t i = 0; i < count; ++i) {
                std::cout << "[ " << vec[i] << "], ";
                if(i > 0 && i % 5 == 0)
                    std::cout << std::endl;
            }
            std::cout << "]" << std::endl;
        }

        std::cout << "sorted : " << stlib::is_sorted(vec.begin(), vec.end()) << std::endl;
    }
    {
        std::cout << "test in-place merge sort" << std::endl;
        //test in-place merge sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < count; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::inplace_merge_sort(vec.begin(), vec.end());
        }

        if(verbose) {
            std::cout << "[" << std::endl;
            for(uint32_t i = 0; i < count; ++i) {
                std::cout << "[ " << vec[i] << "], ";
                if(i > 0 && i % 5 == 0)
                    std::cout << std::endl;
            }
            std::cout << "]" << std::endl;
        }

        std::cout << "sorted : " << stlib::is_sorted(vec.begin(), vec.end()) << std::endl;
    }
    {
        std::cout << "test hybrid in-place merge sort" << std::endl;
        //test hybrid in-place merge sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < count; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::hybird_inplace_merge_sort(vec.begin(), vec.end());
        }

        if(verbose) {
            std::cout << "[" << std::endl;
            for(uint32_t i = 0; i < count; ++i) {
                std::cout << "[ " << vec[i] << "], ";
                if(i > 0 && i % 5 == 0)
                    std::cout << std::endl;
            }
            std::cout << "]" << std::endl;
        }

        std::cout << "sorted : " << stlib::is_sorted(vec.begin(), vec.end()) << std::endl;
    }
    {
        std::cout << "test rotate merge sort" << std::endl;
        //test rotate merge sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < count; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::rotate_merge_sort(vec.begin(), vec.end());
        }

        if(verbose) {
            std::cout << "[" << std::endl;
            for(uint32_t i = 0; i < count; ++i) {
                std::cout << "[ " << vec[i] << "], ";
                if(i > 0 && i % 5 == 0)
                    std::cout << std::endl;
            }
            std::cout << "]" << std::endl;
        }

        std::cout << "sorted : " << stlib::is_sorted(vec.begin(), vec.end()) << std::endl;
    }
    {
        std::cout << "test hybrid rotate merge sort" << std::endl;
        //test hybrid rotate merge sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < count; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::hybird_rotate_merge_sort(vec.begin(), vec.end());
        }

        if(verbose) {
            std::cout << "[" << std::endl;
            for(uint32_t i = 0; i < count; ++i) {
                std::cout << "[ " << vec[i] << "], ";
                if(i > 0 && i % 5 == 0)
                    std::cout << std::endl;
            }
            std::cout << "]" << std::endl;
        }

        std::cout << "sorted : " << stlib::is_sorted(vec.begin(), vec.end()) << std::endl;
    }
    {
        std::cout << "test zip sort" << std::endl;
        //test zip sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < count; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::zip_sort(vec.begin(), vec.end());
        }

        if(verbose) {
            std::cout << "[" << std::endl;
            for(uint32_t i = 0; i < count; ++i) {
                std::cout << "[ " << vec[i] << "], ";
                if(i > 0 && i % 5 == 0)
                    std::cout << std::endl;
            }
            std::cout << "]" << std::endl;
        }

        std::cout << "sorted : " << stlib::is_sorted(vec.begin(), vec.end()) << std::endl;
    }
    {
        std::cout << "test hybrid zip sort" << std::endl;
        //test hybrid zip sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < count; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::hybrid_zip_sort(vec.begin(), vec.end());
        }

        if(verbose) {
            std::cout << "[" << std::endl;
            for(uint32_t i = 0; i < count; ++i) {
                std::cout << "[ " << vec[i] << "], ";
                if(i > 0 && i % 5 == 0)
                    std::cout << std::endl;
            }
            std::cout << "]" << std::endl;
        }

        std::cout << "sorted : " << stlib::is_sorted(vec.begin(), vec.end()) << std::endl;
    }
    {
        std::cout << "test new zip sort" << std::endl;
        //test new zip sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < count; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::new_zip_sort(vec.begin(), vec.end());
        }

        if(verbose) {
            std::cout << "[" << std::endl;
            for(uint32_t i = 0; i < count; ++i) {
                std::cout << "[ " << vec[i] << "], ";
                if(i > 0 && i % 5 == 0)
                    std::cout << std::endl;
            }
            std::cout << "]" << std::endl;
        }

        std::cout << "sorted : " << stlib::is_sorted(vec.begin(), vec.end()) << std::endl;
    }
    {
        std::cout << "test hybrid new zip sort" << std::endl;
        //test hybrid new zip sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < count; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::hybrid_new_zip_sort(vec.begin(), vec.end());
        }

        if(verbose) {
            std::cout << "[" << std::endl;
            for(uint32_t i = 0; i < count; ++i) {
                std::cout << "[ " << vec[i] << "], ";
                if(i > 0 && i % 5 == 0)
                    std::cout << std::endl;
            }
            std::cout << "]" << std::endl;
        }

        std::cout << "sorted : " << stlib::is_sorted(vec.begin(), vec.end()) << std::endl;
    }
    {
        std::cout << "test merge sweep sort" << std::endl;
        //test merge sweep sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < count; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::merge_sweep_sort(vec.begin(), vec.end());
        }

        if(verbose) {
            std::cout << "[" << std::endl;
            for(uint32_t i = 0; i < count; ++i) {
                std::cout << "[ " << vec[i] << "], ";
                if(i > 0 && i % 5 == 0)
                    std::cout << std::endl;
            }
            std::cout << "]" << std::endl;
        }

        std::cout << "sorted : " << stlib::is_sorted(vec.begin(), vec.end()) << std::endl;
    }
    {
        std::cout << "test bubble sort" << std::endl;
        //test bubble sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < count; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::bubble_sort(vec.begin(), vec.end());
        }

        if(verbose) {
            std::cout << "[" << std::endl;
            for(uint32_t i = 0; i < count; ++i) {
                std::cout << "[ " << vec[i] << "], ";
                if(i > 0 && i % 5 == 0)
                    std::cout << std::endl;
            }
            std::cout << "]" << std::endl;
        }

        std::cout << "sorted : " << stlib::is_sorted(vec.begin(), vec.end()) << std::endl;
    }
    {
        std::cout << "test cocktail shaker sort" << std::endl;
        //test cocktail shaker sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < count; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::cocktail_shaker_sort(vec.begin(), vec.end());
        }

        if(verbose) {
            std::cout << "[" << std::endl;
            for(uint32_t i = 0; i < count; ++i) {
                std::cout << "[ " << vec[i] << "], ";
                if(i > 0 && i % 5 == 0)
                    std::cout << std::endl;
            }
            std::cout << "]" << std::endl;
        }

        std::cout << "sorted : " << stlib::is_sorted(vec.begin(), vec.end()) << std::endl;
    }
    {
        std::cout << "test insertion sort" << std::endl;
        //test insertion sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < count; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::insertion_sort(vec.begin(), vec.end());
        }

        if(verbose) {
            std::cout << "[" << std::endl;
            for(uint32_t i = 0; i < count; ++i) {
                std::cout << "[ " << vec[i] << "], ";
                if(i > 0 && i % 5 == 0)
                    std::cout << std::endl;
            }
            std::cout << "]" << std::endl;
        }

        std::cout << "sorted : " << stlib::is_sorted(vec.begin(), vec.end()) << std::endl;
    }
    {
        std::cout << "test binary insertion sort" << std::endl;
        //test binary insertion sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < count; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::binary_insertion_sort(vec.begin(), vec.end());
        }

        if(verbose) {
            std::cout << "[" << std::endl;
            for(uint32_t i = 0; i < count; ++i) {
                std::cout << "[ " << vec[i] << "], ";
                if(i > 0 && i % 5 == 0)
                    std::cout << std::endl;
            }
            std::cout << "]" << std::endl;
        }

        std::cout << "sorted : " << stlib::is_sorted(vec.begin(), vec.end()) << std::endl;
    }
    {
        std::cout << "test intro sort" << std::endl;
        //test intro sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < count; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::intro_quick_sort(vec.begin(), vec.end());
        }

        if(verbose) {
            std::cout << "[" << std::endl;
            for(uint32_t i = 0; i < count; ++i) {
                std::cout << "[ " << vec[i] << "], ";
                if(i > 0 && i % 5 == 0)
                    std::cout << std::endl;
            }
            std::cout << "]" << std::endl;
        }

        std::cout << "sorted : " << stlib::is_sorted(vec.begin(), vec.end()) << std::endl;
    }
    {
        std::cout << "test adaptive intro sort" << std::endl;
        //test adaptive intro sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < count; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::intro_sort(vec.begin(), vec.end());
        }

        if(verbose) {
            std::cout << "[" << std::endl;
            for(uint32_t i = 0; i < count; ++i) {
                std::cout << "[ " << vec[i] << "], ";
                if(i > 0 && i % 5 == 0)
                    std::cout << std::endl;
            }
            std::cout << "]" << std::endl;
        }

        std::cout << "sorted : " << stlib::is_sorted(vec.begin(), vec.end()) << std::endl;
    }
    {
        std::cout << "test adaptive stable intro sort" << std::endl;
        //test adaptive stable intro sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < count; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            stlib::adaptive_stable_intro_sort(vec.begin(), vec.end());
        }

        if(verbose) {
            std::cout << "[" << std::endl;
            for(uint32_t i = 0; i < count; ++i) {
                std::cout << "[ " << vec[i] << "], ";
                if(i > 0 && i % 5 == 0)
                    std::cout << std::endl;
            }
            std::cout << "]" << std::endl;
        }

        std::cout << "sorted : " << stlib::is_sorted(vec.begin(), vec.end()) << std::endl;
    }
    {
        std::cout << "test std::sort" << std::endl;
        //test std::sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < count; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            std::sort(&*vec.begin(), &*vec.end());
        }

        if(verbose) {
            std::cout << "[" << std::endl;
            for(uint32_t i = 0; i < count; ++i) {
                std::cout << "[ " << vec[i] << "], ";
                if(i > 0 && i % 5 == 0)
                    std::cout << std::endl;
            }
            std::cout << "]" << std::endl;
        }

        std::cout << "sorted : " << stlib::is_sorted(vec.begin(), vec.end()) << std::endl;
    }
    {
        std::cout << "test std::stable_sort" << std::endl;
        //test std::stable_sort
        std::vector<uint32_t> vec;
        for(uint32_t i = 0; i < count; ++i)
            vec.push_back(rand());

        {
            timer tmr;
            std::stable_sort(&*vec.begin(), &*vec.end());
        }

        if(verbose) {
            std::cout << "[" << std::endl;
            for(uint32_t i = 0; i < count; ++i) {
                std::cout << "[ " << vec[i] << "], ";
                if(i > 0 && i % 5 == 0)
                    std::cout << std::endl;
            }
            std::cout << "]" << std::endl;
        }

        std::cout << "sorted : " << stlib::is_sorted(vec.begin(), vec.end()) << std::endl;
    }

    return 0;
}
```

# new algorithm descriptions

(descriptions originally posted on reddit)

## merge_sweep_sort

The following is an description of the merge_sweep_sort algorithm, merge_sweep_sort is a quick_sort like algorithm.

```
Step 1: pick a pivot

Step 2: Recursive half the size of the list until we get to a list of length 1 or 2.

    a : if length 1 return the items indicated as in sublist 1, containing the items below the pivot or sublist 2, containing the items above the pivot.

    b : if length 2 sort the items, if below the pivot move to the left, if above move to the right. return the items indicated as in sublist 1, containing the items below the pivot or sublist 2, containing the items above the pivot.

    c : else Combine the left and right halfs, stlib::rotate the bottom sublist of the left (those greater than the pivot) with the top half of the right (those less than the pivot).

    d : you have now created two new sublists. return the items indicated as in sublist 1, containing the items below the pivot or sublist 2, containing the items above the pivot.

Step 3: Once done you have all of the items greater than the pivot on one side and all those less than the pivot on the other. Move the pivot into place.

Step 4: Repeat from Step 1 for those greater than the pivot and those less than the pivot.

Step 5: The list is sorted.
```

Merge sweep sort is an interesting sorting algorithm as it simply resembles no other sorting algorithm. It exclusively moves items using the rotate function (instead of swap). Allowing it to be an in-place sorting algorithm that compares to both merge_sort and quick_sort in behavoir while doing the work of neither. It is possibly the fastest in-place, stable sorting algorithm that uses only stack space (log N).

## zip_sort

The zip_sort outer function simply repeats the zip_merge across the whole list until we reach array_size > length, at which point we can stop. You can think of this as subdividing the array until we have lengths of 2, which we zip_merge back together to get lengths of 4 then 8 then 16 etc..

```
Step 1: starting at array_size = 1

Step 2: Repeat Step 3 for all items in the list, move forward by array_size * 2

    Step 3: Take 2 array sublists next to each other, both of length array_size, apply zip_merge (sublist form the left and right buffers)

Step 4: array_size = array_size * 2

Step 5: Repeat from Step 2 if array_size < length
```


The zip_merge is just like a merge_sort merge function except everything remains in place. Put simply the function keeps four buffers the output buffer, the left buffer, the right buffer and the middle buffer.

Given left and right buffers, both sorted, the algorithm proceeds to merge them as follows:

```
Repeat until we have no items left (left buffer or right buffer is empty)

	if we have any items in the middle buffer compare the middle buffer item with the right buffer item

		a: if the right buffer has the smaller item move it into the output buffer. Move the left buffer item onto the end of the middle buffer.

		b: if the middle buffer has the smaller item move it into the output buffer. Move the left buffer item onto the end of the middle buffer.

	else compare the left buffer item with the right buffer item

		a: if the left buffer has the smaller item move it into the output buffer

		b: if the right buffer has the smaller item move it into the output buffer. Move the left buffer item onto the end of the middle buffer.

	if the left buffer has reached the middle buffer, all the items in the middle buffer now form the left buffer, middle buffer is now empty
```

To keep the above algorithm fast the following optimisations are needed.

1. the output buffer precedes the left buffer.

2. The middle buffer is kept as a cicular list that occupies the space between the left buffer and the right buffers.

3. When the left buffer reaches the middle buffer the middle buffer is rearranged into the new left buffer using in-place rotate, middle buffer continues as the space between the left buffer and the right buffers.

These mean the following optimization can be achieved:

 - Moving items from the left buffer to the output buffer is O(1) operation (no memory copy needed).

 - Because only left items are moved into the middle buffer the middle buffer only contains items from the left buffer, so moving the left buffer item to the middle buffer and moving the middle buffer item to the output buffer is O(1) operation. Just swap the item in the left buffer with the item in the middle buffer. This places the top of the middle buffer into the output buffer and the top of the left buffer onto the end of the middle buffer.

 - Moving items from the right buffer to the output buffer involves moving items in the left buffer into the middle buffer, at worst this is (1/4 n) operation. This can be optimised further by moving multiple items at once where it is possible to do so.
 
 - Also because the middle buffer is kept as a cicular list, the middle buffer can be reordered with a rotate operation at any time. This means that by reordering the middle buffer the algorithm can reduce the total number of writes to the middle buffer. The version implemented opts to reorder the middle buffer when the total middle buffer write count exceeds the number of items in the middle buffer, this is close to the optimal time for reordering the middle buffer to reduce the number of total writes.

A well implemented algorithm does exactly n comparisons, and at the very worst approximately (n * (1/4 * n)) swaps but due to the above optimisations the total number of swaps will be much less.

The algorithm works very well when the input has a large number of consecutive "runs" on either the left or right sides as these operations take exactly O(n) for the length of the run. Typically this happens rarely in random data (runs are generally 2 or 3 long, on average, in our tests) but in real world data runs can often be much longer.

Because a well implemented zip_sort algorithm needs to move items around more than 1 item at a time this means that the algorithm makes use of a smaller ammount of stack allocated "scratch space" for the efficient movement of multiple items between the buffers (specifically from the left buffer to the middle buffer). This is constant in size, as it is set at the larger of (1 x sizeof(T)) or 2048 bytes. This means the algorithm can move (trunc(2048/sizeof(T)) or 1) items in a single iteration.

So long as the algorithm doesn't run out of scratch space (very unlikely), the algorithm achieves exactly (n log n) comparisons, the minimum possible for a comparison sorting algorithm. However the number of swaps/writes remains high to keep all items in-place but are significantly reduced by the efficient management of the 4 buffers.

zip_sort acheives an incredible O(n log n) average and worst case performance with constant memory through a variety of neat tricks all while acheiving very good runtime performance.

As of June 2020 a new much faster version of zip_sort was created, named new_zip_sort, this is a new version of the original idea but instead of appending to the end of the middle buffer, new_zip_sort simply keeps track of the contents of the middle buffer as it is built and removed from. This gives new_zip_sort a different memory/performance profile than zip_sort. Instead of the above optimisations new_zip_sort keeps a number of circular buffers in the middle buffer (or in other words many middle buffers), this has the pleasing effect that when moving from the left buffer into the middle buffer you can limit the number of items that need to be moved about in the middle buffer to append the left item. This hugely improves performance at the cost of having to keep track of where the next item in the middle buffer is located, as it could be on the end of any of the circular lists. This can be done efficiently using a list of iterators (representing each of the circular buffers) and a circular list of indexes (with counts) into those iterators representing the next item from the middle buffer, all kept on the stack. When we reach the middle buffer it is reordered into the new left buffer, just as with zip_sort, however because this is made up of many circular buffers these are, instead, merged together before continuing.

new_zip_sort, unlike zip_sort, is not stable by default instead it has the option to be stable or instable, at a very small performance cost, a flag can be set (set by default) to make new_zip_sort stable. This has the cost of one additional comparison every time an item is moved into the middle buffer.

Details for intro_sort can be found on wikipedia https://en.wikipedia.org/wiki/Introsort.

Please use and let me know what you think.

Thanks

ceorron

aka

Richard Cookman

