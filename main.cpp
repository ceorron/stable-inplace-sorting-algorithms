/*----------------------------------------------------------------------------------*\
 |																					|
 | main.cpp		 																	|
 |																					|
 | Copyright (c) 2020 Richard Cookman												|
 |																					|
 | Permission is hereby granted, free of charge, to any person obtaining a copy		|
 | of this software and associated documentation files (the "Software"), to deal	|
 | in the Software without restriction, including without limitation the rights		|
 | to use, copy, modify, merge, publish, distribute, sublicense, and/or sell		|
 | copies of the Software, and to permit persons to whom the Software is			|
 | furnished to do so, subject to the following conditions:							|
 |																					|
 | The above copyright notice and this permission notice shall be included in all	|
 | copies or substantial portions of the Software.									|
 |																					|
 | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR		|
 | IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,			|
 | FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE		|
 | AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER			|
 | LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,	|
 | OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE	|
 | SOFTWARE.																		|
 |																					|
\*----------------------------------------------------------------------------------*/

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
