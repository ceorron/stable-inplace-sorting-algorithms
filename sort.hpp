/*----------------------------------------------------------------------------------*\
 |																					|
 | sort.hpp		 																	|
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
#pragma once

#include <utility>
#include <stdlib.h>

namespace stlib {

template<typename T>
struct value_for {
    //for iterators
    typedef typename T::value_type value_type;
};

template<typename T>
struct value_for<T*> {
    //for pointers
    typedef T value_type;
};

template<typename T>
void construct(T& lhs, T&& rhs) {
	//just call placement new
	new (&lhs) T(std::move(rhs));
}

template<typename Itr>
void rotate(Itr first, Itr middle, Itr last) {
	if(middle == last)
		return;
	Itr next = middle;
	while(first != next) {
		std::swap(*first++, *next++);
		if(next == last) next = middle;
		else if(first == middle) middle = next;
	}
}


template<typename Itr>
inline Itr half_point(Itr first, Itr last) {
	auto tmp = distance(first, last);
	return first + (tmp/2);
}


template<typename Itr>
void quick_sort(Itr beg, Itr end) {
	if(distance(beg, end) <= 1)
		return;
	Itr left = beg;
	Itr right = end;
	--right;
	Itr rend = right;

	auto pivot = half_point(beg, end);
	while(left != right) {
		//all that are equal go to the left!!
		while(left != pivot && !(*pivot < *left))
			++left;
		while(right != pivot && *pivot < *right)
			--right;
		if(left != right) {
			std::swap(*left, *right);
			if(left == pivot)
				pivot = right;
			else if(right == pivot)
				pivot = left;
		}
	}

	if(beg != left) quick_sort(beg, left);
	if(left != rend) quick_sort(left, end);
}


template<typename Itr, typename Comp>
void quick_sort(Itr beg, Itr end, Comp cmp) {
	if(distance(beg, end) <= 1)
		return;
	Itr left = beg;
	Itr right = end;
	--right;
	Itr rend = right;

	auto pivot = half_point(beg, end);
	while(left != right) {
		//all that are equal go to the left!!
		while(left != pivot && !cmp(*pivot, *left))
			++left;
		while(right != pivot && cmp(*pivot, *right))
			--right;
		if(left != right) {
			std::swap(*left, *right);
			if(left == pivot)
				pivot = right;
			else if(right == pivot)
				pivot = left;
		}
	}

	if(beg != left) quick_sort(beg, left, cmp);
	if(left != rend) quick_sort(left, end, cmp);
}

template<typename Itr>
void sweep_sort(Itr beg, Itr end) {
	if(distance(beg, end) <= 1)
		return;
	auto pivot = half_point(beg, end);

	//record to pivot + 1 for later use
	Itr pivotstore = pivot + 1;

	//go from the pivot to the beginning, find all of the items greater than pivot, move them past the pivot
	size_t greater_count = 0;
	size_t less_count = 0;
	size_t total_less_count = 0;
	{
		Itr it = pivot - 1;
		Itr moveit = pivot + 1;
		while(it != beg - 1) {
			//count in those less (or equal) to pivot
			while(it != beg - 1 && !(*pivot < *it))
				--it;
			if(it != beg - 1) {
				--it;
				++greater_count;
			}
			//count in those greater than pivot
			while(it != beg - 1 && *pivot < *it) {
				--it;
				++greater_count;
			}

			//do move
			rotate(it + 1, it + (1 + greater_count), moveit);
			moveit -= greater_count;
			pivot -= greater_count;

			greater_count = 0;
			if(it != beg - 1)
				--it;
		}
	}

	//go from the old pivot to the end, find all of the items less than pivot, move them after the pivot
	{
		Itr it = pivotstore;
		while(it != end) {
			//count in those greater (or equal) to pivot
			while(it != end && !(*it < *pivot))
				++it;
			if(it != end) {
				++it;
				++less_count;
				++total_less_count;
			}
			//count in those less than pivot
			while(it != end && *it < *pivot) {
				++it;
				++less_count;
				++total_less_count;
			}

			//do move
			rotate(pivotstore, it - less_count, it);
			pivotstore += less_count;

			less_count = 0;
			if(it != end)
				++it;
		}
	}

	//do a final rotate to reorder pivot
	rotate(pivot, pivotstore - total_less_count, pivotstore);
	pivot += distance(pivotstore - total_less_count, pivotstore);

	sweep_sort(beg, pivot);
	sweep_sort(pivot + 1, end);
}


template<typename Itr, typename Comp>
void sweep_sort(Itr beg, Itr end, Comp cmp) {
	if(distance(beg, end) <= 1)
		return;
	auto pivot = half_point(beg, end);

	//record to pivot + 1 for later use
	Itr pivotstore = pivot + 1;

	//go from the pivot to the beginning, find all of the items greater than pivot, move them past the pivot
	size_t greater_count = 0;
	size_t less_count = 0;
	size_t total_less_count = 0;
	{
		Itr it = pivot - 1;
		Itr moveit = pivot + 1;
		while(it != beg - 1) {
			//count in those less (or equal) to pivot
			while(it != beg - 1 && !cmp(*pivot, *it))
				--it;
			if(it != beg - 1) {
				--it;
				++greater_count;
			}
			//count in those greater than pivot
			while(it != beg - 1 && cmp(*pivot, *it)) {
				--it;
				++greater_count;
			}

			//do move
			rotate(it + 1, it + (1 + greater_count), moveit);
			moveit -= greater_count;
			pivot -= greater_count;

			greater_count = 0;
			if(it != beg - 1)
				--it;
		}
	}

	//go from the old pivot to the end, find all of the items less than pivot, move them after the pivot
	{
		Itr it = pivotstore;
		while(it != end) {
			//count in those greater (or equal) to pivot
			while(it != end && !cmp(*it, *pivot))
				++it;
			if(it != end) {
				++it;
				++less_count;
				++total_less_count;
			}
			//count in those less than pivot
			while(it != end && cmp(*it, *pivot)) {
				++it;
				++less_count;
				++total_less_count;
			}

			//do move
			rotate(pivotstore, it - less_count, it);
			pivotstore += less_count;

			less_count = 0;
			if(it != end)
				++it;
		}
	}

	//do a final rotate to reorder pivot
	rotate(pivot, pivotstore - total_less_count, pivotstore);
	pivot += distance(pivotstore - total_less_count, pivotstore);

	sweep_sort(beg, pivot, cmp);
	sweep_sort(pivot + 1, end, cmp);
}


namespace stlib_internal {
template<typename Itr>
void merge_sweep_sort_recurse(Itr& pivot, Itr beg, Itr end, Itr& nhalf) {
	if(distance(beg, end) == 0) {
		nhalf = beg;
		return;
	}
	if(distance(beg, end) == 1) {
		nhalf = beg;
		if(*nhalf < *pivot)
			++nhalf;
		return;
	}

	auto half = half_point(beg, end);

	Itr nhalf1;
	merge_sweep_sort_recurse(pivot, beg, half, nhalf1);
	Itr nhalf2;
	merge_sweep_sort_recurse(pivot, half, end, nhalf2);

	//do rotate to combine the halfs
	//swap the greater of the first with the less of the second
	rotate(nhalf1, half, nhalf2);
	//keep track of pivot, pivot moves lesser up
	if(pivot >= nhalf1 && pivot < half)
		pivot += distance(half, nhalf2);

	//the new half point is all of the lesser
	nhalf = nhalf1 + distance(half, nhalf2);
}
}
template<typename Itr>
void merge_sweep_sort(Itr beg, Itr end) {
	if(distance(beg, end) <= 1)
		return;
	auto pivot = half_point(beg, end);

	Itr nhalf;
	stlib_internal::merge_sweep_sort_recurse(pivot, beg, end, nhalf);

	size_t greater_count = 0;
	{
		Itr it = pivot - 1;
		Itr moveit = pivot + 1;
		while(it != nhalf - 1) {
			//count in those less (or equal) to pivot
			while(it != nhalf - 1 && !(*pivot < *it))
				--it;
			if(it != nhalf - 1) {
				--it;
				++greater_count;
			}
			//count in those greater than pivot
			while(it != nhalf - 1 && *pivot < *it) {
				--it;
				++greater_count;
			}

			//do move
			rotate(it + 1, it + (1 + greater_count), moveit);
			moveit -= greater_count;
			pivot -= greater_count;

			greater_count = 0;
			if(it != nhalf - 1)
				--it;
		}
	}

	merge_sweep_sort(beg, nhalf);
	merge_sweep_sort(pivot + 1, end);
}
namespace stlib_internal {
template<typename Itr, typename Comp>
void merge_sweep_sort_recurse(Itr& pivot, Itr beg, Itr end, Itr& nhalf, Comp cmp) {
	if(distance(beg, end) == 0) {
		nhalf = beg;
		return;
	}
	if(distance(beg, end) == 1) {
		nhalf = beg;
		if(cmp(*nhalf, *pivot))
			++nhalf;
		return;
	}

	auto half = half_point(beg, end);

	Itr nhalf1;
	merge_sweep_sort_recurse(pivot, beg, half, nhalf1, cmp);
	Itr nhalf2;
	merge_sweep_sort_recurse(pivot, half, end, nhalf2, cmp);

	//do rotate to combine the halfs
	//swap the greater of the first with the less of the second
	rotate(nhalf1, half, nhalf2);
	//keep track of pivot, pivot moves lesser up
	if(pivot >= nhalf1 && pivot < half)
		pivot += distance(half, nhalf2);

	//the new half point is all of the lesser
	nhalf = nhalf1 + distance(half, nhalf2);
}
}
template<typename Itr, typename Comp>
void merge_sweep_sort(Itr beg, Itr end, Comp cmp) {
	if(distance(beg, end) <= 1)
		return;
	auto pivot = half_point(beg, end);

	Itr nhalf;
	stlib_internal::merge_sweep_sort_recurse(pivot, beg, end, nhalf, cmp);

	size_t greater_count = 0;
	{
		Itr it = pivot - 1;
		Itr moveit = pivot + 1;
		while(it != nhalf - 1) {
			//count in those less (or equal) to pivot
			while(it != nhalf - 1 && !cmp(*pivot, *it))
				--it;
			if(it != nhalf - 1) {
				--it;
				++greater_count;
			}
			//count in those greater than pivot
			while(it != nhalf - 1 && cmp(*pivot, *it)) {
				--it;
				++greater_count;
			}

			//do move
			rotate(it + 1, it + (1 + greater_count), moveit);
			moveit -= greater_count;
			pivot -= greater_count;

			greater_count = 0;
			if(it != nhalf - 1)
				--it;
		}
	}

	merge_sweep_sort(beg, nhalf, cmp);
	merge_sweep_sort(pivot + 1, end, cmp);
}


namespace stlib_internal {
template<typename Itr>
void merge_internal(Itr beg1, Itr end1, Itr beg2, Itr end2, char* buf) {
	Itr begrng = beg1;
	typename stlib::value_for<Itr>::value_type* tmp = (typename stlib::value_for<Itr>::value_type*)buf;
	typename stlib::value_for<Itr>::value_type* tmpbeg = tmp;

	//go through both lists, build the sorted list
	for(; beg1 != end1 && beg2 != end2; ++tmp) {
		if(*beg2 < *beg1) {
			construct(*tmp, std::move(*beg2));
			++beg2;
		} else {
			construct(*tmp, std::move(*beg1));
			++beg1;
		}
	}

	Itr begrng2 = beg1 + (distance(beg1, end2) - distance(beg1, end1));
	//finish off copy back of lists
	for(; beg1 != end1; ++begrng2, ++beg1)
		construct(*begrng2, std::move(*beg1));

	//copy the new list back
	for(; tmpbeg != tmp; ++tmpbeg, ++begrng)
		construct(*begrng, std::move(*tmpbeg));
	return;
}
template<typename Itr>
void merge_sort_internal(Itr beg, Itr end, char* buf) {
	auto sze = distance(beg, end);
	if(sze <= 1)
		return;

	sze /= 2;
	merge_sort_internal(beg, beg + sze, buf);
	merge_sort_internal(beg + sze, end, buf);

	merge_internal(beg, beg + sze, beg + sze, end, buf);
}
}
template<typename Itr>
bool merge_sort(Itr beg, Itr end) {
	if(distance(beg, end) <= 1)
		return true;
	using valueof = typename stlib::value_for<Itr>::value_type;
	char* buf = (char*)malloc(distance(beg, end) * sizeof(valueof));
	if(buf) {
		stlib_internal::merge_sort_internal(beg, end, buf);

		free(buf);
		return true;
	}
	return false;
}


namespace stlib_internal {
template<typename Itr, typename Comp>
void merge_internal(Itr beg1, Itr end1, Itr beg2, Itr end2, char* buf, Comp cmp) {
	Itr begrng = beg1;
	typename stlib::value_for<Itr>::value_type* tmp = (typename stlib::value_for<Itr>::value_type*)buf;
	typename stlib::value_for<Itr>::value_type* tmpbeg = tmp;

	//go through both lists, build the sorted list
	for(; beg1 != end1 && beg2 != end2; ++tmp) {
		if(cmp(*beg2, *beg1)) {
			construct(*tmp, std::move(*beg2));
			++beg2;
		} else {
			construct(*tmp, std::move(*beg1));
			++beg1;
		}
	}

	Itr begrng2 = beg1 + (distance(beg1, end2) - distance(beg1, end1));
	//finish off copy back of lists
	for(; beg1 != end1; ++begrng2, ++beg1)
		construct(*begrng2, std::move(*beg1));

	//copy the new list back
	for(; tmpbeg != tmp; ++tmpbeg, ++begrng)
		construct(*begrng, std::move(*tmpbeg));
	return;
}
template<typename Itr, typename Comp>
void merge_sort_internal(Itr beg, Itr end, char* buf, Comp cmp) {
	auto sze = distance(beg, end);
	if(sze <= 1)
		return;

	sze /= 2;
	merge_sort_internal(beg, beg + sze, buf, cmp);
	merge_sort_internal(beg + sze, end, buf, cmp);

	merge_internal(beg, beg + sze, beg + sze, end, buf, cmp);
}
}
template<typename Itr, typename Comp>
bool merge_sort(Itr beg, Itr end, Comp cmp) {
	if(distance(beg, end) <= 1)
		return true;
	using valueof = typename stlib::value_for<Itr>::value_type;
	char* buf = (char*)malloc(distance(beg, end) * sizeof(valueof));
	if(buf) {
		stlib_internal::merge_sort_internal(beg, end, buf, cmp);

		free(buf);
		return true;
	}
	return false;
}

template<typename Itr>
inline void stable_sort(Itr beg, Itr end) {
	merge_sort(beg, end);
}
template<typename Itr, typename Comp>
inline void stable_sort(Itr beg, Itr end, Comp cmp) {
	merge_sort(beg, end, cmp);
}
template<typename Itr>
inline void sort(Itr beg, Itr end) {
	quick_sort(beg, end);
}
template<typename Itr, typename Comp>
inline void sort(Itr beg, Itr end, Comp cmp) {
	quick_sort(beg, end, cmp);
}

template<typename Itr>
bool is_sorted(Itr beg, Itr end) {
	for(; beg != end - 1; ++beg)
		if(*(beg + 1) < *beg)
			return false;
	return true;
}
template<typename Itr>
bool is_reverse_sorted(Itr beg, Itr end) {
	for(; beg != end - 1; ++beg)
		if(*(beg + 1) > *beg)
			return false;
	return true;
}

}
