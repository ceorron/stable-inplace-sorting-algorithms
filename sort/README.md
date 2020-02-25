# stable-inplace-sorting-algorithms

These are some new and tested sorting algorithms inspired by quick_sort and merge_sort

Inlcuded are implementations of quick_sort and merge_sort as well as new sorting algorithms stable_quick_sort and merge_sweep_sort.

They have the following characteristics.

| Name | Inplace | Stable | Average Complexity (Big O) | Worst Case Complexity (Big O) | speed sorting 700 random numbers (MSVC compiler in debug) |
| --- | --- | --- | --- | --- | --- |
| quick_sort | Yes | No | O(n log n) | O(n<sup>2</sup>) | 2353 |
| stable_quick_sort | Yes | Yes | O(n<sup>2</sup> log n) | O(n<sup>2</sup>) | 20572 |
| merge_sort | No | Yes | O(n log n) | O(n log n) | 2804 |
| merge_sweep_sort | Yes | Yes | O(n log n) | O(n log n) | 15269 |

This is presented for those looking to study some new sorting techniques and who are interested in sorting algorithms in general.

The idea for stable_quick_sort and merge_sweep_sort came from the idea of using std::rotate as part for a recursive decent sorting algorithm, and so both stable_quick_sort and merge_sweep_sort make use of rotate function.

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
			stdlib::quick_sort(vec.begin(), vec.end());
		}

		std::cout << "[" << std::endl;
		for(uint32_t i = 0; i < 700; ++i) {
			std::cout << "[ " << vec[i] << "], ";
			if(i > 0 && i % 5 == 0)
				std::cout << std::endl;
		}
		std::cout << "]" << std::endl;

		std::cout << "sorted : " << stdlib::is_sorted(vec.begin(), vec.end()) << std::endl;
	}
	{
		std::cout << "test stable quick sort" << std::endl;
		//test stable quick sort
		std::vector<uint32_t> vec;
		for(uint32_t i = 0; i < 700; ++i)
			vec.push_back(rand());

		{
			timer tmr;
			stdlib::stable_quick_sort(vec.begin(), vec.end());
		}

		std::cout << "[" << std::endl;
		for(uint32_t i = 0; i < 700; ++i) {
			std::cout << "[ " << vec[i] << "], ";
			if(i > 0 && i % 5 == 0)
				std::cout << std::endl;
		}
		std::cout << "]" << std::endl;

		std::cout << "sorted : " << stdlib::is_sorted(vec.begin(), vec.end()) << std::endl;
	}
	{
		std::cout << "test merge sort" << std::endl;
		//test merge sort
		std::vector<uint32_t> vec;
		for(uint32_t i = 0; i < 700; ++i)
			vec.push_back(rand());

		{
			timer tmr;
			stdlib::merge_sort(vec.begin(), vec.end());
		}

		std::cout << "[" << std::endl;
		for(uint32_t i = 0; i < 700; ++i) {
			std::cout << "[ " << vec[i] << "], ";
			if(i > 0 && i % 5 == 0)
				std::cout << std::endl;
		}
		std::cout << "]" << std::endl;

		std::cout << "sorted : " << stdlib::is_sorted(vec.begin(), vec.end()) << std::endl;
	}
	{
		std::cout << "test merge sweep sort" << std::endl;
		//test sweep merge sort
		std::vector<uint32_t> vec;
		for(uint32_t i = 0; i < 700; ++i)
			vec.push_back(rand());

		{
			timer tmr;
			stdlib::merge_sweep_sort(vec.begin(), vec.end());
		}

		std::cout << "[" << std::endl;
		for(uint32_t i = 0; i < 700; ++i) {
			std::cout << "[ " << vec[i] << "], ";
			if(i > 0 && i % 5 == 0)
				std::cout << std::endl;
		}
		std::cout << "]" << std::endl;

		std::cout << "sorted : " << stdlib::is_sorted(vec.begin(), vec.end()) << std::endl;
	}

	return 0;
}
```

Please use and let me know what you think.

Thanks

ceorron

aka

Richard Cookman

