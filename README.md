# stable-in place-sorting-algorithms

Included are implementations of common sorting algorithms as well as new sorting algorithms merge_sweep_sort and zip_sort.

They have the following characteristics.

| Name | Inplace | Stable | Average complexity (Big O) | Worst case complexity (Big O) | Stack memory | Additional memory | time sorting 1000 random numbers | time sorting 80,000 random numbers |
| --- | --- | --- | --- | --- | --- | --- | ---: | ---: |
| bubble_sort | Yes | Yes | O(n<sup>2</sup>) | O(n<sup>2</sup>) | (1) | - | 1040 | 9225716 |
| insertion_sort | Yes | Yes | O(n<sup>2</sup>) | O(n<sup>2</sup>) | (1) | - | 161 | 1074187 |
| binary_insertion_sort | Yes | Yes | O(n<sup>2</sup>) | O(n<sup>2</sup>) | (1) | - | 205 | 1035926 |
| zip_sort | Yes | Yes | O(n log n) | O(n log n) | (1) when optimised | - | 89 | 55025 |
| hybrid_zip_sort | Yes | Yes | O(n log n) | O(n log n) | (1) when optimised | - | 75 | 51888 |
| merge_sweep_sort | Yes | Yes | O(n log n) | O(n<sup>2</sup>) | approx (log N) | - | 252 | 30865 |
| stable_quick_sort | Yes | Yes | O(n log n) | O(n<sup>2</sup>) | approx (log N) | (N) | 66 | 7012 |
| quick_sort | Yes | No | O(n log n) | O(n<sup>2</sup>) | approx (log N) | - | 52 | 6908 |
| adaptive_stable_quick_sort | Yes | Yes | O(n log n) | O(n<sup>2</sup>) | approx (log N) | (N) | 74 | 6714 |
| merge_sort | No | Yes | O(n log n) | O(n log n) | (1) when optimised | (N) | 45 | 6041 |
| std::sort | Yes | No | O(n log n) | O(n log n) | approx (log N) | - | 44 | 5421 |
| hybrid_merge_sort | No | Yes | O(n log n) | O(n log n) | (1) when optimised | (N) | 37 | 5228 |
| intro_sort | Yes | No | O(n log n) | O(n log n) | approx (log N) | - | 36 | 4894 |
| adaptive_intro_sort | Yes | No | O(n log n) | O(n log n) | approx (log N) | - | 37 | 4848 |
| std::stable_sort | No | Yes | O(n log n) | O(n log n) | (1) | (N) | 36 | 4738 |

(All tests with MSVC compiler in release x64)
(times in microseconds)

NOTE: as default zip_sort and merge_sort (and hybird_zip_sort, hybird_merge_sort) use the optimised constant stack memory algorithm (1).
Also sweep_sort has since been removed entirely due to redundancy (slower than both zip_sort and merge_sweep_sort).

This is presented for those looking to study some new sorting techniques and who are interested in sorting algorithms in general.

The idea for merge_sweep_sort and zip_sort came from the idea of using std::rotate as part of a recursive decent sorting algorithm, and so merge_sweep_sort and zip_sort make use of the rotate function. zip_sort came later and was added in May 2020.

merge_sweep_sort gives the best overall performance for an in-place, stable sorting algorithm that uses no additional memory.
zip_sort gives the best overall performance for an in-place, stable sorting algorithm that uses no additional memory, as well as giving O(n log n) worst case performance.

merge_sweep_sort could be said to be quick_sort like algorithms.

zip_sort is a merge_sort like algorithm. It's merge function does everything in-place, unlike merge_sort, while also providing excellent speed (minimal moves).

As of May 2020 we introduced intro_sort, this is an implementation similar to std::sort. However our intro_sort algorithm is commonly faster than both std::sort and std::stable_sort in our tests. See table above. (NOTE intro_sort is not a stable sorting algorithm as it builds from quick_sort.)

Adaptive version of stable_quick_sort and intro_sort use a more complex function for finding a pivot, this has a minor (but non zero overhead). This is to avoid the probable worst case performance that occurs in quick_sort when the input data is flat (aka many items in the input list are equal), meaning these versions of algorithms stable_quick_sort and intro_sort perform much better in scenarios where there are some equal items in the input list, and so should be prefered, but are otherwise identical to those algorithms.

hybrid_zip_sort and hybrid_merge_sort are both hybird sorting algorithms, using insertion_sort with their respective algorithms, and as a result are faster variations of zip_sort and merge_sort respectively.

binary_insertion_sort is a re-thought insertion_sort that searches in the sorted part of the list using binary search to find the insertion point. The algorithm is still O(n<sup>2</sup>), best and worst case, but now only does O(n log n) comparisons and so is faster in some cases. Note binary_insertion_sort is still slower than insertion_sort when sorting a small number of items.

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

# new algorithm descriptions

(descriptions originally posted on reddit)

## merge_sweep_sort

The following is an explination of the merge_sweep_sort algorithm, merge_sweep_sort is a quick_sort like algorithm.

```
Step 1: pick a pivot

Step 2: Recursive halving the size of the list until we get to list size 1 or 2

    a : if 1 return the items indicated as in sublist 1 containing the items below the pivot or sublist 2 containing the items above the pivot

    b : if 2 sort the items if below the pivot move to the left, if above move to the right return the items indicated as in sublist 1 containing the items below the pivot or sublist 2 containing the items above the pivot

Do Step 2 for both the left and right halfs.

Step 3: else Combine the left and right halfs, stlib::rotate the bottom sublist of the left (those greater than the pivot) with the top half of the right (those less than the pivot).

    c : you have now created two new sublists return the items indicated as in sublist 1 containing the items below the pivot or sublist 2 containing the items above the pivot

Step 4: Once done you have all of the items greater than the pivot on one side and all those less than the pivot on the other. Move the pivot into place.

Step 5: Repeat from Step 1 for those greater than the pivot and those less than the pivot.

Step 6: The list is sorted.
```

Merge sweep sort is an interesting sorting algorithm as it simply resembles no other sorting algorithm. It exclusively moves items using the rotate function (instead of swap). Allowing it to be an in-place sorting algorithm that compares to both merge_sort and quick_sort in behavoir while doing the work of neither. It is possibly the fastest in-place, stable sorting algorithm that uses only stack space (log N).

## zip_sort

The zip_sort outer function simply repeats the zip_merge across the whole list until we reach array_size > length, at which point we can stop. You can think of this as subdividing the array until we have lengths of 2, which we zip_merge back together to get lengths of 4 then 8 then 16 etc..

```
Step 1: starting at array_size = 1

Step 2: no-op

Step 3: Repeat Step 4 for all items in the list move forward by array_size * 2

Step 4: Take 2 array sublists next to each other both of length array_size apply zip_merge (sublist are left and right buffers)

Step 5: array_size = array_size * 2

Step 6: Repeat from Step 2 if array_size < length
```


The zip_merge is just like a merge_sort merge function except everything remains in place. Put simply the function keeps four buffers the output buffer, the left buffer, the right buffer and the middle buffer.

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

1. the output buffer preceeds the left buffer.

2. The middle buffer is kept as a cicular list that occupies the space between the left buffer and the right buffers.

3. When the left buffer reaches the middle buffer the middle buffer is rearranged into the new left buffer using in-place rotate, middle buffer continues as the space between the left buffer and the right buffers.

These mean the following optimization can be achieved:

 - Moving items from the left buffer to the output buffer is O(1) operation (no memory copy needed).

 - Because only left items are moved into the middle buffer the middle buffer only contains items from the left buffer, so moving the left buffer item to the middle buffer and moving the middle buffer item to the output buffer is O(1) operation. Just swap the item in the left buffer with the item in the middle buffer. This places the top of the middle buffer into the output buffer and the top of the left buffer onto the end of the middle buffer.

 - Moving items from the right buffer to the output buffer involves moving items on the left into the middle buffer at worst this is (1/4 n) operation. This can be optimised further by moving multiple items at once where it is possible to do so.

A well implemented algorithm does exactly n comparisons, and at worst approximately (1/2 n(1/4 n)) swaps.

The algorithm works very well when the input has a large number of consecutive "runs" on either the left or right sides as these operations take exactly O(n) for the length of the run. Typically this happens rarely in random data (runs are generally 2 or 3 long, on average, in our tests) but in real world data runs can often be much longer.

Because a well implemented zip_sort algorithm needs to move items around more than 1 item at a time this means that the algorithm makes use of a smaller ammount of stack allocated "scratch space" for the efficient movement of multiple items between the buffers (specifically from the left buffer to the middle buffer). This is constant in size, as it is set at the larger of (1 x sizeof(T)) or 2048 bytes. This means the algorithm can move (trunc(2048/sizeof(T)) or 1) items in a single iteration.

So long as the algorithm doesn't run out of scratch space (very unlikely), the algorithm achieves exactly (n log n) comparisons, the minimum possible for a comparison sorting algorithm. However the number of swaps remains high to keep all items in-place but are significantly reduced by the efficient management of the 4 buffers.

zip_sort acheives an incredible O(n log n) average and worst case performance with constant memory through a variety of neat tricks all while acheiving very good runtime performance.

Details for intro_sort can be found on wikipedia https://en.wikipedia.org/wiki/Introsort.

Please use and let me know what you think.

Thanks

ceorron

aka

Richard Cookman

