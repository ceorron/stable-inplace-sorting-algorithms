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
	size_t count = 700;
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
        std::cout << "test stable quick sort" << std::endl;
        //test stable quick sort
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
            stlib::hybrid_inplace_merge_sort(vec.begin(), vec.end());
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
            stlib::hybrid_rotate_merge_sort(vec.begin(), vec.end());
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
