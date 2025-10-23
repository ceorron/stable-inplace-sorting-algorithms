/*----------------------------------------------------------------------------------*\
 |																					|
 | sort.hpp																			|
 |																					|
 | Copyright (c) 2020-2025 Richard Cookman											|
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
#include <stdint.h>
#include <vector>
#include <algorithm>

namespace stlib {

constexpr int INSERTION_SORT_CUTOFF = 32;

constexpr int NEW_ZIP_SORT_ARRAY_SIZE = 64;
constexpr int NEW_ZIP_SORT_INDEX_ARRAY_SIZE = 7000;

//general distance function for pointers
template<typename U>
inline ptrdiff_t distance(U* first, U* last) {
	return last - first;
}

template<typename Itr>
void rotate_merge_sort(Itr beg, Itr end);
template<typename Itr, typename Comp>
void rotate_merge_sort(Itr beg, Itr end, Comp cmp);

namespace stlib_internal {

template<typename Itr, typename IdxItr>
void stable_rotate_merge_sort(Itr strt, Itr beg, Itr end, IdxItr begidx);
template<typename Itr, typename IdxItr, typename Comp>
void stable_rotate_merge_sort(Itr strt, Itr beg, Itr end, IdxItr begidx, Comp cmp);
template<typename Itr, typename IdxItr>
void stable_quick_sort_swap(Itr beg, Itr left, Itr right, IdxItr begidx);

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
	//call placement new
	new (&lhs) T(std::move(rhs));
}
template<typename T>
void construct(T& lhs, T& rhs) {
	//call placement new
	new (&lhs) T(rhs);
}
template<typename T>
void destruct(T& val) {
	//call destructor
	val.~T();
}

//some function abstractions, only use less than operator
template<typename T>
inline bool equal_func(const T& lhs, const T& rhs) {
	return !(lhs < rhs) && !(rhs < lhs);
}
template<typename T>
inline bool less_func(const T& lhs, const T& rhs) {
	return lhs < rhs;
}
template<typename T>
inline bool greater_func(const T& lhs, const T& rhs) {
	return rhs < lhs;
}
template<typename T>
inline bool less_equal_func(const T& lhs, const T& rhs) {
	return !(rhs < lhs);
}
template<typename T>
inline bool greater_equal_func(const T& lhs, const T& rhs) {
	return !(lhs < rhs);
}
template<typename T, typename Comp>
inline bool equal_func(const T& lhs, const T& rhs, Comp cmp) {
	return !cmp(lhs, rhs) && !cmp(rhs, lhs);
}
template<typename T, typename Comp>
inline bool less_func(const T& lhs, const T& rhs, Comp cmp) {
	return cmp(lhs, rhs);
}
template<typename T, typename Comp>
inline bool greater_func(const T& lhs, const T& rhs, Comp cmp) {
	return cmp(rhs, lhs);
}
template<typename T, typename Comp>
inline bool less_equal_func(const T& lhs, const T& rhs, Comp cmp) {
	return !cmp(rhs, lhs);
}
template<typename T, typename Comp>
inline bool greater_equal_func(const T& lhs, const T& rhs, Comp cmp) {
	return !cmp(lhs, rhs);
}

bool equal_func_bool(const bool less, const bool greater);
bool less_func_bool(const bool less);
bool greater_func_bool(const bool greater);
bool less_equal_func_bool(const bool greater);
bool greater_equal_func_bool(const bool less);

void* aligned_storage_new(size_t sze);
void aligned_storage_delete(size_t sze, void* ptr);

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
template<typename Itr, typename IdxItr>
void stable_rotate(Itr strt, Itr first, Itr middle, Itr last, IdxItr begidx) {
	if(middle == last)
		return;
	Itr next = middle;
	while(first != next) {
		//swap
		stable_quick_sort_swap(strt, first, next, begidx);
		++first;
		++next;
		if(next == last) next = middle;
		else if(first == middle) middle = next;
	}
}
template<typename Itr>
void reverse(Itr first, Itr last) {
	while(first != last && first != --last) {
		std::swap(*first, *last);
		++first;
	}
}
template<typename Itr1, typename Itr2>
void copy_buffers(Itr1 beg, Itr1 end, Itr2& out) {
	//found to be faster then memcpy!!!
	for(; beg != end; ++beg, ++out)
		construct(*out, std::move(*beg));
}


template<typename Itr, typename IdxItr>
void stable_quick_sort_swap(Itr beg, Itr left, Itr right, IdxItr begidx) {
	size_t lidx = distance(beg, left);
	size_t ridx = distance(beg, right);

	std::swap(*left, *right);
	std::swap(*(begidx + lidx), *(begidx + ridx));
}
template<typename Itr, typename IdxItr>
bool stable_quick_sort_equal_func(Itr beg, Itr left, Itr right, IdxItr begidx) {
	//items are equal if same item
	if(!equal_func(*left, *right))
		return false;
	size_t lidx = distance(beg, left);
	size_t ridx = distance(beg, right);
	return lidx == ridx;
}
template<typename Itr, typename IdxItr>
bool stable_quick_sort_not_equal_func(Itr beg, Itr left, Itr right, IdxItr begidx) {
	return !stable_quick_sort_equal_func(beg, left, right, begidx);
}
template<typename Itr, typename IdxItr>
bool stable_quick_sort_less_func(Itr beg, Itr left, Itr right, IdxItr begidx) {
	if(less_func(*left, *right))
		return true;
	if(greater_func(*left, *right))
		return false;
	size_t lidx = distance(beg, left);
	size_t ridx = distance(beg, right);
	return *(begidx + lidx) < *(begidx + ridx);
}
template<typename Itr, typename IdxItr>
bool stable_quick_sort_greater_func(Itr beg, Itr left, Itr right, IdxItr begidx) {
	if(less_func(*left, *right))
		return false;
	if(greater_func(*left, *right))
		return true;
	size_t lidx = distance(beg, left);
	size_t ridx = distance(beg, right);
	return *(begidx + lidx) > *(begidx + ridx);
}
template<typename Itr, typename IdxItr>
bool stable_quick_sort_less_equal_func(Itr beg, Itr left, Itr right, IdxItr begidx) {
	if(less_func(*left, *right))
		return true;
	if(greater_func(*left, *right))
		return false;
	//never equal
	size_t lidx = distance(beg, left);
	size_t ridx = distance(beg, right);
	return *(begidx + lidx) <= *(begidx + ridx);
}
template<typename Itr, typename IdxItr>
bool stable_quick_sort_greater_equal_func(Itr beg, Itr left, Itr right, IdxItr begidx) {
	if(less_func(*left, *right))
		return false;
	if(greater_func(*left, *right))
		return true;
	//never equal
	size_t lidx = distance(beg, left);
	size_t ridx = distance(beg, right);
	return *(begidx + lidx) >= *(begidx + ridx);
}
template<typename Itr, typename IdxItr>
bool stable_quick_sort_is_sorted(Itr start, Itr beg, Itr end, IdxItr begidx) {
	for(; beg != end - 1; ++beg)
		if(stable_quick_sort_less_func(start, (beg + 1), beg, begidx))
			return false;
	return true;
}
template<typename Itr, typename IdxItr>
bool stable_quick_sort_is_reverse_sorted(Itr start, Itr beg, Itr end, IdxItr begidx) {
	for(; beg != end - 1; ++beg)
		if(stable_quick_sort_greater_func(start, (beg + 1), beg, begidx))
			return false;
	return true;
}
template<typename Itr, typename IdxItr, typename Comp>
bool stable_quick_sort_equal_func(Itr beg, Itr left, Itr right, IdxItr begidx, Comp cmp) {
	//items are equal if same item
	if(!equal_func(*left, *right, cmp))
		return false;
	size_t lidx = distance(beg, left);
	size_t ridx = distance(beg, right);
	return lidx == ridx;
}
template<typename Itr, typename IdxItr, typename Comp>
bool stable_quick_sort_not_equal_func(Itr beg, Itr left, Itr right, IdxItr begidx, Comp cmp) {
	return !stable_quick_sort_equal_func(beg, left, right, begidx, cmp);
}
template<typename Itr, typename IdxItr, typename Comp>
bool stable_quick_sort_less_func(Itr beg, Itr left, Itr right, IdxItr begidx, Comp cmp) {
	if(less_func(*left, *right, cmp))
		return true;
	if(greater_func(*left, *right, cmp))
		return false;
	size_t lidx = distance(beg, left);
	size_t ridx = distance(beg, right);
	return *(begidx + lidx) < *(begidx + ridx);
}
template<typename Itr, typename IdxItr, typename Comp>
bool stable_quick_sort_greater_func(Itr beg, Itr left, Itr right, IdxItr begidx, Comp cmp) {
	if(less_func(*left, *right, cmp))
		return false;
	if(greater_func(*left, *right, cmp))
		return true;
	size_t lidx = distance(beg, left);
	size_t ridx = distance(beg, right);
	return *(begidx + lidx) > *(begidx + ridx);
}
template<typename Itr, typename IdxItr, typename Comp>
bool stable_quick_sort_less_equal_func(Itr beg, Itr left, Itr right, IdxItr begidx, Comp cmp) {
	if(less_func(*left, *right, cmp))
		return true;
	if(greater_func(*left, *right, cmp))
		return false;
	//never equal
	size_t lidx = distance(beg, left);
	size_t ridx = distance(beg, right);
	return *(begidx + lidx) <= *(begidx + ridx);
}
template<typename Itr, typename IdxItr, typename Comp>
bool stable_quick_sort_greater_equal_func(Itr beg, Itr left, Itr right, IdxItr begidx, Comp cmp) {
	if(less_func(*left, *right, cmp))
		return false;
	if(greater_func(*left, *right, cmp))
		return true;
	//never equal
	size_t lidx = distance(beg, left);
	size_t ridx = distance(beg, right);
	return *(begidx + lidx) >= *(begidx + ridx);
}
template<typename Itr, typename IdxItr, typename Comp>
bool stable_quick_sort_is_sorted(Itr start, Itr beg, Itr end, IdxItr begidx, Comp cmp) {
	for(; beg != end - 1; ++beg)
		if(stable_quick_sort_less_func(start, (beg + 1), beg, begidx, cmp))
			return false;
	return true;
}
template<typename Itr, typename IdxItr, typename Comp>
bool stable_quick_sort_is_reverse_sorted(Itr start, Itr beg, Itr end, IdxItr begidx, Comp cmp) {
	for(; beg != end - 1; ++beg)
		if(stable_quick_sort_greater_func(start, (beg + 1), beg, begidx, cmp))
			return false;
	return true;
}

template<typename Itr, typename IdxItr>
void stable_insertion_sort(Itr beg, Itr end, IdxItr begidx) {
	if(distance(beg, end) <= 1)
		return;
	Itr strt = beg + 1;
	for(; strt != end; ++strt) {
		Itr crnt = strt;

		//move this to the correct place (do insert)
		while(crnt != beg && stable_quick_sort_greater_func(beg, crnt - 1, crnt, begidx)) {
			//swap if in wrong order
			stable_quick_sort_swap(beg, crnt, crnt - 1, begidx);
			--crnt;
		}
	}
}
template<typename Itr, typename IdxItr, typename Comp>
void stable_insertion_sort(Itr beg, Itr end, IdxItr begidx, Comp cmp) {
	if(distance(beg, end) <= 1)
		return;
	Itr strt = beg + 1;
	for(; strt != end; ++strt) {
		Itr crnt = strt;

		//move this to the correct place (do insert)
		while(crnt != beg && stable_quick_sort_greater_func(beg, crnt - 1, crnt, begidx, cmp)) {
			//swap if in wrong order
			stable_quick_sort_swap(beg, crnt, crnt - 1, begidx);
			--crnt;
		}
	}
}

}

template<typename Itr>
bool is_sorted(Itr beg, Itr end) {
	for(; beg != end - 1; ++beg)
		if(stlib_internal::less_func(*(beg + 1), *beg))
			return false;
	return true;
}
template<typename Itr, typename Comp>
bool is_sorted(Itr beg, Itr end, Comp cmp) {
	for(; beg != end - 1; ++beg)
		if(stlib_internal::less_func(*(beg + 1), *beg, cmp))
			return false;
	return true;
}
template<typename Itr>
bool is_reverse_sorted(Itr beg, Itr end) {
	for(; beg != end - 1; ++beg)
		if(stlib_internal::greater_func(*(beg + 1), *beg))
			return false;
	return true;
}
template<typename Itr, typename Comp>
bool is_reverse_sorted(Itr beg, Itr end, Comp cmp) {
	for(; beg != end - 1; ++beg)
		if(stlib_internal::greater_func(*(beg + 1), *beg, cmp))
			return false;
	return true;
}


namespace stlib_internal {
template<typename Itr>
struct stack_less_data {
	Itr beg;
	Itr end;
};
template<typename Itr>
void add_stack_item(Itr beg1, Itr end1,
					std::vector<stack_less_data<Itr>>& stk, size_t& idx) {
	stack_less_data<Itr> dat = {
		beg1,
		end1
	};
	stk[idx++] = std::move(dat);
	if(idx == stk.size())
		stk.resize(stk.size() * 2);
}
template<typename Itr>
struct intro_stack_less_data {
	Itr beg;
	Itr end;
	unsigned depth;
};
template<typename Itr>
void add_stack_item(Itr beg1, Itr end1, Itr beg2, Itr end2, unsigned depth,
					std::vector<intro_stack_less_data<Itr>>& stk, size_t& idx) {
	if(depth == 1) {
		//do O(n log n) rotate merge if we have reached the maximum depth
		rotate_merge_sort(beg1, end1);
	} else {
		intro_stack_less_data<Itr> dat = {
			beg2,
			end2,
			depth - 1
		};
		stk[idx++] = std::move(dat);
		if(idx == stk.size())
			stk.resize(stk.size() * 2);
	}
}
template<typename Itr, typename Comp>
void add_stack_item(Itr beg1, Itr end1, Itr beg2, Itr end2, unsigned depth,
					std::vector<intro_stack_less_data<Itr>>& stk, size_t& idx, Comp cmp) {
	if(depth == 1) {
		//do O(n log n) rotate merge if we have reached the maximum depth
		rotate_merge_sort(beg1, end1, cmp);
	} else {
		intro_stack_less_data<Itr> dat = {
			beg2,
			end2,
			depth - 1
		};
		stk[idx++] = std::move(dat);
		if(idx == stk.size())
			stk.resize(stk.size() * 2);
	}
}
template<typename Itr, typename IdxItr>
void stable_add_stack_item(Itr beg, Itr beg1, Itr end1, Itr beg2, Itr end2, IdxItr begidx, unsigned depth,
						   std::vector<intro_stack_less_data<Itr>>& stk, size_t& idx) {
	if(depth == 1) {
		//do O(n log n) inplace rotate merge sort if we have reached the maximum depth
		stable_rotate_merge_sort(beg, beg1, end1, begidx);
	} else {
		intro_stack_less_data<Itr> dat = {
			beg2,
			end2,
			depth - 1
		};
		stk[idx++] = std::move(dat);
		if(idx == stk.size())
			stk.resize(stk.size() * 2);
	}
}
template<typename Itr, typename IdxItr, typename Comp>
void stable_add_stack_item(Itr beg, Itr beg1, Itr end1, Itr beg2, Itr end2, IdxItr begidx, unsigned depth,
						   std::vector<intro_stack_less_data<Itr>>& stk, size_t& idx, Comp cmp) {
	if(depth == 1) {
		//do O(n log n) inplace rotate merge sort if we have reached the maximum depth
		stable_rotate_merge_sort(beg, beg1, end1, begidx, cmp);
	} else {
		intro_stack_less_data<Itr> dat = {
			beg2,
			end2,
			depth - 1
		};
		stk[idx++] = std::move(dat);
		if(idx == stk.size())
			stk.resize(stk.size() * 2);
	}
}
template<typename Num>
unsigned get_depth(Num n) {
	//always have atleast a depth of 3
	unsigned depth = 1;
	while(n > 1) {
		n /= 2;
		++depth;
	}
	return depth * 2 + 1;
}

template<typename Itr>
inline Itr half_point(Itr first, Itr last) {
	auto tmp = distance(first, last);
	return first + (tmp/2);
}
template<typename Itr>
Itr middle_of_three(Itr first, Itr middle, Itr last) {
	if(less_func(*middle, *first)) {
		if(less_func(*last, *middle))
			return middle;
		else if(less_func(*last, *first))
			return last;
		return first;
	} else {
		if(less_func(*middle, *last))
			return middle;
		else if(less_func(*last, *first))
			return first;
		return last;
	}
}
template<typename Itr, typename Comp>
Itr middle_of_three(Itr first, Itr middle, Itr last, Comp cmp) {
	if(less_func(*middle, *first, cmp)) {
		if(less_func(*last, *middle, cmp))
			return middle;
		else if(less_func(*last, *first, cmp))
			return last;
		return first;
	} else {
		if(less_func(*middle, *last, cmp))
			return middle;
		else if(less_func(*last, *first, cmp))
			return first;
		return last;
	}
}
template<typename Itr>
Itr middle_of_four(Itr first, Itr middle, Itr last) {
	//do upfront comparison
	bool first_middle = less_func(*first, *middle);
	bool middle_first = less_func(*middle, *first);

	bool middle_last = less_func(*middle, *last);
	bool last_middle = less_func(*last, *middle);

	bool last_first = less_func(*last, *first);
	bool first_last = less_func(*first, *last);

	//if two of them are the same then choose the other one
	if(equal_func_bool(first_middle, middle_first) & !equal_func_bool(middle_last, last_middle))
		return last;
	if(equal_func_bool(middle_last, last_middle) & !equal_func_bool(first_middle, middle_first))
		return first;
	if(equal_func_bool(last_first, first_last) & !equal_func_bool(middle_last, last_middle))
		return middle;

	//choose the middle one
	if(less_func_bool(middle_first)) {
		if(less_func_bool(last_middle))
			return middle;
		else if(less_func_bool(last_first))
			return last;
		return first;
	} else {
		if(less_func_bool(middle_last))
			return middle;
		else if(less_func_bool(last_first))
			return first;
		return last;
	}
}
template<typename Itr, typename Comp>
Itr middle_of_four(Itr first, Itr middle, Itr last, Comp cmp) {
	//do upfront comparison
	bool first_middle = less_func(*first, *middle, cmp);
	bool middle_first = less_func(*middle, *first, cmp);

	bool middle_last = less_func(*middle, *last, cmp);
	bool last_middle = less_func(*last, *middle, cmp);

	bool last_first = less_func(*last, *first, cmp);
	bool first_last = less_func(*first, *last, cmp);

	//if two of them are the same then choose the other one
	if(equal_func_bool(first_middle, middle_first) & !equal_func_bool(middle_last, last_middle))
		return last;
	if(equal_func_bool(middle_last, last_middle) & !equal_func_bool(first_middle, middle_first))
		return first;
	if(equal_func_bool(last_first, first_last) & !equal_func_bool(middle_last, last_middle))
		return middle;

	//choose the middle one
	if(less_func_bool(middle_first)) {
		if(less_func_bool(last_middle))
			return middle;
		else if(less_func_bool(last_first))
			return last;
		return first;
	} else {
		if(less_func_bool(middle_last))
			return middle;
		else if(less_func_bool(last_first))
			return first;
		return last;
	}
}
template<typename Itr, typename IdxItr>
Itr stable_middle_of_four(Itr beg, Itr first, Itr middle, Itr last, IdxItr begidx) {
	//do upfront comparison
	bool first_middle = stable_quick_sort_less_func(beg, first, middle, begidx);
	bool middle_first = stable_quick_sort_less_func(beg, middle, first, begidx);

	bool middle_last = stable_quick_sort_less_func(beg, middle, last, begidx);
	bool last_middle = stable_quick_sort_less_func(beg, last, middle, begidx);

	bool last_first = stable_quick_sort_less_func(beg, last, first, begidx);
	bool first_last = stable_quick_sort_less_func(beg, first, last, begidx);

	//if two of them are the same then choose the other one
	if(equal_func_bool(first_middle, middle_first) & !equal_func_bool(middle_last, last_middle))
		return last;
	if(equal_func_bool(middle_last, last_middle) & !equal_func_bool(first_middle, middle_first))
		return first;
	if(equal_func_bool(last_first, first_last) & !equal_func_bool(middle_last, last_middle))
		return middle;

	//choose the middle one
	if(less_func_bool(middle_first)) {
		if(less_func_bool(last_middle))
			return middle;
		else if(less_func_bool(last_first))
			return last;
		return first;
	} else {
		if(less_func_bool(middle_last))
			return middle;
		else if(less_func_bool(last_first))
			return first;
		return last;
	}
}
template<typename Itr, typename IdxItr, typename Comp>
Itr stable_middle_of_four(Itr beg, Itr first, Itr middle, Itr last, IdxItr begidx, Comp cmp) {
	//do upfront comparison
	bool first_middle = stable_quick_sort_less_func(beg, first, middle, begidx, cmp);
	bool middle_first = stable_quick_sort_less_func(beg, middle, first, begidx, cmp);

	bool middle_last = stable_quick_sort_less_func(beg, middle, last, begidx, cmp);
	bool last_middle = stable_quick_sort_less_func(beg, last, middle, begidx, cmp);

	bool last_first = stable_quick_sort_less_func(beg, last, first, begidx, cmp);
	bool first_last = stable_quick_sort_less_func(beg, first, last, begidx, cmp);

	//if two of them are the same then choose the other one
	if(equal_func_bool(first_middle, middle_first) & !equal_func_bool(middle_last, last_middle))
		return last;
	if(equal_func_bool(middle_last, last_middle) & !equal_func_bool(first_middle, middle_first))
		return first;
	if(equal_func_bool(last_first, first_last) & !equal_func_bool(middle_last, last_middle))
		return middle;

	//choose the middle one
	if(less_func_bool(middle_first)) {
		if(less_func_bool(last_middle))
			return middle;
		else if(less_func_bool(last_first))
			return last;
		return first;
	} else {
		if(less_func_bool(middle_last))
			return middle;
		else if(less_func_bool(last_first))
			return first;
		return last;
	}
}

template<typename Itr>
void move_pivot(Itr nhalf, Itr& pivot) {
	//move any that are greater than the pivot to the right of the pivot
	size_t greater_count = 0;
	{
		Itr it = pivot - 1;
		Itr moveit = pivot + 1;
		while(it != nhalf - 1) {
			//count in those less (or equal) to pivot
			while(it != nhalf - 1 && less_equal_func(*it, *pivot))
				--it;
			if(it != nhalf - 1) {
				--it;
				++greater_count;
			}
			//count in those greater than pivot
			while(it != nhalf - 1 && greater_func(*it, *pivot)) {
				--it;
				++greater_count;
			}

			//do move
			stlib_internal::rotate(it + 1, it + (1 + greater_count), moveit);
			moveit -= greater_count;
			pivot -= greater_count;

			greater_count = 0;
			if(it != nhalf - 1)
				--it;
		}
	}
}
template<typename Itr, typename Comp>
void move_pivot(Itr nhalf, Itr& pivot, Comp cmp) {
	//move any that are greater than the pivot to the right of the pivot
	size_t greater_count = 0;
	{
		Itr it = pivot - 1;
		Itr moveit = pivot + 1;
		while(it != nhalf - 1) {
			//count in those less (or equal) to pivot
			while(it != nhalf - 1 && less_equal_func(*it, *pivot, cmp))
				--it;
			if(it != nhalf - 1) {
				--it;
				++greater_count;
			}
			//count in those greater than pivot
			while(it != nhalf - 1 && greater_func(*it, *pivot, cmp)) {
				--it;
				++greater_count;
			}

			//do move
			stlib_internal::rotate(it + 1, it + (1 + greater_count), moveit);
			moveit -= greater_count;
			pivot -= greater_count;

			greater_count = 0;
			if(it != nhalf - 1)
				--it;
		}
	}
}
}

//basic binary search
template<typename Itr, typename T>
bool binary_search(Itr beg, Itr end, const T& item,
				   Itr& out) {
	//binary search return the insertion point, in both the found and not found case
	ptrdiff_t sze = distance(beg, end);
	if(sze == 0) {
		out = end;
		return false;
	}

	out = beg;
	ptrdiff_t step = 0;
	ptrdiff_t count = sze;

	//returns first element greater than or equal to the element
	while(count > 0) {
		auto it = out;
		step = count / 2;
		it += step;
		if(stlib_internal::less_func(*it, item)) {
			out = ++it;
			count -= step + 1;
		} else
			count = step;
	}
	return out != end && stlib_internal::greater_equal_func(*out, item) && stlib_internal::less_equal_func(*out, item);
}
template<typename Itr, typename T, typename Less>
bool binary_search(Itr beg, Itr end, const T& item,
				   Less comp, Itr& out) {
	//binary search return the insertion point, in both the found and not found case
	ptrdiff_t sze = distance(beg, end);
	if(sze == 0) {
		out = end;
		return false;
	}

	out = beg;
	ptrdiff_t step = 0;
	ptrdiff_t count = sze;

	//returns first element greater than or equal to the element
	while(count > 0) {
		auto it = out;
		step = count / 2;
		it += step;
		if(stlib_internal::less_func(*it, item, comp)) {
			out = ++it;
			count -= step + 1;
		} else
			count = step;
	}
	return out != end && stlib_internal::greater_equal_func(*out, item, comp) && stlib_internal::less_equal_func(*out, item, comp);
}
namespace stlib_internal {
template<typename Itr, typename IdxItr>
bool stable_binary_search(Itr strt, Itr beg, Itr end, Itr item, IdxItr begidx,
						  Itr& out) {
	//binary search return the insertion point, in both the found and not found case
	ptrdiff_t sze = distance(beg, end);
	if(sze == 0) {
		out = end;
		return false;
	}

	out = beg;
	ptrdiff_t step = 0;
	ptrdiff_t count = sze;

	//returns first element greater than or equal to the element
	while(count > 0) {
		auto it = out;
		step = count / 2;
		it += step;
		if(stable_quick_sort_less_func(strt, it, item, begidx)) {
			out = ++it;
			count -= step + 1;
		} else
			count = step;
	}
	return out != end && stable_quick_sort_greater_equal_func(strt, out, item, begidx) && stable_quick_sort_less_equal_func(strt, out, item, begidx);
}
template<typename Itr, typename IdxItr, typename Less>
bool stable_binary_search(Itr strt, Itr beg, Itr end, Itr item, IdxItr begidx,
						  Less comp, Itr& out) {
	//binary search return the insertion point, in both the found and not found case
	ptrdiff_t sze = distance(beg, end);
	if(sze == 0) {
		out = end;
		return false;
	}

	out = beg;
	ptrdiff_t step = 0;
	ptrdiff_t count = sze;

	//returns first element greater than or equal to the element
	while(count > 0) {
		auto it = out;
		step = count / 2;
		it += step;
		if(stable_quick_sort_less_func(strt, it, item, begidx, comp)) {
			out = ++it;
			count -= step + 1;
		} else
			count = step;
	}
	return out != end && stable_quick_sort_greater_equal_func(strt, out, item, begidx, comp) && stable_quick_sort_less_equal_func(strt, out, item, begidx, comp);
}
}


template<typename Itr>
void bubble_sort(Itr beg, Itr end) {
	auto dist = distance(beg, end);
	if(dist <= 1)
		return;
	//new optimisation, jumps the new end of this list to the last item swapped position in the list, as all greater must be larger
	size_t ndist = dist;
	while(ndist > 1) {
		ndist = 0;
		size_t pos = 1;
		for(Itr bg = beg; pos < dist; ++bg, ++pos) {
			Itr nxt = bg;
			++nxt;
			if(stlib_internal::less_func(*nxt, *bg)) {
				std::swap(*bg, *nxt);
				ndist = pos;
			}
		}
		dist = ndist;
	}
}
template<typename Itr, typename Comp>
void bubble_sort(Itr beg, Itr end, Comp cmp) {
	auto dist = distance(beg, end);
	if(dist <= 1)
		return;
	//new optimisation, jumps the new end of this list to the last item swapped position in the list, as all greater must be larger
	size_t ndist = dist;
	while(ndist > 1) {
		ndist = 0;
		size_t pos = 1;
		for(Itr bg = beg; pos < dist; ++bg, ++pos) {
			Itr nxt = bg;
			++nxt;
			if(stlib_internal::less_func(*nxt, *bg, cmp)) {
				std::swap(*bg, *nxt);
				ndist = pos;
			}
		}
		dist = ndist;
	}
}


template<typename Itr>
void cocktail_shaker_sort(Itr beg, Itr end) {
	auto dist = distance(beg, end);
	if(dist <= 1)
		return;
	//optimisated cocktail shaker sort, cuts off the ends of the list, both the start and the end of the list depending on the sorting
	Itr nbeg = beg - 1;
	size_t pos = 0;
	bool sorted = false;
	while(sorted == false) {
		sorted = true;
		Itr bg = nbeg + 1;
		++pos;
		{
			//do forward
			size_t ndist = 0;
			for(; pos < dist; ++bg, ++pos) {
				Itr nxt = bg;
				++nxt;
				if(stlib_internal::less_func(*nxt, *bg)) {
					std::swap(*bg, *nxt);
					ndist = pos;
					sorted = false;
				}
			}
			dist = ndist;
		}

		if(sorted == true)
			break;
		sorted = true;

		{
			//do backward
			Itr tbeg = nbeg;
			--bg; --pos;
			for(; bg != tbeg; --bg, --pos) {
				Itr nxt = bg;
				++nxt;
				if(stlib_internal::less_func(*nxt, *bg)) {
					std::swap(*bg, *nxt);
					nbeg = bg;
					sorted = false;
				}
			}
		}
	}
}
template<typename Itr, typename Comp>
void cocktail_shaker_sort(Itr beg, Itr end, Comp cmp) {
	auto dist = distance(beg, end);
	if(dist <= 1)
		return;
	//optimisated cocktail shaker sort, cuts off the ends of the list, both the start and the end of the list depending on the sorting
	Itr nbeg = beg - 1;
	size_t pos = 0;
	bool sorted = false;
	while(sorted == false) {
		sorted = true;
		Itr bg = nbeg + 1;
		++pos;
		{
			//do forward
			size_t ndist = 0;
			for(; pos < dist; ++bg, ++pos) {
				Itr nxt = bg;
				++nxt;
				if(stlib_internal::less_func(*nxt, *bg, cmp)) {
					std::swap(*bg, *nxt);
					ndist = pos;
					sorted = false;
				}
			}
			dist = ndist;
		}

		if(sorted == true)
			break;
		sorted = true;

		{
			//do backward
			Itr tbeg = nbeg;
			--bg; --pos;
			for(; bg != tbeg; --bg, --pos) {
				Itr nxt = bg;
				++nxt;
				if(stlib_internal::less_func(*nxt, *bg, cmp)) {
					std::swap(*bg, *nxt);
					nbeg = bg;
					sorted = false;
				}
			}
		}
	}
}


template<typename Itr>
void insertion_sort(Itr beg, Itr end) {
	if(distance(beg, end) <= 1)
		return;
	Itr strt = beg + 1;
	for(; strt != end; ++strt) {
		Itr crnt = strt;

		//move this to the correct place (do insert)
		using valueof = typename stlib_internal::value_for<Itr>::value_type;
		alignas(valueof) char item[sizeof(valueof)];
		valueof& val = *(valueof*)item;
		stlib_internal::construct(val, std::move(*crnt));

		while(crnt != beg && stlib_internal::greater_func(*(crnt - 1), val)) {
			stlib_internal::construct(*crnt, std::move(*(crnt - 1)));
			--crnt;
		}
		stlib_internal::construct(*crnt, std::move(val));
	}
}
template<typename Itr, typename Comp>
void insertion_sort(Itr beg, Itr end, Comp cmp) {
	if(distance(beg, end) <= 1)
		return;
	Itr strt = beg + 1;
	for(; strt != end; ++strt) {
		Itr crnt = strt;

		//move this to the correct place (do insert)
		using valueof = typename stlib_internal::value_for<Itr>::value_type;
		alignas(valueof) char item[sizeof(valueof)];
		valueof& val = *(valueof*)item;
		stlib_internal::construct(val, std::move(*crnt));

		while(crnt != beg && stlib_internal::greater_func(*(crnt - 1), val, cmp)) {
			stlib_internal::construct(*crnt, std::move(*(crnt - 1)));
			--crnt;
		}
		stlib_internal::construct(*crnt, std::move(val));
	}
}


template<typename Itr>
unsigned make_auxiliary_array(Itr strt, Itr end, typename stlib_internal::value_for<Itr>::value_type* arr, unsigned item_count, bool& test_consecutive) {
	//there must be atleast one item in the list in order for us to be here
	//move values from the array into the auxiliary array

	//go until we find some thing that is out of order, or we run out of items
	Itr beg = strt + 1;
	unsigned count = 1;
	unsigned up_down = 0; // 0 is unknown, 1 if ascending, 2 is decending
	if(beg != end) {
		if(stlib_internal::less_func(*beg, *strt)) {
			//we are decending
			up_down = 2;
			do {
				++count;
				++beg;
			} while(beg != end && count < item_count && stlib_internal::less_func(*beg, *(beg - 1), cmp));
		} else {
			//we are ascending
			up_down = 1;
			do {
				++count;
				++beg;
			} while(beg != end && count < item_count && stlib_internal::greater_equal_func(*beg, *(beg - 1), cmp));
			//if we are ascending and we have reached the end of the data or used all of the remaining space, test if we don't need to move any data (optimisation)
			if(test_consecutive && (beg == end || count == item_count) && stlib_internal::less_equal_func(*(strt - 1), *strt, cmp)) {
				test_consecutive = false;
				return count;
			}
		}
	}

	unsigned cnt = count;
	if(up_down == 2) {
		//reverse move this into the auxiliary array
		unsigned idx = 0;
		while(cnt > 0) {
			Itr itr = strt + (cnt - 1);
			construct(arr[idx], std::move(*itr));
			--cnt;
			++idx;
		}
	} else {
		//move this into the auxiliary array
		unsigned idx = 0;
		while(cnt > 0) {
			Itr itr = strt + idx;
			construct(arr[idx], std::move(*itr));
			--cnt;
			++idx;
		}
	}
	return count;
}
template<typename Itr>
void multi_insert(Itr beg, Itr strt_to, Itr strt, typename stlib_internal::value_for<Itr>::value_type* arr, unsigned items) {
	//go backwards insert into the output all of the items from array, stop when we run out of items
	unsigned count = distance(beg, strt);
	--strt;
	--strt_to;
	while(items > 0) {
		if(count > 0) {
			//do all of the comparisons on one go
			//if the existing buffer is greater then, otherwise copy from the array
			Itr tmp = strt;
			while(count > 0 && stlib_internal::greater_func(*tmp, arr[items - 1])) {
				//do move after we know that this is greater
				stlib_internal::construct(*strt_to, std::move(*tmp));
				--tmp;
				--count;
				--strt_to;
			}
			strt = tmp;
		}
		//move one of the items from the array into it's final position
		stlib_internal::construct(*strt_to, std::move(arr[items - 1]));
		--items;
		--strt_to;
	}
}
template<typename Itr>
void multi_insertion_sort(Itr beg, Itr end) {
	if(distance(beg, end) <= 1)
		return;

	//move this to the correct place (do insert)
	using valueof = typename stlib_internal::value_for<Itr>::value_type;
	constexpr unsigned itm_num = (((1024 / sizeof(valueof)) + (1024 % sizeof(valueof) ? 1 : 0)) < 10 ? ((1024 / sizeof(valueof)) + (1024 % sizeof(valueof) ? 1 : 0)) : 10);
	constexpr unsigned items = (itm_num < 5 ? 5 : itm_num);
	alignas(valueof) char item[sizeof(valueof) * items];
	valueof* arr = (valueof*)item;

	Itr strt = beg + 1;
	while(strt != end) {
		//build up an auxiliary array with correctly ordered items
		bool test_consecutive = true;
		unsigned count = make_auxiliary_array(strt, end, arr, items, test_consecutive);
		if(test_consecutive)
			//insert the items from the auxiliary array into the correct places in the main array
			multi_insert(beg, strt + count, strt, arr, count);
		//exit if we have reached the end
		strt = strt + count;
	}
}
template<typename Itr, typename Comp>
unsigned make_auxiliary_array(Itr strt, Itr end, typename stlib_internal::value_for<Itr>::value_type* arr, unsigned item_count, bool& test_consecutive, Comp cmp) {
	//there must be atleast one item in the list in order for us to be here
	//move values from the array into the auxiliary array

	//go until we find some thing that is out of order, or we run out of items
	Itr beg = strt + 1;
	unsigned count = 1;
	unsigned up_down = 0; // 0 is unknown, 1 if ascending, 2 is decending
	if(beg != end) {
		if(stlib_internal::less_func(*beg, *strt, cmp)) {
			//we are decending
			up_down = 2;
			do {
				++count;
				++beg;
			} while(beg != end && count < item_count && stlib_internal::less_func(*beg, *(beg - 1), cmp));
		} else {
			//we are ascending
			up_down = 1;
			do {
				++count;
				++beg;
			} while(beg != end && count < item_count && stlib_internal::greater_equal_func(*beg, *(beg - 1), cmp));
			//if we are ascending and we have reached the end of the data or used all of the remaining space, test if we don't need to move any data (optimisation)
			if(test_consecutive && (beg == end || count == item_count) && stlib_internal::less_equal_func(*(strt - 1), *strt, cmp)) {
				test_consecutive = false;
				return count;
			}
		}
	}

	unsigned cnt = count;
	if(up_down == 2) {
		//reverse move this into the auxiliary array
		unsigned idx = 0;
		while(cnt > 0) {
			Itr itr = strt + (cnt - 1);
			construct(arr[idx], std::move(*itr));
			--cnt;
			++idx;
		}
	} else {
		//move this into the auxiliary array
		unsigned idx = 0;
		while(cnt > 0) {
			Itr itr = strt + idx;
			construct(arr[idx], std::move(*itr));
			--cnt;
			++idx;
		}
	}
	return count;
}
template<typename Itr, typename Comp>
void multi_insert(Itr beg, Itr strt_to, Itr strt, typename stlib_internal::value_for<Itr>::value_type* arr, unsigned items, Comp cmp) {
	//go backwards insert into the output all of the items from array, stop when we run out of items
	unsigned count = distance(beg, strt);
	--strt;
	--strt_to;
	while(items > 0) {
		if(count > 0) {
			//do all of the comparisons on one go
			//if the existing buffer is greater then, otherwise copy from the array
			while(count > 0 && stlib_internal::greater_func(*tmp, arr[items - 1], cmp)) {
				//do move after we know that this is greater
				stlib_internal::construct(*strt_to, std::move(*tmp));
				--tmp;
				--count;
				--strt_to;
			}
			strt = tmp;
		}
		//move one of the items from the array into it's final position
		stlib_internal::construct(*strt_to, std::move(arr[items - 1]));
		--items;
		--strt_to;
	}
}
template<typename Itr, typename Comp>
void multi_insertion_sort(Itr beg, Itr end, Comp cmp) {
	if(distance(beg, end) <= 1)
		return;

	//move this to the correct place (do insert)
	using valueof = typename stlib_internal::value_for<Itr>::value_type;
	constexpr unsigned itm_num = (((1024 / sizeof(valueof)) + (1024 % sizeof(valueof) ? 1 : 0)) < 10 ? ((1024 / sizeof(valueof)) + (1024 % sizeof(valueof) ? 1 : 0)) : 10);
	constexpr unsigned items = (itm_num < 5 ? 5 : itm_num);
	alignas(valueof) char item[sizeof(valueof) * items];
	valueof* arr = (valueof*)item;

	Itr strt = beg + 1;
	while(strt != end) {
		//build up an auxiliary array with correctly ordered items
		bool test_consecutive = true;
		unsigned count = make_auxiliary_array(strt, end, arr, items, test_consecutive, cmp);
		if(test_consecutive)
			//insert the items from the auxiliary array into the correct places in the main array
			multi_insert(beg, strt + count, strt, arr, count, cmp);
		//exit if we have reached the end
		strt = strt + count;
	}
}


template<typename Itr>
void binary_insertion_sort(Itr beg, Itr end) {
	if(distance(beg, end) <= 1)
		return;

	Itr strt = beg + 1;
	for(; strt != end; ++strt) {
		Itr out;
		if(binary_search(beg, strt, *strt, out)) {
			//we have found one equal to this, move right until we find one that isn't equal
			++out;
			for(; out != strt && stlib_internal::equal_func(*out, *strt); ++out);
			--out;

			Itr c = strt;
			using valueof = typename stlib_internal::value_for<Itr>::value_type;
			alignas(valueof) char item[sizeof(valueof)];
			valueof& val = *(valueof*)item;
			stlib_internal::construct(val, std::move(*c));

			for(; c != out; --c)
				stlib_internal::construct(*c, std::move(*(c - 1)));
			stlib_internal::construct(*c, std::move(val));
		} else if(out != strt) {
			Itr c = strt;
			using valueof = typename stlib_internal::value_for<Itr>::value_type;
			alignas(valueof) char item[sizeof(valueof)];
			valueof& val = *(valueof*)item;
			stlib_internal::construct(val, std::move(*c));

			for(; c != out; --c)
				stlib_internal::construct(*c, std::move(*(c - 1)));
			stlib_internal::construct(*c, std::move(val));
		}
	}
}
template<typename Itr, typename Comp>
void binary_insertion_sort(Itr beg, Itr end, Comp cmp) {
	if(distance(beg, end) <= 1)
		return;

	Itr strt = beg + 1;
	for(; strt != end; ++strt) {
		Itr out;
		if(binary_search(beg, strt, *strt, cmp, out)) {
			//we have found one equal to this, move right until we find one that isn't equal
			++out;
			for(; out != strt && stlib_internal::equal_func(*out, *strt, cmp); ++out);
			--out;

			Itr c = strt;
			using valueof = typename stlib_internal::value_for<Itr>::value_type;
			alignas(valueof) char item[sizeof(valueof)];
			valueof& val = *(valueof*)item;
			stlib_internal::construct(val, std::move(*c));

			for(; c != out; --c)
				stlib_internal::construct(*c, std::move(*(c - 1)));
			stlib_internal::construct(*c, std::move(val));
		} else if(out != strt) {
			Itr c = strt;
			using valueof = typename stlib_internal::value_for<Itr>::value_type;
			alignas(valueof) char item[sizeof(valueof)];
			valueof& val = *(valueof*)item;
			stlib_internal::construct(val, std::move(*c));

			for(; c != out; --c)
				stlib_internal::construct(*c, std::move(*(c - 1)));
			stlib_internal::construct(*c, std::move(val));
		}
	}
}


template<typename Itr>
void selection_sort(Itr beg, Itr end) {
	auto dist = distance(beg, end);
	if(dist <= 1)
		return;

	Itr endp = end; --endp;
	for(; endp != beg; --endp) {
		Itr largest = endp;
		for(Itr bg = beg; bg != endp; ++bg)
			if(stlib_internal::greater_equal_func(*bg, *largest))
				largest = bg;
		//swap endp with largest
		if(endp != largest)
			std::swap(*endp, *largest);
	}
}
template<typename Itr, typename Comp>
void selection_sort(Itr beg, Itr end, Comp cmp) {
	auto dist = distance(beg, end);
	if(dist <= 1)
		return;

	Itr endp = end; --endp;
	for(; endp != beg; --endp) {
		Itr largest = endp;
		for(Itr bg = beg; bg != endp; ++bg)
			if(stlib_internal::greater_equal_func(*bg, *largest, cmp))
				largest = bg;
		//swap endp with largest
		if(endp != largest)
			std::swap(*endp, *largest);
	}
}
template<typename Itr>
void double_selection_sort(Itr beg, Itr end) {
	auto dist = distance(beg, end);
	if(dist <= 1)
		return;

	Itr endp = end; --endp;
	for(; beg + 1 != endp && endp != beg; ++beg, --endp) {
		//ensure we have the larger and the smaller of the end items tested with each other
		if(stlib_internal::less_func(*endp, *beg))
			std::swap(*beg, *endp);
		//find the smallest and the largest
		Itr smallest = beg;
		Itr largest = endp;
		for(Itr bg = beg + 1; bg != endp; ++bg) {
			if(stlib_internal::less_func(*bg, *smallest))
				smallest = bg;
			if(stlib_internal::greater_equal_func(*bg, *largest))
				largest = bg;
		}
		//swap beg with smallest
		if(beg != smallest)
			std::swap(*beg, *smallest);
		//swap endp with largest
		if(endp != largest)
			std::swap(*endp, *largest);
	}

	if(beg + 1 == endp && stlib_internal::less_func(*endp, *beg))
		std::swap(*beg, *endp);
}
template<typename Itr, typename Comp>
void double_selection_sort(Itr beg, Itr end, Comp cmp) {
	auto dist = distance(beg, end);
	if(dist <= 1)
		return;

	Itr endp = end; --endp;
	for(; beg + 1 != endp && endp != beg; ++beg, --endp) {
		//ensure we have the larger and the smaller of the end items tested with each other
		if(stlib_internal::less_func(*endp, *beg, cmp))
			std::swap(*beg, *endp);
		//find the smallest and the largest
		Itr smallest = beg;
		Itr largest = endp;
		for(Itr bg = beg + 1; bg != endp; ++bg) {
			if(stlib_internal::less_func(*bg, *smallest, cmp))
				smallest = bg;
			if(stlib_internal::greater_equal_func(*bg, *largest, cmp))
				largest = bg;
		}
		//swap beg with smallest
		if(beg != smallest)
			std::swap(*beg, *smallest);
		//swap endp with largest
		if(endp != largest)
			std::swap(*endp, *largest);
	}

	if(beg + 1 == endp && stlib_internal::less_func(*endp, *beg, cmp))
		std::swap(*beg, *endp);
}


template<typename Itr>
void quick_sort(Itr beg, Itr end) {
	if(distance(beg, end) <= 1)
		return;
	std::vector<stlib_internal::stack_less_data<Itr>> stk;
	stk.resize(15);
	size_t idx = 0;
	stlib_internal::stack_less_data<Itr> dat = {
		beg,
		end - 1
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		stlib_internal::stack_less_data<Itr> tmp = stk[--idx];
		Itr left = tmp.beg - 1;
		Itr right = tmp.end + 1;
		Itr pivot = stlib_internal::middle_of_three(tmp.beg, stlib_internal::half_point(tmp.beg, tmp.end + 1), tmp.end);

		do {
			++left;
			--right;
			//pivot goes to the right!!
			while(left != right && left != pivot && stlib_internal::less_func(*left, *pivot))
				++left;
			while(left != right && stlib_internal::greater_equal_func(*right, *pivot))
				--right;
			if(left == right)
				break;

			std::swap(*left, *right);
			if(left == pivot)
				pivot = right;
		} while(left + 1 != right);

		//if right is on the less side, move back
		if(right != pivot) {
			if(stlib_internal::less_func(*right, *pivot))
				++right;
			//move the pivot into place
			if(right != pivot) {
				std::swap(*right, *pivot);
				pivot = right;
			}
		}

		auto dist1 = distance(pivot + 1, tmp.end + 1);
		auto dist2 = distance(tmp.beg, pivot);
		//implements sort shorter first optimisation
		if(dist1 < dist2) {
			if(dist2 > 1)
				stlib_internal::add_stack_item(tmp.beg, pivot - 1, stk, idx);
			if(dist1 > 1)
				stlib_internal::add_stack_item(pivot + 1, tmp.end, stk, idx);
		} else {
			if(dist1 > 1)
				stlib_internal::add_stack_item(pivot + 1, tmp.end, stk, idx);
			if(dist2 > 1)
				stlib_internal::add_stack_item(tmp.beg, pivot - 1, stk, idx);
		}
	}
}
template<typename Itr, typename Comp>
void quick_sort(Itr beg, Itr end, Comp cmp) {
	if(distance(beg, end) <= 1)
		return;
	std::vector<stlib_internal::stack_less_data<Itr>> stk;
	stk.resize(15);
	size_t idx = 0;
	stlib_internal::stack_less_data<Itr> dat = {
		beg,
		end - 1
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		stlib_internal::stack_less_data<Itr> tmp = stk[--idx];
		Itr left = tmp.beg - 1;
		Itr right = tmp.end + 1;
		Itr pivot = stlib_internal::middle_of_three(tmp.beg, stlib_internal::half_point(tmp.beg, tmp.end + 1), tmp.end, cmp);

		do {
			++left;
			--right;
			//pivot goes to the right!!
			while(left != right && left != pivot && stlib_internal::less_func(*left, *pivot, cmp))
				++left;
			while(left != right && stlib_internal::greater_equal_func(*right, *pivot, cmp))
				--right;
			if(left == right)
				break;

			std::swap(*left, *right);
			if(left == pivot)
				pivot = right;
		} while(left + 1 != right);

		//if right is on the less side, move back
		if(right != pivot) {
			if(stlib_internal::less_func(*right, *pivot, cmp))
				++right;
			//move the pivot into place
			if(right != pivot) {
				std::swap(*right, *pivot);
				pivot = right;
			}
		}

		auto dist1 = distance(pivot + 1, tmp.end + 1);
		auto dist2 = distance(tmp.beg, pivot);
		//implements sort shorter first optimisation
		if(dist1 < dist2) {
			if(dist2 > 1)
				stlib_internal::add_stack_item(tmp.beg, pivot - 1, stk, idx);
			if(dist1 > 1)
				stlib_internal::add_stack_item(pivot + 1, tmp.end, stk, idx);
		} else {
			if(dist1 > 1)
				stlib_internal::add_stack_item(pivot + 1, tmp.end, stk, idx);
			if(dist2 > 1)
				stlib_internal::add_stack_item(tmp.beg, pivot - 1, stk, idx);
		}
	}
}

namespace stlib_internal {
template<typename Itr, typename IdxItr>
void stable_quick_sort_internal(Itr beg, Itr end, IdxItr begidx) {
	if(distance(beg, end) <= 1)
		return;
	std::vector<stack_less_data<Itr>> stk;
	stk.resize(15);
	size_t idx = 0;
	stack_less_data<Itr> dat = {
		beg,
		end - 1
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		stack_less_data<Itr> tmp = stk[--idx];
		Itr left = tmp.beg - 1;
		Itr right = tmp.end + 1;
		Itr pivot = middle_of_three(tmp.beg, half_point(tmp.beg, tmp.end + 1), tmp.end);

		do {
			++left;
			--right;
			//pivot goes to the right!!
			while(left != right && left != pivot && stable_quick_sort_less_func(beg, left, pivot, begidx))
				++left;
			while(left != right && stable_quick_sort_greater_equal_func(beg, right, pivot, begidx))
				--right;
			if(left == right)
				break;

			stable_quick_sort_swap(beg, left, right, begidx);
			if(left == pivot)
				pivot = right;
		} while(left + 1 != right);

		//if right is on the less side, move back
		if(right != pivot) {
			if(stable_quick_sort_less_func(beg, right, pivot, begidx))
				++right;
			//move the pivot into place
			if(right != pivot) {
				stable_quick_sort_swap(beg, right, pivot, begidx);
				pivot = right;
			}
		}

		auto dist1 = distance(pivot + 1, tmp.end + 1);
		auto dist2 = distance(tmp.beg, pivot);
		//implements sort shorter first optimisation
		if(dist1 < dist2) {
			if(dist2 > 1)
				add_stack_item(tmp.beg, pivot - 1, stk, idx);
			if(dist1 > 1)
				add_stack_item(pivot + 1, tmp.end, stk, idx);
		} else {
			if(dist1 > 1)
				add_stack_item(pivot + 1, tmp.end, stk, idx);
			if(dist2 > 1)
				add_stack_item(tmp.beg, pivot - 1, stk, idx);
		}
	}
}
}
template<typename Itr>
void stable_quick_sort(Itr beg, Itr end) {
	if(distance(beg, end) <= 1)
		return;
	std::vector<size_t> idxs;
	idxs.resize(distance(beg, end));
	for(size_t i = 0; i < idxs.size(); ++i)
		idxs[i] = i;
	stlib_internal::stable_quick_sort_internal(beg, end, idxs.begin());
}
namespace stlib_internal {
template<typename Itr, typename IdxItr>
void adaptive_stable_quick_sort_internal(Itr beg, Itr end, IdxItr begidx) {
	if(distance(beg, end) <= 1)
		return;
	//add a stack item
	std::vector<stack_less_data<Itr>> stk;
	stk.resize(15);
	size_t idx = 0;
	stack_less_data<Itr> dat = {
		beg,
		end - 1
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		stack_less_data<Itr> tmp = stk[--idx];
		Itr left = tmp.beg - 1;
		Itr right = tmp.end + 1;
		unsigned swaps = 0;
		Itr pivot = stable_middle_of_four(beg, tmp.beg, half_point(tmp.beg, tmp.end + 1), tmp.end, begidx);

		do {
			++left;
			--right;
			//pivot goes to the right!!
			while(left != right && left != pivot && stable_quick_sort_less_func(beg, left, pivot, begidx))
				++left;
			while(left != right && stable_quick_sort_greater_equal_func(beg, right, pivot, begidx))
				--right;
			if(left == right)
				break;

			stable_quick_sort_swap(beg, left, right, begidx);
			++swaps;
			if(left == pivot)
				pivot = right;
		} while(left + 1 != right);

		//if right is on the less side, move back
		if(right != pivot) {
			if(stable_quick_sort_less_func(beg, right, pivot, begidx))
				++right;
			//move the pivot into place
			if(right != pivot) {
				stable_quick_sort_swap(beg, right, pivot, begidx);
				++swaps;
				pivot = right;
			}
		}

		auto dist1 = distance(pivot + 1, tmp.end + 1);
		auto dist2 = distance(tmp.beg, pivot);
		if(swaps == 0 && ((dist1 > 1) | (dist2 > 1)) && stable_quick_sort_is_sorted(beg, tmp.beg, tmp.end + 1, begidx)) continue;

		//implements sort shorter first optimisation
		if(dist1 < dist2) {
			if(dist2 > 1)
				add_stack_item(tmp.beg, pivot - 1, stk, idx);
			if(dist1 > 1)
				add_stack_item(pivot + 1, tmp.end, stk, idx);
		} else {
			if(dist1 > 1)
				add_stack_item(pivot + 1, tmp.end, stk, idx);
			if(dist2 > 1)
				add_stack_item(tmp.beg, pivot - 1, stk, idx);
		}
	}
}
template<typename Itr, typename IdxItr>
void adaptive_stable_intro_sort_internal(Itr beg, Itr end, IdxItr begidx) {
	if(distance(beg, end) <= 1)
		return;
	unsigned maxdepth = get_depth(distance(beg, end));

	//add a stack item
	std::vector<intro_stack_less_data<Itr>> stk;
	stk.resize(15);
	size_t idx = 0;
	intro_stack_less_data<Itr> dat = {
		beg,
		end - 1,
		maxdepth
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		intro_stack_less_data<Itr> tmp = stk[--idx];
		Itr left = tmp.beg - 1;
		Itr right = tmp.end + 1;
		unsigned swaps = 0;
		Itr pivot = stable_middle_of_four(beg, tmp.beg, half_point(tmp.beg, tmp.end + 1), tmp.end, begidx);

		do {
			++left;
			--right;
			//pivot goes to the right!!
			while(left != right && left != pivot && stable_quick_sort_less_func(beg, left, pivot, begidx))
				++left;
			while(left != right && stable_quick_sort_greater_equal_func(beg, right, pivot, begidx))
				--right;
			if(left == right)
				break;

			stable_quick_sort_swap(beg, left, right, begidx);
			++swaps;
			if(left == pivot)
				pivot = right;
		} while(left + 1 != right);

		//if right is on the less side, move back
		if(right != pivot) {
			if(stable_quick_sort_less_func(beg, right, pivot, begidx))
				++right;
			//move the pivot into place
			if(right != pivot) {
				stable_quick_sort_swap(beg, right, pivot, begidx);
				++swaps;
				pivot = right;
			}
		}

		auto dist1 = distance(pivot + 1, tmp.end + 1);
		auto dist2 = distance(tmp.beg, pivot);
		if(swaps == 0 && ((dist1 > INSERTION_SORT_CUTOFF) | (dist2 > INSERTION_SORT_CUTOFF)) && stable_quick_sort_is_sorted(beg, tmp.beg, tmp.end + 1, begidx)) continue;

		//implements sort shorter first optimisation
		if(dist1 < dist2) {
			if(dist2 > INSERTION_SORT_CUTOFF)
				stable_add_stack_item(beg, tmp.beg, pivot, tmp.beg, pivot - 1, begidx, tmp.depth, stk, idx);
			if(dist1 > INSERTION_SORT_CUTOFF)
				stable_add_stack_item(beg, pivot + 1, tmp.end + 1, pivot + 1, tmp.end, begidx, tmp.depth, stk, idx);
		} else {
			if(dist1 > INSERTION_SORT_CUTOFF)
				stable_add_stack_item(beg, pivot + 1, tmp.end + 1, pivot + 1, tmp.end, begidx, tmp.depth, stk, idx);
			if(dist2 > INSERTION_SORT_CUTOFF)
				stable_add_stack_item(beg, tmp.beg, pivot, tmp.beg, pivot - 1, begidx, tmp.depth, stk, idx);
		}
	}
}
}
template<typename Itr>
void adaptive_stable_quick_sort(Itr beg, Itr end) {
	if(distance(beg, end) <= 1)
		return;
	std::vector<size_t> idxs;
	idxs.resize(distance(beg, end));
	for(size_t i = 0; i < idxs.size(); ++i)
		idxs[i] = i;
	stlib_internal::adaptive_stable_quick_sort_internal(beg, end, idxs.begin());
}
template<typename Itr>
void adaptive_stable_intro_sort(Itr beg, Itr end) {
	if(distance(beg, end) <= 1)
		return;
	std::vector<size_t> idxs;
	idxs.resize(distance(beg, end));
	for(size_t i = 0; i < idxs.size(); ++i)
		idxs[i] = i;
	stlib_internal::adaptive_stable_intro_sort_internal(beg, end, idxs.begin());

	stlib_internal::stable_insertion_sort(beg, end, idxs.begin());
}
namespace stlib_internal {
template<typename Itr, typename IdxItr, typename Comp>
void stable_quick_sort_internal(Itr beg, Itr end, IdxItr begidx, Comp cmp) {
	if(distance(beg, end) <= 1)
		return;
	std::vector<stack_less_data<Itr>> stk;
	stk.resize(15);
	size_t idx = 0;
	stack_less_data<Itr> dat = {
		beg,
		end - 1
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		stack_less_data<Itr> tmp = stk[--idx];
		Itr left = tmp.beg - 1;
		Itr right = tmp.end + 1;
		Itr pivot = middle_of_three(tmp.beg, half_point(tmp.beg, tmp.end + 1), tmp.end, cmp);

		do {
			++left;
			--right;
			//pivot goes to the right!!
			while(left != right && left != pivot && stable_quick_sort_less_func(beg, left, pivot, begidx, cmp))
				++left;
			while(left != right && stable_quick_sort_greater_equal_func(beg, right, pivot, begidx, cmp))
				--right;
			if(left == right)
				break;

			stable_quick_sort_swap(beg, left, right, begidx);
			if(left == pivot)
				pivot = right;
		} while(left + 1 != right);

		//if right is on the less side, move back
		if(right != pivot) {
			if(stable_quick_sort_less_func(beg, right, pivot, begidx, cmp))
				++right;
			//move the pivot into place
			if(right != pivot) {
				stable_quick_sort_swap(beg, right, pivot, begidx);
				pivot = right;
			}
		}

		auto dist1 = distance(pivot + 1, tmp.end + 1);
		auto dist2 = distance(tmp.beg, pivot);
		//implements sort shorter first optimisation
		if(dist1 < dist2) {
			if(dist2 > 1)
				add_stack_item(tmp.beg, pivot - 1, stk, idx);
			if(dist1 > 1)
				add_stack_item(pivot + 1, tmp.end, stk, idx);
		} else {
			if(dist1 > 1)
				add_stack_item(pivot + 1, tmp.end, stk, idx);
			if(dist2 > 1)
				add_stack_item(tmp.beg, pivot - 1, stk, idx);
		}
	}
}
}
template<typename Itr, typename Comp>
void stable_quick_sort(Itr beg, Itr end, Comp cmp) {
	if(distance(beg, end) <= 1)
		return;
	std::vector<size_t> idxs;
	idxs.resize(distance(beg, end));
	for(size_t i = 0; i < idxs.size(); ++i)
		idxs[i] = i;
	stlib_internal::stable_quick_sort_internal(beg, end, idxs.begin(), cmp);
}
namespace stlib_internal {
template<typename Itr, typename IdxItr, typename Comp>
void adaptive_stable_quick_sort_internal(Itr beg, Itr end, IdxItr begidx, Comp cmp) {
	if(distance(beg, end) <= 1)
		return;
	//add a stack item
	std::vector<stack_less_data<Itr>> stk;
	stk.resize(15);
	size_t idx = 0;
	stack_less_data<Itr> dat = {
		beg,
		end - 1
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		stack_less_data<Itr> tmp = stk[--idx];
		Itr left = tmp.beg - 1;
		Itr right = tmp.end + 1;
		unsigned swaps = 0;
		Itr pivot = stable_middle_of_four(beg, tmp.beg, half_point(tmp.beg, tmp.end + 1), tmp.end, begidx, cmp);

		do {
			++left;
			--right;
			//pivot goes to the right!!
			while(left != right && left != pivot && stable_quick_sort_less_func(beg, left, pivot, begidx, cmp))
				++left;
			while(left != right && stable_quick_sort_greater_equal_func(beg, right, pivot, begidx, cmp))
				--right;
			if(left == right)
				break;

			stable_quick_sort_swap(beg, left, right, begidx);
			++swaps;
			if(left == pivot)
				pivot = right;
		} while(left + 1 != right);

		//if right is on the less side, move back
		if(right != pivot) {
			if(stable_quick_sort_less_func(beg, right, pivot, begidx, cmp))
				++right;
			//move the pivot into place
			if(right != pivot) {
				stable_quick_sort_swap(beg, right, pivot, begidx);
				++swaps;
				pivot = right;
			}
		}

		auto dist1 = distance(pivot + 1, tmp.end + 1);
		auto dist2 = distance(tmp.beg, pivot);
		if(swaps == 0 && ((dist1 > 1) | (dist2 > 1)) && stable_quick_sort_is_sorted(beg, tmp.beg, tmp.end + 1, begidx, cmp)) continue;

		//implements sort shorter first optimisation
		if(dist1 < dist2) {
			if(dist2 > 1)
				add_stack_item(tmp.beg, pivot - 1, stk, idx);
			if(dist1 > 1)
				add_stack_item(pivot + 1, tmp.end, stk, idx);
		} else {
			if(dist1 > 1)
				add_stack_item(pivot + 1, tmp.end, stk, idx);
			if(dist2 > 1)
				add_stack_item(tmp.beg, pivot - 1, stk, idx);
		}
	}
}
template<typename Itr, typename IdxItr, typename Comp>
void adaptive_stable_intro_sort_internal(Itr beg, Itr end, IdxItr begidx, Comp cmp) {
	if(distance(beg, end) <= 1)
		return;
	unsigned maxdepth = get_depth(distance(beg, end));

	//add a stack item
	std::vector<intro_stack_less_data<Itr>> stk;
	stk.resize(15);
	size_t idx = 0;
	intro_stack_less_data<Itr> dat = {
		beg,
		end - 1,
		maxdepth
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		intro_stack_less_data<Itr> tmp = stk[--idx];
		Itr left = tmp.beg - 1;
		Itr right = tmp.end + 1;
		unsigned swaps = 0;
		Itr pivot = stable_middle_of_four(beg, tmp.beg, half_point(tmp.beg, tmp.end + 1), tmp.end, begidx, cmp);

		do {
			++left;
			--right;
			//pivot goes to the right!!
			while(left != right && left != pivot && stable_quick_sort_less_func(beg, left, pivot, begidx, cmp))
				++left;
			while(left != right && stable_quick_sort_greater_equal_func(beg, right, pivot, begidx, cmp))
				--right;
			if(left == right)
				break;

			stable_quick_sort_swap(beg, left, right, begidx);
			++swaps;
			if(left == pivot)
				pivot = right;
		} while(left + 1 != right);

		//if right is on the less side, move back
		if(right != pivot) {
			if(stable_quick_sort_less_func(beg, right, pivot, begidx, cmp))
				++right;
			//move the pivot into place
			if(right != pivot) {
				stable_quick_sort_swap(beg, right, pivot, begidx);
				++swaps;
				pivot = right;
			}
		}

		auto dist1 = distance(pivot + 1, tmp.end + 1);
		auto dist2 = distance(tmp.beg, pivot);
		if(swaps == 0 && ((dist1 > INSERTION_SORT_CUTOFF) | (dist2 > INSERTION_SORT_CUTOFF)) && stable_quick_sort_is_sorted(beg, tmp.beg, tmp.end + 1, begidx, cmp)) continue;

		//implements sort shorter first optimisation
		if(dist1 < dist2) {
			if(dist2 > INSERTION_SORT_CUTOFF)
				stable_add_stack_item(beg, tmp.beg, pivot, tmp.beg, pivot - 1, begidx, tmp.depth, stk, idx, cmp);
			if(dist1 > INSERTION_SORT_CUTOFF)
				stable_add_stack_item(beg, pivot + 1, tmp.end + 1, pivot + 1, tmp.end, begidx, tmp.depth, stk, idx, cmp);
		} else {
			if(dist1 > INSERTION_SORT_CUTOFF)
				stable_add_stack_item(beg, pivot + 1, tmp.end + 1, pivot + 1, tmp.end, begidx, tmp.depth, stk, idx, cmp);
			if(dist2 > INSERTION_SORT_CUTOFF)
				stable_add_stack_item(beg, tmp.beg, pivot, tmp.beg, pivot - 1, begidx, tmp.depth, stk, idx, cmp);
		}
	}
}
}
template<typename Itr, typename Comp>
void adaptive_stable_quick_sort(Itr beg, Itr end, Comp cmp) {
	if(distance(beg, end) <= 1)
		return;
	std::vector<size_t> idxs;
	idxs.resize(distance(beg, end));
	for(size_t i = 0; i < idxs.size(); ++i)
		idxs[i] = i;
	stlib_internal::adaptive_stable_quick_sort_internal(beg, end, idxs.begin(), cmp);
}
template<typename Itr, typename Comp>
void adaptive_stable_intro_sort(Itr beg, Itr end, Comp cmp) {
	if(distance(beg, end) <= 1)
		return;
	std::vector<size_t> idxs;
	idxs.resize(distance(beg, end));
	for(size_t i = 0; i < idxs.size(); ++i)
		idxs[i] = i;
	stlib_internal::adaptive_stable_intro_sort_internal(beg, end, idxs.begin(), cmp);

	stlib_internal::stable_insertion_sort(beg, end, idxs.begin(), cmp);
}


template<typename Itr>
bool stack_quick_sort(Itr beg, Itr end, uint32_t limit = 100) {
	if(distance(beg, end) <= 1)
		return true;
	if(limit == 0)
		return false;
	Itr left = beg;
	Itr right = end;
	--right;
	Itr rend = right;

	Itr pivot = stlib_internal::middle_of_three(left, stlib_internal::half_point(beg, end), right);
	while(left != right) {
		//all that are equal go to the left!!
		while(left != pivot && stlib_internal::less_equal_func(*left, *pivot))
			++left;
		while(right != pivot && stlib_internal::greater_func(*right, *pivot))
			--right;
		if(left != right) {
			std::swap(*left, *right);
			if(left == pivot)
				pivot = right;
			else if(right == pivot)
				pivot = left;
		}
	}

	bool rtn = true;
	auto dist1 = distance(left, end);
	auto dist2 = distance(beg, left);
	//implements sort shorter first optimisation
	if(dist1 < dist2) {
		if(left != rend) rtn &= stack_quick_sort(left, end, limit - 1);
		if(beg != left) rtn &= stack_quick_sort(beg, left, limit - 1);
	} else {
		if(beg != left) rtn &= stack_quick_sort(beg, left, limit - 1);
		if(left != rend) rtn &= stack_quick_sort(left, end, limit - 1);
	}
	return rtn;
}
template<typename Itr, typename Comp>
bool stack_quick_sort(Itr beg, Itr end, uint32_t limit, Comp cmp) {
	if(distance(beg, end) <= 1)
		return true;
	if(limit == 0)
		return false;
	Itr left = beg;
	Itr right = end;
	--right;
	Itr rend = right;

	Itr pivot = stlib_internal::middle_of_three(left, stlib_internal::half_point(beg, end), right, cmp);
	while(left != right) {
		//all that are equal go to the left!!
		while(left != pivot && stlib_internal::less_equal_func(*left, *pivot, cmp))
			++left;
		while(right != pivot && stlib_internal::greater_func(*right, *pivot, cmp))
			--right;
		if(left != right) {
			std::swap(*left, *right);
			if(left == pivot)
				pivot = right;
			else if(right == pivot)
				pivot = left;
		}
	}

	bool rtn = true;
	auto dist1 = distance(left, end);
	auto dist2 = distance(beg, left);
	//implements sort shorter first optimisation
	if(dist1 < dist2) {
		if(left != rend) rtn &= stack_quick_sort(left, end, limit - 1, cmp);
		if(beg != left) rtn &= stack_quick_sort(beg, left, limit - 1, cmp);
	} else {
		if(beg != left) rtn &= stack_quick_sort(beg, left, limit - 1, cmp);
		if(left != rend) rtn &= stack_quick_sort(left, end, limit - 1, cmp);
	}
	return rtn;
}
template<typename Itr, typename Comp>
inline bool stack_quick_sort(Itr beg, Itr end, Comp cmp) {
	return stack_quick_sort(beg, end, 100, cmp);
}


namespace stlib_internal {
template<typename Itr>
struct merge_sweep_stack_less_data {
	Itr beg;
	Itr end;
	Itr nhalf;
};
template<typename Itr>
void do_merge_sweep(merge_sweep_stack_less_data<Itr>& lhs, merge_sweep_stack_less_data<Itr>& rhs, Itr& pivot) {
	//NOTE end of lhs == beg of rhs

	//do rotate to combine the halfs
	//swap the greater of the first with the less of the second
	stlib_internal::rotate(lhs.nhalf, lhs.end, rhs.nhalf);

	//keep track of pivot, pivot moves with greater
	if(pivot >= lhs.nhalf && pivot < lhs.end)
		pivot += distance(rhs.beg, rhs.nhalf);

	//update this to be the merged region
	lhs.end = rhs.end;
	lhs.nhalf += distance(rhs.beg, rhs.nhalf);
}
template<typename Itr>
void comp_swap(Itr left, Itr right, Itr& pivot) {
	if(less_func(*right, *left)) {
		std::swap(*left, *right);
		if(pivot == right)
			pivot = left;
		else if(pivot == left)
			pivot = right;
	}
}
template<typename Itr>
void merge_sweep_sort_add_item(Itr& bg, Itr end, Itr& pivot, std::vector<merge_sweep_stack_less_data<Itr>>& stk, size_t& idx) {
	auto tmp = bg;
	++tmp;
	if(tmp == end) {
		merge_sweep_stack_less_data<Itr> sld{ bg, end, bg };

		//record the half way point for each
		if(less_func(*bg, *pivot))
			++sld.nhalf;

		stk[idx++] = sld;
		if(idx == stk.size())
			stk.resize(stk.size() * 2);
		++bg;
	} else {
		//ensure correct ordering of pairs
		comp_swap(bg, bg + 1, pivot);
		merge_sweep_stack_less_data<Itr> sld{ bg, bg + 2, bg };

		//record the half way point for each
		if(less_func(*bg, *pivot)) {
			++sld.nhalf;
			if(less_func(*(bg + 1), *pivot))
				++sld.nhalf;
		}

		stk[idx++] = sld;
		if(idx == stk.size())
			stk.resize(stk.size() * 2);
		bg += 2;
	}
}
template<typename Itr>
void merge_sweep_sort_iterative(Itr& pivot, Itr beg, Itr end, Itr& nhalf) {
	uint64_t sze = distance(beg, end);
	if(sze <= 0) {
		nhalf = beg;
		return;
	}
	if(sze <= 1) {
		if(less_func(*beg, *pivot))
			nhalf = end;
		else
			nhalf = beg;
		return;
	}
	if(sze <= 2) {
		comp_swap(beg, beg + 1, pivot);

		//record the half way point for each
		nhalf = beg;
		if(less_func(*beg, *pivot)) {
			++nhalf;
			if(less_func(*(beg + 1), *pivot))
				++nhalf;
		}
		return;
	}

	Itr bg = beg;
	std::vector<merge_sweep_stack_less_data<Itr>> stk;
	stk.resize(15);
	size_t idx = 0;

	merge_sweep_sort_add_item(bg, end, pivot, stk, idx);

	do {
		if(bg != end)
			//add another item
			merge_sweep_sort_add_item(bg, end, pivot, stk, idx);

		//merge the last two items in the list together if they are the same size
		while(idx > 1 && (bg == end || distance(stk[idx - 2].beg, stk[idx - 2].end) == distance(stk[idx - 1].beg, stk[idx - 1].end))) {
			do_merge_sweep(stk[idx - 2], stk[idx - 1], pivot);
			--idx;
		}
	} while((bg != end) | (idx > 1));

	nhalf = stk.front().nhalf;
}
template<typename Itr>
void merge_sweep_sort_recurse(Itr& pivot, Itr beg, Itr end, Itr& nhalf) {
	if(distance(beg, end) == 0) {
		nhalf = beg;
		return;
	}
	if(distance(beg, end) == 1) {
		nhalf = beg;
		if(less_func(*nhalf, *pivot))
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
	stlib_internal::rotate(nhalf1, half, nhalf2);
	//keep track of pivot, pivot moves lesser up
	if(pivot >= nhalf1 && pivot < half)
		pivot += distance(half, nhalf2);

	//the new half point is all of the lesser
	nhalf = nhalf1 + distance(half, nhalf2);
}
}
template<typename Itr>
void stack_merge_sweep_sort(Itr beg, Itr end) {
	if(distance(beg, end) <= 1)
		return;
	Itr pivot = stlib_internal::half_point(beg, end);

	Itr nhalf;
	stlib_internal::merge_sweep_sort_iterative(pivot, beg, end, nhalf);

	stlib_internal::move_pivot(nhalf, pivot);

	stack_merge_sweep_sort(beg, nhalf);
	stack_merge_sweep_sort(pivot + 1, end);
}
template<typename Itr>
void merge_sweep_sort(Itr beg, Itr end) {
	if(distance(beg, end) <= 1)
		return;
	std::vector<stlib_internal::stack_less_data<Itr>> stk;
	stk.resize(15);
	size_t idx = 0;
	stlib_internal::stack_less_data<Itr> dat = {
		beg,
		end
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		stlib_internal::stack_less_data<Itr> item = stk[--idx];

		Itr pivot = stlib_internal::middle_of_three(item.beg, stlib_internal::half_point(item.beg, item.end), item.end - 1);

		Itr nhalf;
		stlib_internal::merge_sweep_sort_iterative(pivot, item.beg, item.end, nhalf);

		stlib_internal::move_pivot(nhalf, pivot);

		auto dist1 = distance(pivot + 1, item.end);
		auto dist2 = distance(item.beg, nhalf);
		//implements sort shorter first optimisation
		if(dist1 < dist2) {
			if(dist2 > 1)
				stlib_internal::add_stack_item(item.beg, nhalf, stk, idx);
			if(dist1 > 1)
				stlib_internal::add_stack_item(pivot + 1, item.end, stk, idx);
		} else {
			if(dist1 > 1)
				stlib_internal::add_stack_item(pivot + 1, item.end, stk, idx);
			if(dist2 > 1)
				stlib_internal::add_stack_item(item.beg, nhalf, stk, idx);
		}
	}
}
namespace stlib_internal {
template<typename Itr, typename Comp>
void comp_swap(Itr left, Itr right, Itr& pivot, Comp cmp) {
	if(less_func(*right, *left, cmp)) {
		std::swap(*left, *right);
		if(pivot == right)
			pivot = left;
		else if(pivot == left)
			pivot = right;
	}
}
template<typename Itr, typename Comp>
void merge_sweep_sort_add_item(Itr& bg, Itr end, Itr& pivot, std::vector<merge_sweep_stack_less_data<Itr>>& stk, size_t& idx, Comp cmp) {
	auto tmp = bg;
	++tmp;
	if(tmp == end) {
		merge_sweep_stack_less_data<Itr> sld{ bg, end, bg };

		//record the half way point for each
		if(less_func(*bg, *pivot, cmp))
			++sld.nhalf;

		stk[idx++] = sld;
		if(idx == stk.size())
			stk.resize(stk.size() * 2);
		++bg;
	} else {
		//ensure correct ordering of pairs
		comp_swap(bg, bg + 1, pivot, cmp);
		merge_sweep_stack_less_data<Itr> sld{ bg, bg + 2, bg };

		//record the half way point for each
		if(less_func(*bg, *pivot, cmp)) {
			++sld.nhalf;
			if(less_func(*(bg + 1), *pivot, cmp))
				++sld.nhalf;
		}

		stk[idx++] = sld;
		if(idx == stk.size())
			stk.resize(stk.size() * 2);
		bg += 2;
	}
}
template<typename Itr, typename Comp>
void merge_sweep_sort_iterative(Itr& pivot, Itr beg, Itr end, Itr& nhalf, Comp cmp) {
	uint64_t sze = distance(beg, end);
	if(sze <= 0) {
		nhalf = beg;
		return;
	}
	if(sze <= 1) {
		if(less_func(*beg, *pivot, cmp))
			nhalf = end;
		else
			nhalf = beg;
		return;
	}
	if(sze <= 2) {
		comp_swap(beg, beg + 1, pivot, cmp);

		//record the half way point for each
		nhalf = beg;
		if(less_func(*beg, *pivot, cmp)) {
			++nhalf;
			if(less_func(*(beg + 1), *pivot, cmp))
				++nhalf;
		}
		return;
	}

	Itr bg = beg;
	std::vector<merge_sweep_stack_less_data<Itr>> stk;
	stk.resize(15);
	size_t idx = 0;

	merge_sweep_sort_add_item(bg, end, pivot, stk, idx, cmp);

	do {
		if(bg != end)
			//add another item
			merge_sweep_sort_add_item(bg, end, pivot, stk, idx, cmp);

		//merge the last two items in the list together if they are the same size
		while(idx > 1 && (bg == end || distance(stk[idx - 2].beg, stk[idx - 2].end) == distance(stk[idx - 1].beg, stk[idx - 1].end))) {
			do_merge_sweep(stk[idx - 2], stk[idx - 1], pivot);
			--idx;
		}
	} while((bg != end) | (idx > 1));

	nhalf = stk.front().nhalf;
}
template<typename Itr, typename Comp>
void merge_sweep_sort_recurse(Itr& pivot, Itr beg, Itr end, Itr& nhalf, Comp cmp) {
	if(distance(beg, end) == 0) {
		nhalf = beg;
		return;
	}
	if(distance(beg, end) == 1) {
		nhalf = beg;
		if(less_func(*nhalf, *pivot, cmp))
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
	stlib_internal::rotate(nhalf1, half, nhalf2);
	//keep track of pivot, pivot moves lesser up
	if(pivot >= nhalf1 && pivot < half)
		pivot += distance(half, nhalf2);

	//the new half point is all of the lesser
	nhalf = nhalf1 + distance(half, nhalf2);
}
}
template<typename Itr, typename Comp>
void stack_merge_sweep_sort(Itr beg, Itr end, Comp cmp) {
	if(distance(beg, end) <= 1)
		return;
	Itr pivot = stlib_internal::half_point(beg, end);

	Itr nhalf;
	stlib_internal::merge_sweep_sort_iterative(pivot, beg, end, nhalf, cmp);

	stlib_internal::move_pivot(nhalf, pivot, cmp);

	stack_merge_sweep_sort(beg, nhalf, cmp);
	stack_merge_sweep_sort(pivot + 1, end, cmp);
}
template<typename Itr, typename Comp>
void merge_sweep_sort(Itr beg, Itr end, Comp cmp) {
	if(distance(beg, end) <= 1)
		return;
	std::vector<stlib_internal::stack_less_data<Itr>> stk;
	stk.resize(15);
	size_t idx = 0;
	stlib_internal::stack_less_data<Itr> dat = {
		beg,
		end
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		stlib_internal::stack_less_data<Itr> item = stk[--idx];

		Itr pivot = stlib_internal::middle_of_three(item.beg, stlib_internal::half_point(item.beg, item.end), item.end - 1, cmp);

		Itr nhalf;
		stlib_internal::merge_sweep_sort_iterative(pivot, item.beg, item.end, nhalf, cmp);

		stlib_internal::move_pivot(nhalf, pivot, cmp);

		auto dist1 = distance(pivot + 1, item.end);
		auto dist2 = distance(item.beg, nhalf);
		//implements sort shorter first optimisation
		if(dist1 < dist2) {
			if(dist2 > 1)
				stlib_internal::add_stack_item(item.beg, nhalf, stk, idx);
			if(dist1 > 1)
				stlib_internal::add_stack_item(pivot + 1, item.end, stk, idx);
		} else {
			if(dist1 > 1)
				stlib_internal::add_stack_item(pivot + 1, item.end, stk, idx);
			if(dist2 > 1)
				stlib_internal::add_stack_item(item.beg, nhalf, stk, idx);
		}
	}
}


namespace stlib_internal {
template<typename Itr1, typename Itr2, typename Comp>
void merge_internal(Itr1 beg1, Itr1 beg2, Itr1 end2, Itr2& begout, Comp cmp) {
	Itr1 end1 = beg2;

	//go through both lists, build the sorted list
	for(; beg1 != end1 && beg2 != end2; ++begout)
		if(less_func(*beg2, *beg1, cmp)) {
			construct(*begout, std::move(*beg2));
			++beg2;
		} else {
			construct(*begout, std::move(*beg1));
			++beg1;
		}

	//finish off copy back of remaining lists (if any)
	copy_buffers(beg1, end1, begout);
	copy_buffers(beg2, end2, begout);
	return;
}
template<typename Itr, typename T, typename Comp>
void merge_sort_internal(Itr beg, Itr end, T* buf, Comp cmp) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;
	T* bfrend = buf + sze;

	//go through all of the lengths starting at 1 doubling
	uint64_t len = 1;
	bool first = true;
	while(len < sze) {
		uint64_t pos = 0;
		//go through all of the sorted sublists, zip them together
		//which way are we copying this?
		if(first) {
			T* bufbeg = buf;
			//T* bufend = bfrend;
			while(pos + len < sze) {
				//make the two halves
				Itr cleft = beg + pos;
				Itr cright = cleft + len;
				Itr cend = (pos + (len * 2) > sze ? end : cleft + (len * 2));

				//do merge
				merge_internal(cleft, cright, cend, bufbeg, cmp);
				pos += (len * 2);
			}
			//move the rest of the buffer across (needed as we swap buffers)
			if(pos < sze) {
				Itr cleft = beg + pos;
				copy_buffers(cleft, end, bufbeg);
			}
		} else {
			Itr bufbeg = beg;
			//Itr bufend = end;
			while(pos + len < sze) {
				//make the two halves
				T* cleft = buf + pos;
				T* cright = cleft + len;
				T* cend = (pos + (len * 2) > sze ? bfrend : cleft + (len * 2));

				//do merge
				merge_internal(cleft, cright, cend, bufbeg, cmp);
				pos += (len * 2);
			}
			//move the rest of the buffer across (needed as we swap buffers)
			if(pos < sze) {
				T* cleft = buf + pos;
				copy_buffers(cleft, bfrend, bufbeg);
			}
		}

		len *= 2;
		first = !first;
	}

	//ensure we copy this back at the original buffer if needed
	if(!first)
		copy_buffers(buf, bfrend, beg);
}
}
template<typename Itr, typename Comp>
bool merge_sort(Itr beg, Itr end, Comp cmp) {
	if(distance(beg, end) <= 1)
		return true;
	using valueof = typename stlib::stlib_internal::value_for<Itr>::value_type;
	valueof* buf = (valueof*)stlib_internal::aligned_storage_new(distance(beg, end) * sizeof(valueof));
	if(buf) {
		stlib_internal::merge_sort_internal(beg, end, buf, cmp);

		stlib_internal::aligned_storage_delete(distance(beg, end) * sizeof(valueof), buf);
		return true;
	}
	return false;
}
namespace stlib_internal {
bool start_out_of_place_merge(uint64_t sze, uint64_t insert_count);
template<typename Itr, typename T, typename Comp>
void out_of_place_insertion_sort(Itr beg, Itr end, T* buf, Comp cmp) {
	if(distance(beg, end) <= 1)
		return;

	//do an insertion sort but copying the array to a different destination
	T* lsted = buf;
	for(Itr strt = beg; strt != end; ++strt, ++lsted) {
		T* led = lsted;
		while(led != buf && greater_func(*(led - 1), *strt), cmp) {
			construct(*led, std::move(*(led - 1)));
			--led;
		}
		construct(*led, std::move(*strt));
	}
}
template<typename Itr, typename T, typename Comp>
void out_of_place_multi_insertion_sort(Itr beg, Itr end, T* buf, Comp cmp) {
	if(distance(beg, end) <= 1)
		return;

	//move this to the correct place (do insert)
	using valueof = typename stlib_internal::value_for<Itr>::value_type;
	constexpr unsigned itm_num = (((1024 / sizeof(valueof)) + (1024 % sizeof(valueof) ? 1 : 0)) < 10 ? ((1024 / sizeof(valueof)) + (1024 % sizeof(valueof) ? 1 : 0)) : 10);
	constexpr unsigned items = (itm_num < 5 ? 5 : itm_num);
	alignas(valueof) char item[sizeof(valueof) * items];
	valueof* arr = (valueof*)item;

	//move the results into the output buffer
	T* bufbg = buf;
	T* bufed = buf;
	construct(*buf, std::move(*beg));
	Itr strt = beg + 1;
	++bufed;
	do {
		//build up an auxiliary array with correctly ordered items
		bool test_consecutive = false;
		unsigned count = make_auxiliary_array(strt, end, arr, items, test_consecutive, cmp);
		//insert the items from the auxiliary array into the correct places in the main array
		multi_insert(bufbg, bufed + count, bufed, arr, count, cmp);
		//exit if we have reached the end
		strt = strt + count;
		bufed = bufed + count;
	} while(strt != end);
}
template<typename Itr, typename T, typename Comp>
void hybrid_merge_sort_internal(Itr beg, Itr end, T* buf, Comp cmp) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;

	uint64_t insert_count = INSERTION_SORT_CUTOFF;
	bool out_of_place = start_out_of_place_merge(sze, insert_count);
	//sort small runs with insertion sort before doing merge
	if(out_of_place) {
		uint64_t len = insert_count;
		uint64_t count = 0;
		for(Itr bg = beg; bg != end; count+=len) {
			Itr ed = (count + len > sze ? end : bg + len);
			out_of_place_multi_insertion_sort(bg, ed, buf + count, cmp);
			bg = ed;
		}
	} else {
		uint64_t len = insert_count;
		uint64_t count = 0;
		for(Itr bg = beg; bg != end; count+=len) {
			Itr ed = (count + len > sze ? end : bg + len);
			multi_insertion_sort(bg, ed, cmp);
			bg = ed;
		}
	}
	if(sze <= insert_count)
		return;

	T* bfrend = buf + sze;
	//go through all of the lengths starting at 1 doubling
	uint64_t len = insert_count;
	while(len < sze) {
		uint64_t pos = 0;
		//go through all of the sorted sublists, zip them together
		//which way are we copying this?
		if(out_of_place) {
			Itr bufbeg = beg;
			Itr bufend = end;
			while(pos + len < sze) {
				//make the two halves
				T* cleft = buf + pos;
				T* cright = cleft + len;
				T* cend = (pos + (len * 2) > sze ? bfrend : cleft + (len * 2));

				//do merge
				merge_internal(cleft, cright, cend, bufbeg, cmp);
				pos += (len * 2);
			}
			//move the rest of the buffer across (needed as we swap buffers)
			if(pos < sze) {
				T* cleft = buf + pos;
				copy_buffers(cleft, bfrend, bufbeg);
			}
		} else {
			T* bufbeg = buf;
			T* bufend = bfrend;
			while(pos + len < sze) {
				//make the two halves
				Itr cleft = beg + pos;
				Itr cright = cleft + len;
				Itr cend = (pos + (len * 2) > sze ? end : cleft + (len * 2));

				//do merge
				merge_internal(cleft, cright, cend, bufbeg, cmp);
				pos += (len * 2);
			}
			//move the rest of the buffer across (needed as we swap buffers)
			if(pos < sze) {
				Itr cleft = beg + pos;
				copy_buffers(cleft, end, bufbeg);
			}
		}

		len *= 2;
		out_of_place = !out_of_place;
	}

	//ensure we copy this back at the original buffer if needed, (should never happen because of start_out_of_place_merge)
	if(out_of_place)
		copy_buffers(buf, bfrend, beg);
}
}
template<typename Itr, typename Comp>
bool hybrid_merge_sort(Itr beg, Itr end, Comp cmp) {
	if(distance(beg, end) <= 1)
		return true;
	using valueof = typename stlib::stlib_internal::value_for<Itr>::value_type;
	valueof* buf = (valueof*)stlib_internal::aligned_storage_new(distance(beg, end) * sizeof(valueof));
	if(buf) {
		stlib_internal::hybrid_merge_sort_internal(beg, end, buf, cmp);

		stlib_internal::aligned_storage_delete(distance(beg, end) * sizeof(valueof), buf);
		return true;
	}
	return false;
}

namespace stlib_internal {
template<typename Itr1, typename Itr2>
void merge_internal(Itr1 beg1, Itr1 beg2, Itr1 end2, Itr2& begout) {
	Itr1 end1 = beg2;

	//go through both lists, build the sorted list
	for(; beg1 != end1 && beg2 != end2; ++begout)
		if(less_func(*beg2, *beg1)) {
			construct(*begout, std::move(*beg2));
			++beg2;
		} else {
			construct(*begout, std::move(*beg1));
			++beg1;
		}

	//finish off copy back of remaining lists (if any)
	copy_buffers(beg1, end1, begout);
	copy_buffers(beg2, end2, begout);
	return;
}
template<typename Itr, typename T>
void merge_sort_internal(Itr beg, Itr end, T* buf) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;
	T* bfrend = buf + sze;

	//go through all of the lengths starting at 1 doubling
	uint64_t len = 1;
	bool first = true;
	while(len < sze) {
		uint64_t pos = 0;
		//go through all of the sorted sublists, zip them together
		//which way are we copying this?
		if(first) {
			T* bufbeg = buf;
			//T* bufend = bfrend;
			while(pos + len < sze) {
				//make the two halves
				Itr cleft = beg + pos;
				Itr cright = cleft + len;
				Itr cend = (pos + (len * 2) > sze ? end : cleft + (len * 2));

				//do merge
				merge_internal(cleft, cright, cend, bufbeg);
				pos += (len * 2);
			}
			//move the rest of the buffer across (needed as we swap buffers)
			if(pos < sze) {
				Itr cleft = beg + pos;
				copy_buffers(cleft, end, bufbeg);
			}
		} else {
			Itr bufbeg = beg;
			//Itr bufend = end;
			while(pos + len < sze) {
				//make the two halves
				T* cleft = buf + pos;
				T* cright = cleft + len;
				T* cend = (pos + (len * 2) > sze ? bfrend : cleft + (len * 2));

				//do merge
				merge_internal(cleft, cright, cend, bufbeg);
				pos += (len * 2);
			}
			//move the rest of the buffer across (needed as we swap buffers)
			if(pos < sze) {
				T* cleft = buf + pos;
				copy_buffers(cleft, bfrend, bufbeg);
			}
		}

		len *= 2;
		first = !first;
	}

	//ensure we copy this back at the original buffer if needed
	if(!first)
		copy_buffers(buf, bfrend, beg);
}
}
template<typename Itr>
bool merge_sort(Itr beg, Itr end) {
	if(distance(beg, end) <= 1)
		return true;
	using valueof = typename stlib::stlib_internal::value_for<Itr>::value_type;
	valueof* buf = (valueof*)stlib_internal::aligned_storage_new(distance(beg, end) * sizeof(valueof));
	if(buf) {
		stlib_internal::merge_sort_internal(beg, end, buf);

		stlib_internal::aligned_storage_delete(distance(beg, end) * sizeof(valueof), buf);
		return true;
	}
	return false;
}
namespace stlib_internal {
template<typename Itr, typename T>
void out_of_place_insertion_sort(Itr beg, Itr end, T* buf) {
	if(distance(beg, end) <= 1)
		return;

	//do an insertion sort but copying the array to a different destination
	T* lsted = buf;
	for(Itr strt = beg; strt != end; ++strt, ++lsted) {
		T* led = lsted;
		while(led != buf && greater_func(*(led - 1), *strt)) {
			construct(*led, std::move(*(led - 1)));
			--led;
		}
		construct(*led, std::move(*strt));
	}
}
template<typename Itr, typename T>
void out_of_place_multi_insertion_sort(Itr beg, Itr end, T* buf) {
	if(distance(beg, end) <= 1)
		return;

	//move this to the correct place (do insert)
	using valueof = typename stlib_internal::value_for<Itr>::value_type;
	constexpr unsigned itm_num = (((1024 / sizeof(valueof)) + (1024 % sizeof(valueof) ? 1 : 0)) < 10 ? ((1024 / sizeof(valueof)) + (1024 % sizeof(valueof) ? 1 : 0)) : 10);
	constexpr unsigned items = (itm_num < 5 ? 5 : itm_num);
	alignas(valueof) char item[sizeof(valueof) * items];
	valueof* arr = (valueof*)item;

	//move the results into the output buffer
	T* bufbg = buf;
	T* bufed = buf;
	construct(*buf, std::move(*beg));
	Itr strt = beg + 1;
	++bufed;
	do {
		//build up an auxiliary array with correctly ordered items
		bool test_consecutive = false;
		unsigned count = make_auxiliary_array(strt, end, arr, items, test_consecutive);
		//insert the items from the auxiliary array into the correct places in the main array
		multi_insert(bufbg, bufed + count, bufed, arr, count);
		//exit if we have reached the end
		strt = strt + count;
		bufed = bufed + count;
	} while(strt != end);
}
template<typename Itr, typename T>
void hybrid_merge_sort_internal(Itr beg, Itr end, T* buf) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;

	uint64_t insert_count = INSERTION_SORT_CUTOFF;
	bool out_of_place = start_out_of_place_merge(sze, insert_count);
	//sort small runs with insertion sort before doing merge
	if(out_of_place) {
		uint64_t len = insert_count;
		uint64_t count = 0;
		for(Itr bg = beg; bg != end; count+=len) {
			Itr ed = (count + len > sze ? end : bg + len);
			out_of_place_multi_insertion_sort(bg, ed, buf + count);
			bg = ed;
		}
	} else {
		uint64_t len = insert_count;
		uint64_t count = 0;
		for(Itr bg = beg; bg != end; count+=len) {
			Itr ed = (count + len > sze ? end : bg + len);
			multi_insertion_sort(bg, ed);
			bg = ed;
		}
	}
	if(sze <= insert_count)
		return;

	T* bfrend = buf + sze;
	//go through all of the lengths starting at 1 doubling
	uint64_t len = insert_count;
	while(len < sze) {
		uint64_t pos = 0;
		//go through all of the sorted sublists, zip them together
		//which way are we copying this?
		if(out_of_place) {
			Itr bufbeg = beg;
			Itr bufend = end;
			while(pos + len < sze) {
				//make the two halves
				T* cleft = buf + pos;
				T* cright = cleft + len;
				T* cend = (pos + (len * 2) > sze ? bfrend : cleft + (len * 2));

				//do merge
				merge_internal(cleft, cright, cend, bufbeg);
				pos += (len * 2);
			}
			//move the rest of the buffer across (needed as we swap buffers)
			if(pos < sze) {
				T* cleft = buf + pos;
				copy_buffers(cleft, bfrend, bufbeg);
			}
		} else {
			T* bufbeg = buf;
			T* bufend = bfrend;
			while(pos + len < sze) {
				//make the two halves
				Itr cleft = beg + pos;
				Itr cright = cleft + len;
				Itr cend = (pos + (len * 2) > sze ? end : cleft + (len * 2));

				//do merge
				merge_internal(cleft, cright, cend, bufbeg);
				pos += (len * 2);
			}
			//move the rest of the buffer across (needed as we swap buffers)
			if(pos < sze) {
				Itr cleft = beg + pos;
				copy_buffers(cleft, end, bufbeg);
			}
		}

		len *= 2;
		out_of_place = !out_of_place;
	}

	//ensure we copy this back at the original buffer if needed (should never happen because of start_out_of_place_merge)
	if(out_of_place)
		copy_buffers(buf, bfrend, beg);
}
}
template<typename Itr>
bool hybrid_merge_sort(Itr beg, Itr end) {
	if(distance(beg, end) <= 1)
		return true;
	using valueof = typename stlib::stlib_internal::value_for<Itr>::value_type;
	valueof* buf = (valueof*)stlib_internal::aligned_storage_new(distance(beg, end) * sizeof(valueof));
	if(buf) {
		stlib_internal::hybrid_merge_sort_internal(beg, end, buf);

		stlib_internal::aligned_storage_delete(distance(beg, end) * sizeof(valueof), buf);
		return true;
	}
	return false;
}


namespace stlib_internal {
template<typename Itr>
void inplace_merge(Itr beg1, Itr beg2, Itr end2) {
	//backwards inplace merge sort, is more efficient than forwards inplace merge sort
	Itr left = beg2 - 1;
	Itr right = end2 - 1;
	for(; left != beg1 - 1 && right != left; --right) {
		//get all of those that are greater, move them to the right
		Itr lft = left;
		Itr rght = right;
		for(; lft != beg1 - 1 && greater_func(*lft, *right); --lft, --rght);
		if(lft != left) {
			//move those greater left to the right hande side
			stlib_internal::rotate(lft + 1, left + 1, right + 1);
			left = lft;
		}
		right = rght;
	}
}
}
template<typename Itr>
void inplace_merge_sort(Itr beg, Itr end) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;

	//go through all of the lengths starting at 1 doubling
	uint64_t len = 1;
	while(len < sze) {
		uint64_t pos = 0;
		//go through all of the sorted sublists, merge them together
		while(pos + len < sze) {
			//make the two halves
			Itr cleft = beg + pos;
			Itr cright = cleft + len;
			Itr cend = (pos + (len * 2) > sze ? end : cleft + (len * 2));

			//do in-place merge
			stlib_internal::inplace_merge(cleft, cright, cend);
			pos += (len * 2);
		}
		len *= 2;
	}
}
template<typename Itr>
void hybrid_inplace_merge_sort(Itr beg, Itr end) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;
	//sort small runs with insertion sort before doing merge
	uint64_t insert_count = INSERTION_SORT_CUTOFF;
	{
		uint64_t len = insert_count;
		uint64_t count = 0;
		for(Itr bg = beg; bg != end; count+=len) {
			Itr ed = (count + len > sze ? end : bg + len);
			multi_insertion_sort(bg, ed);
			bg = ed;
		}
	}
	if(sze <= insert_count)
		return;

	//go through all of the lengths starting at 1 doubling
	uint64_t len = insert_count;
	while(len < sze) {
		uint64_t pos = 0;
		//go through all of the sorted sublists, merge them together
		while(pos + len < sze) {
			//make the two halves
			Itr cleft = beg + pos;
			Itr cright = cleft + len;
			Itr cend = (pos + (len * 2) > sze ? end : cleft + (len * 2));

			//do in-place merge
			stlib_internal::inplace_merge(cleft, cright, cend);
			pos += (len * 2);
		}
		len *= 2;
	}
}
namespace stlib_internal {
template<typename Itr, typename Comp>
void inplace_merge(Itr beg1, Itr beg2, Itr end2, Comp cmp) {
	//backwards inplace merge sort, is more efficient than forwards inplace merge sort
	Itr left = beg2 - 1;
	Itr right = end2 - 1;
	for(; left != beg1 - 1 && right != left; --right) {
		//get all of those that are greater, move them to the right
		Itr lft = left;
		Itr rght = right;
		for(; lft != beg1 - 1 && greater_func(*lft, *right, cmp); --lft, --rght);
		if(lft != left) {
			//move those greater left to the right hande side
			stlib_internal::rotate(lft + 1, left + 1, right + 1);
			left = lft;
		}
		right = rght;
	}
}
}
template<typename Itr, typename Comp>
void inplace_merge_sort(Itr beg, Itr end, Comp cmp) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;

	//go through all of the lengths starting at 1 doubling
	uint64_t len = 1;
	while(len < sze) {
		uint64_t pos = 0;
		//go through all of the sorted sublists, merge them together
		while(pos + len < sze) {
			//make the two halves
			Itr cleft = beg + pos;
			Itr cright = cleft + len;
			Itr cend = (pos + (len * 2) > sze ? end : cleft + (len * 2));

			//do in-place merge
			stlib_internal::inplace_merge(cleft, cright, cend, cmp);
			pos += (len * 2);
		}
		len *= 2;
	}
}
template<typename Itr, typename Comp>
void hybrid_inplace_merge_sort(Itr beg, Itr end, Comp cmp) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;
	//sort small runs with insertion sort before doing merge
	uint64_t insert_count = INSERTION_SORT_CUTOFF;
	{
		uint64_t len = insert_count;
		uint64_t count = 0;
		for(Itr bg = beg; bg != end; count+=len) {
			Itr ed = (count + len > sze ? end : bg + len);
			multi_insertion_sort(bg, ed, cmp);
			bg = ed;
		}
	}
	if(sze <= insert_count)
		return;

	//go through all of the lengths starting at 1 doubling
	uint64_t len = insert_count;
	while(len < sze) {
		uint64_t pos = 0;
		//go through all of the sorted sublists, merge them together
		while(pos + len < sze) {
			//make the two halves
			Itr cleft = beg + pos;
			Itr cright = cleft + len;
			Itr cend = (pos + (len * 2) > sze ? end : cleft + (len * 2));

			//do in-place merge
			stlib_internal::inplace_merge(cleft, cright, cend, cmp);
			pos += (len * 2);
		}
		len *= 2;
	}
}


namespace stlib_internal {
template<typename Itr>
void inner_rotate_merge(Itr beg1, Itr beg2, Itr end2, Itr& out) {
	//if size == 0 return
	uint64_t sze = distance(beg1, beg2);
	if(sze == 0)
		return;

	//search through the right for the position of the first from the left hand side
	Itr tmp;
	bool found = binary_search(beg2, end2, *beg1, tmp);
	if(found) {
		//if we are moving in an equal item, move to the left while we are looking at equal items (ensures stable ordering)
		--tmp;
		for(; tmp != (beg2 - 1) && equal_func(*beg1, *tmp); --tmp);
		++tmp;
	}
	uint64_t pos = distance(beg2, tmp);

	//rotate so that all of the left is in the correct position
	stlib_internal::rotate(beg1, beg2, tmp);
	out = beg1 + pos;

	if(sze == 1) return;

	//call rotate merge on top half of the left side
	Itr nbeg1_1 = out + sze / 2;
	Itr nbeg2_1 = out + sze;
	Itr nend2_1 = end2;
	Itr nout_1;
	inner_rotate_merge(nbeg1_1, nbeg2_1, nend2_1, nout_1);

	if(sze >= 2) {
		//call rotate merge on bottom half of the left side (also advance 1 forward as this is now in the correct place)
		Itr nbeg1_2 = out + 1;
		Itr nbeg2_2 = out + sze / 2;
		Itr nend2_2 = nout_1;
		Itr nout_2;
		inner_rotate_merge(nbeg1_2, nbeg2_2, nend2_2, nout_2);
	}
}
template<typename Itr>
void rotate_merge(Itr beg1, Itr beg2, Itr end2) {
	//half the left, do this before doing rotate merge (reduces initial rotate cost considerably)
	uint64_t sze = distance(beg1, beg2);
	if(sze == 0)
		return;

	Itr nout_1 = end2;
	while(sze > 0) {
		Itr nbeg1_1 = beg1 + sze / 2;
		Itr nbeg2_1 = beg1 + sze;
		Itr nend2_1 = nout_1;
		inner_rotate_merge(nbeg1_1, nbeg2_1, nend2_1, nout_1);
		sze /= 2;
	}
}
}
template<typename Itr>
void rotate_merge_sort(Itr beg, Itr end) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;

	//go through all of the lengths starting at 1 doubling
	uint64_t len = 1;
	while(len < sze) {
		uint64_t pos = 0;
		//go through all of the sorted sublists, merge them together
		while(pos + len < sze) {
			//make the two halves
			Itr cleft = beg + pos;
			Itr cright = cleft + len;
			Itr cend = (pos + (len * 2) > sze ? end : cleft + (len * 2));

			//do rotate merge
			stlib_internal::rotate_merge(cleft, cright, cend);
			pos += (len * 2);
		}
		len *= 2;
	}
}
template<typename Itr>
void hybrid_rotate_merge_sort(Itr beg, Itr end) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;
	//sort small runs with insertion sort before doing merge
	uint64_t insert_count = INSERTION_SORT_CUTOFF;
	{
		uint64_t len = insert_count;
		uint64_t count = 0;
		for(Itr bg = beg; bg != end; count+=len) {
			Itr ed = (count + len > sze ? end : bg + len);
			multi_insertion_sort(bg, ed);
			bg = ed;
		}
	}
	if(sze <= insert_count)
		return;

	//go through all of the lengths starting at 1 doubling
	uint64_t len = insert_count;
	while(len < sze) {
		uint64_t pos = 0;
		//go through all of the sorted sublists, merge them together
		while(pos + len < sze) {
			//make the two halves
			Itr cleft = beg + pos;
			Itr cright = cleft + len;
			Itr cend = (pos + (len * 2) > sze ? end : cleft + (len * 2));

			//do rotate merge
			stlib_internal::rotate_merge(cleft, cright, cend);
			pos += (len * 2);
		}
		len *= 2;
	}
}
namespace stlib_internal {
template<typename Itr, typename Comp>
void inner_rotate_merge(Itr beg1, Itr beg2, Itr end2, Comp cmp, Itr& out) {
	//if size == 0 return
	uint64_t sze = distance(beg1, beg2);
	if(sze == 0)
		return;

	//search through the right for the position of the first from the left hand side
	Itr tmp;
	bool found = binary_search(beg2, end2, *beg1, cmp, tmp);
	if(found) {
		//if we are moving in an equal item, move to the left while we are looking at equal items (ensures stable ordering)
		--tmp;
		for(; tmp != (beg2 - 1) && equal_func(*beg1, *tmp, cmp); --tmp);
		++tmp;
	}
	uint64_t pos = distance(beg2, tmp);

	//rotate so that all of the left is in the correct position
	stlib_internal::rotate(beg1, beg2, tmp);
	out = beg1 + pos;

	if(sze == 1) return;

	//call rotate merge on top half of the left side
	Itr nbeg1_1 = out + sze / 2;
	Itr nbeg2_1 = out + sze;
	Itr nend2_1 = end2;
	Itr nout_1;
	inner_rotate_merge(nbeg1_1, nbeg2_1, nend2_1, cmp, nout_1);

	if(sze >= 2) {
		//call rotate merge on bottom half of the left side (also advance 1 forward as this is now in the correct place)
		Itr nbeg1_2 = out + 1;
		Itr nbeg2_2 = out + sze / 2;
		Itr nend2_2 = nout_1;
		Itr nout_2;
		inner_rotate_merge(nbeg1_2, nbeg2_2, nend2_2, cmp, nout_2);
	}
}
template<typename Itr, typename Comp>
void rotate_merge(Itr beg1, Itr beg2, Itr end2, Comp cmp) {
	//half the left, do this before doing rotate merge (reduces initial rotate cost considerably)
	uint64_t sze = distance(beg1, beg2);
	if(sze == 0)
		return;

	Itr nout_1 = end2;
	while(sze > 0) {
		Itr nbeg1_1 = beg1 + sze / 2;
		Itr nbeg2_1 = beg1 + sze;
		Itr nend2_1 = nout_1;
		inner_rotate_merge(nbeg1_1, nbeg2_1, nend2_1, cmp, nout_1);
		sze /= 2;
	}
}
}
template<typename Itr, typename Comp>
void rotate_merge_sort(Itr beg, Itr end, Comp cmp) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;

	//go through all of the lengths starting at 1 doubling
	uint64_t len = 1;
	while(len < sze) {
		uint64_t pos = 0;
		//go through all of the sorted sublists, merge them together
		while(pos + len < sze) {
			//make the two halves
			Itr cleft = beg + pos;
			Itr cright = cleft + len;
			Itr cend = (pos + (len * 2) > sze ? end : cleft + (len * 2));

			//do rotate merge
			stlib_internal::rotate_merge(cleft, cright, cend, cmp);
			pos += (len * 2);
		}
		len *= 2;
	}
}
template<typename Itr, typename Comp>
void hybrid_rotate_merge_sort(Itr beg, Itr end, Comp cmp) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;
	//sort small runs with insertion sort before doing merge
	uint64_t insert_count = INSERTION_SORT_CUTOFF;
	{
		uint64_t len = insert_count;
		uint64_t count = 0;
		for(Itr bg = beg; bg != end; count+=len) {
			Itr ed = (count + len > sze ? end : bg + len);
			multi_insertion_sort(bg, ed, cmp);
			bg = ed;
		}
	}
	if(sze <= insert_count)
		return;

	//go through all of the lengths starting at 1 doubling
	uint64_t len = insert_count;
	while(len < sze) {
		uint64_t pos = 0;
		//go through all of the sorted sublists, merge them together
		while(pos + len < sze) {
			//make the two halves
			Itr cleft = beg + pos;
			Itr cright = cleft + len;
			Itr cend = (pos + (len * 2) > sze ? end : cleft + (len * 2));

			//do rotate merge
			stlib_internal::rotate_merge(cleft, cright, cend, cmp);
			pos += (len * 2);
		}
		len *= 2;
	}
}


namespace stlib_internal {
constexpr unsigned stackless_rotate_range_array_len = 24;
template<typename Itr>
struct stackless_range {
	Itr bg;
	Itr ed;
};
template<typename Itr>
bool should_stackless_rotate_merge(const stackless_range<Itr>& first, const stackless_range<Itr>& second) {
	//if this second distance is 7/8 the size of the first then merge is considered good
	auto dist1 = distance(first.bg, first.ed);
	auto dist2 = distance(second.bg, second.ed);

	dist1 = (float)dist1 * ((1.0f / 8.0f) * 7.0f);
	return dist2 >= dist1;
}
template<typename Itr>
bool first_best_merge(const stackless_range<Itr>& first, const stackless_range<Itr>& second, const stackless_range<Itr>& third) {
	//get sizes
	auto dist1 = distance(first.bg, first.ed);
	auto dist2 = distance(second.bg, second.ed);
	auto dist3 = distance(third.bg, third.ed);

	//which of the two is the most similar in length, merge the closest in size
	return labs(dist1 - dist2) <= labs(dist2 - dist3);
}
template<typename Itr>
void do_stackless_rotate_merge(stackless_range<Itr>* stk, const stackless_range<Itr>& first, const stackless_range<Itr>& second,
							   unsigned idx, unsigned& stack_pos) {
	//merge them
	rotate_merge(first.bg, second.bg, second.ed);

	//record the merge
	stk[idx].ed = stk[idx + 1].ed;
	++idx;
	for(; idx < (stack_pos - 1); ++idx)
		stk[idx] = stk[idx + 1];
	--stack_pos;
}
template<typename Itr>
void do_stackless_rotate_callapse(stackless_range<Itr>* stk, Itr beg, Itr end, unsigned& stack_pos) {
	while(stack_pos > 1)
		if(stack_pos >= 3) {
			//test the last two elements/second to last two elements against each other to see which we need to merge
			if(first_best_merge(stk[stack_pos - 3], stk[stack_pos - 2], stk[stack_pos - 1]))
				do_stackless_rotate_merge(stk, stk[stack_pos - 3], stk[stack_pos - 2],
										  stack_pos - 3, stack_pos);
			else
				do_stackless_rotate_merge(stk, stk[stack_pos - 2], stk[stack_pos - 1],
										  stack_pos - 2, stack_pos);
		} else
			do_stackless_rotate_merge(stk, stk[0], stk[1], 0, stack_pos);
}
template<typename Itr>
stackless_range<Itr> get_ascending_descending(Itr beg, Itr end, bool& ascending, bool& some_equal) {
	unsigned unknown_ascending = 0; //0 == unknown, 1 == ascending, 2 == descending
	Itr lst = beg;
	Itr tmp = beg;
	++tmp;

	for(; tmp != end; ++tmp) {
		if(some_equal == false && equal_func(*lst, *tmp)) {
			some_equal = true;
		} else if(less_func(*lst, *tmp)) {
			//this is less, must be ascending
			if(unknown_ascending == 2)
				break;
			unknown_ascending = 1;
		} else {
			//this is greater, must be descending
			if(unknown_ascending == 1)
				break;
			unknown_ascending = 2;
		}
		lst = tmp;
	}

	stackless_range<Itr> rtn = {beg, tmp};
	ascending = (unknown_ascending != 2);
	return rtn;
}
template<typename Itr>
Itr equal_run(Itr& beg, Itr end) {
	//find any equal runs, move beg forward so passing equal items by
	Itr strt = beg;
	Itr tmp = beg;
	++tmp;
	for(; tmp != end && equal_func(*beg, *tmp); ++tmp);
	beg = tmp;
	return strt;
}
template<typename Itr>
void do_stackless_rotate_identify(stackless_range<Itr>* stk, Itr beg, Itr end, unsigned& stack_pos) {
	bool ascending = true;
	bool some_equal = false;
	stackless_range<Itr> rslt = get_ascending_descending(beg, end, ascending, some_equal);

	//do reverse of this if we are not a
	if(!ascending) {
		reverse(rslt.bg, rslt.ed);
		//to ensure stable ordering we must reverse again on any equal elements
		if(some_equal) {
			//go through them re-reverse any that we need
			stackless_range<Itr> strt = rslt;
			while(strt.bg != strt.ed) {
				Itr it = equal_run(strt.bg, strt.ed);
				reverse(it, strt.bg);
			}
		}
	}

	stk[stack_pos] = rslt;
	++stack_pos;
}
template<typename Itr>
void stackless_rotate_merge_sort_internal(Itr beg, Itr end) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;

	//the central loop, decides what we need to do each iteration
	stackless_range<Itr> stk[stackless_rotate_range_array_len];
	unsigned stack_pos = 0;
	Itr bg = beg;
	while(stack_pos == 0 || !(stack_pos == 1 && stk[stack_pos - 1].ed == end))
		if(stack_pos == stackless_rotate_range_array_len || (stack_pos > 0 && stk[stack_pos - 1].ed == end))
			//should we collapse?
			//if so then collapse to a single complete sorted list
			do_stackless_rotate_callapse(stk, beg, end, stack_pos);
		else if(stack_pos >= 4 && should_stackless_rotate_merge(stk[stack_pos - 4], stk[stack_pos - 3]))
			//should we merge?
			//if so then do a merge of the top two lists then
			do_stackless_rotate_merge(stk, stk[stack_pos - 4], stk[stack_pos - 3], stack_pos - 4, stack_pos);
		else if(stack_pos >= 3 && should_stackless_rotate_merge(stk[stack_pos - 3], stk[stack_pos - 2]))
			//should we merge?
			//if so then do a merge of the top two lists then
			do_stackless_rotate_merge(stk, stk[stack_pos - 3], stk[stack_pos - 2], stack_pos - 3, stack_pos);
		else if(stack_pos >= 2 && should_stackless_rotate_merge(stk[stack_pos - 2], stk[stack_pos - 1]))
			//should we merge?
			//if so then do a merge of the top two lists then
			do_stackless_rotate_merge(stk, stk[stack_pos - 2], stk[stack_pos - 1], stack_pos - 2, stack_pos);
		else {
			//do identify to add something to the stack (reverse if this needs to be reversed)
			do_stackless_rotate_identify(stk, bg, end, stack_pos);
			bg = stk[stack_pos - 1].ed;
		}
}
}
template<typename Itr>
void stackless_rotate_merge_sort(Itr beg, Itr end) {
	stlib_internal::stackless_rotate_merge_sort_internal(beg, end);
}

namespace stlib_internal {
template<typename Itr, typename Comp>
void do_stackless_rotate_merge(stackless_range<Itr>* stk, const stackless_range<Itr>& first, const stackless_range<Itr>& second,
							   unsigned idx, unsigned& stack_pos, Comp cmp) {
	//merge them
	rotate_merge(first.bg, second.bg, second.ed, cmp);

	//record the merge
	stk[idx].ed = stk[idx + 1].ed;
	++idx;
	for(; idx < (stack_pos - 1); ++idx)
		stk[idx] = stk[idx + 1];
	--stack_pos;
}
template<typename Itr, typename Comp>
void do_stackless_rotate_callapse(stackless_range<Itr>* stk, Itr beg, Itr end, unsigned& stack_pos, Comp cmp) {
	while(stack_pos > 1)
		if(stack_pos >= 3) {
			//test the last two elements/second to last two elements against each other to see which we need to merge
			if(first_best_merge(stk[stack_pos - 3], stk[stack_pos - 2], stk[stack_pos - 1]))
				do_stackless_rotate_merge(stk, stk[stack_pos - 3], stk[stack_pos - 2],
										  stack_pos - 3, stack_pos, cmp);
			else
				do_stackless_rotate_merge(stk, stk[stack_pos - 2], stk[stack_pos - 1],
										  stack_pos - 2, stack_pos, cmp);
		} else
			do_stackless_rotate_merge(stk, stk[0], stk[1], 0, stack_pos, cmp);
}
template<typename Itr, typename Comp>
stackless_range<Itr> get_ascending_descending(Itr beg, Itr end, bool& ascending, bool& some_equal, Comp cmp) {
	unsigned unknown_ascending = 0; //0 == unknown, 1 == ascending, 2 == descending
	Itr lst = beg;
	Itr tmp = beg;
	++tmp;

	for(; tmp != end; ++tmp) {
		if(some_equal == false && equal_func(*lst, *tmp, cmp)) {
			some_equal = true;
		} else if(less_func(*lst, *tmp, cmp)) {
			//this is less, must be ascending
			if(unknown_ascending == 2)
				break;
			unknown_ascending = 1;
		} else {
			//this is greater, must be descending
			if(unknown_ascending == 1)
				break;
			unknown_ascending = 2;
		}
		lst = tmp;
	}

	stackless_range<Itr> rtn = {beg, tmp};
	ascending = (unknown_ascending != 2);
	return rtn;
}
template<typename Itr, typename Comp>
Itr equal_run(Itr& beg, Itr end, Comp cmp) {
	//find any equal runs, move beg forward so passing equal items by
	Itr strt = beg;
	Itr tmp = beg;
	++tmp;
	for(; tmp != end && equal_func(*beg, *tmp, cmp); ++tmp);
	beg = tmp;
	return strt;
}
template<typename Itr, typename Comp>
void do_stackless_rotate_identify(stackless_range<Itr>* stk, Itr beg, Itr end, unsigned& stack_pos, Comp cmp) {
	bool ascending = true;
	bool some_equal = false;
	stackless_range<Itr> rslt = get_ascending_descending(beg, end, ascending, some_equal, cmp);

	//do reverse of this if we are not a
	if(!ascending) {
		reverse(rslt.bg, rslt.ed);
		//to ensure stable ordering we must reverse again on any equal elements
		if(some_equal) {
			//go through them re-reverse any that we need
			stackless_range<Itr> strt = rslt;
			while(strt.bg != strt.ed) {
				Itr it = equal_run(strt.bg, strt.ed, cmp);
				reverse(it, strt.bg);
			}
		}
	}

	stk[stack_pos] = rslt;
	++stack_pos;
}
template<typename Itr, typename Comp>
void stackless_rotate_merge_sort_internal(Itr beg, Itr end, Comp cmp) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;

	//the central loop, decides what we need to do each iteration
	stackless_range<Itr> stk[stackless_rotate_range_array_len];
	unsigned stack_pos = 0;
	Itr bg = beg;
	while(stack_pos == 0 || !(stack_pos == 1 && stk[stack_pos - 1].ed == end))
		if(stack_pos == stackless_rotate_range_array_len || (stack_pos > 0 && stk[stack_pos - 1].ed == end))
			//should we collapse?
			//if so then collapse to a single complete sorted list
			do_stackless_rotate_callapse(stk, beg, end, stack_pos, cmp);
		else if(stack_pos >= 4 && should_stackless_rotate_merge(stk[stack_pos - 4], stk[stack_pos - 3]))
			//should we merge?
			//if so then do a merge of the top two lists then
			do_stackless_rotate_merge(stk, stk[stack_pos - 4], stk[stack_pos - 3], stack_pos - 4, stack_pos, cmp);
		else if(stack_pos >= 3 && should_stackless_rotate_merge(stk[stack_pos - 3], stk[stack_pos - 2]))
			//should we merge?
			//if so then do a merge of the top two lists then
			do_stackless_rotate_merge(stk, stk[stack_pos - 3], stk[stack_pos - 2], stack_pos - 3, stack_pos, cmp);
		else if(stack_pos >= 2 && should_stackless_rotate_merge(stk[stack_pos - 2], stk[stack_pos - 1]))
			//should we merge?
			//if so then do a merge of the top two lists then
			do_stackless_rotate_merge(stk, stk[stack_pos - 2], stk[stack_pos - 1], stack_pos - 2, stack_pos, cmp);
		else {
			//do identify to add something to the stack (reverse if this needs to be reversed)
			do_stackless_rotate_identify(stk, bg, end, stack_pos, cmp);
			bg = stk[stack_pos - 1].ed;
		}
}
}
template<typename Itr, typename Comp>
void stackless_rotate_merge_sort(Itr beg, Itr end, Comp cmp) {
	stlib_internal::stackless_rotate_merge_sort_internal(beg, end, cmp);
}


namespace stlib_internal {
template<typename Itr>
uint64_t get_last_size(Itr beg, Itr cbeg, uint64_t block_count) {
	uint64_t sze = distance(beg, cbeg);
	if(sze == 0)
		return 0;

	//this is the last fitting region
	uint64_t last_count = 0;
	do {
		//grow this to fill the area (as large as it can be)
		uint64_t tcount = block_count;
		while(tcount * 2 <= sze)
			tcount *= 2;
		last_count = tcount;
		sze -= tcount;
	} while(sze > 0);
	return last_count;
}
template<typename Itr>
void hybrid_stackless_rotate_merge_sort_internal(Itr beg, Itr end) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;
	//sort small runs with insertion sort before doing merge
	uint64_t insert_count = INSERTION_SORT_CUTOFF;
	{
		uint64_t len = insert_count;
		uint64_t count = 0;
		for(Itr bg = beg; bg != end; count+=len) {
			Itr ed = (count + len > sze ? end : bg + len);
			multi_insertion_sort(bg, ed);
			bg = ed;
		}
	}
	if(sze <= insert_count)
		return;

	//start this as the second block
	Itr cbeg = beg + insert_count;
	uint64_t current_count = (insert_count < sze - insert_count ? insert_count : sze - insert_count);
	uint64_t last_count = insert_count;

	while(current_count >= insert_count && cbeg != end) {
		//work out the last size that fits in the space, compare this, this ensures we only merge with blocks of the same size
		if(last_count == current_count) {
			//do merge
			rotate_merge(cbeg - last_count, cbeg, cbeg + current_count);

			current_count += last_count;
			cbeg -= last_count;
			last_count = get_last_size(beg, cbeg, insert_count);
		} else {
			//advance
			cbeg += current_count;
			last_count = current_count;
			current_count = (insert_count < distance(cbeg, end) ? insert_count : distance(cbeg, end));
		}
	}

	//this is the finish, do callapse
	if(cbeg == end) {
		//take a step "back"
		cbeg -= last_count;
		last_count = get_last_size(beg, cbeg, insert_count);
	}

	while(cbeg != beg) {
		//do merge until we reach the beginning of the list again
		rotate_merge(cbeg - last_count, cbeg, end);
		cbeg -= last_count;
		last_count = get_last_size(beg, cbeg, insert_count);
	}
}
}
template<typename Itr>
void hybrid_stackless_rotate_merge_sort(Itr beg, Itr end) {
	stlib_internal::hybrid_stackless_rotate_merge_sort_internal(beg, end);
}

namespace stlib_internal {
template<typename Itr, typename Comp>
void hybrid_stackless_rotate_merge_sort_internal(Itr beg, Itr end, Comp cmp) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;
	//sort small runs with insertion sort before doing merge
	uint64_t insert_count = INSERTION_SORT_CUTOFF;
	{
		uint64_t len = insert_count;
		uint64_t count = 0;
		for(Itr bg = beg; bg != end; count+=len) {
			Itr ed = (count + len > sze ? end : bg + len);
			multi_insertion_sort(bg, ed, cmp);
			bg = ed;
		}
	}
	if(sze <= insert_count)
		return;

	//start this as the second block
	Itr cbeg = beg + insert_count;
	uint64_t current_count = (insert_count < sze - insert_count ? insert_count : sze - insert_count);
	uint64_t last_count = insert_count;

	while(current_count >= insert_count && cbeg != end) {
		//work out the last size that fits in the space, compare this, this ensures we only merge with blocks of the same size
		if(last_count == current_count) {
			//do merge
			rotate_merge(cbeg - last_count, cbeg, cbeg + current_count, cmp);

			current_count += last_count;
			cbeg -= last_count;
			last_count = get_last_size(beg, cbeg, insert_count);
		} else {
			//advance
			cbeg += current_count;
			last_count = current_count;
			current_count = (insert_count < distance(cbeg, end) ? insert_count : distance(cbeg, end));
		}
	}

	//this is the finish, do callapse
	if(cbeg == end) {
		//take a step "back"
		cbeg -= last_count;
		last_count = get_last_size(beg, cbeg, insert_count);
	}

	while(cbeg != beg) {
		//do merge until we reach the beginning of the list again
		rotate_merge(cbeg - last_count, cbeg, end, cmp);
		cbeg -= last_count;
		last_count = get_last_size(beg, cbeg, insert_count);
	}
}
}
template<typename Itr, typename Comp>
void hybrid_stackless_rotate_merge_sort(Itr beg, Itr end, Comp cmp) {
	stlib_internal::hybrid_stackless_rotate_merge_sort_internal(beg, end, cmp);
}



namespace stlib_internal {
//for use with stable intro sort (depth > maxdepth)
template<typename Itr, typename IdxItr>
void stable_inner_rotate_merge(Itr strt, Itr beg1, Itr beg2, Itr end2, IdxItr begidx, Itr& out) {
	//if size == 0 return
	uint64_t sze = distance(beg1, beg2);
	if(sze == 0)
		return;

	//search through the right for the position of the first from the left hand side
	Itr tmp;
	bool found = stable_binary_search(strt, beg2, end2, beg1, begidx, tmp);
	if(found) {
		//if we are moving in an equal item, move to the left while we are looking at equal items (ensures stable ordering)
		--tmp;
		for(; tmp != (beg2 - 1) && stable_quick_sort_equal_func(strt, beg1, tmp, begidx); --tmp);
		++tmp;
	}
	uint64_t pos = distance(beg2, tmp);

	//rotate so that all of the left is in the correct position
	stable_rotate(strt, beg1, beg2, tmp, begidx);
	out = beg1 + pos;

	if(sze == 1) return;

	//call rotate merge on top half of the left side
	Itr nbeg1_1 = out + sze / 2;
	Itr nbeg2_1 = out + sze;
	Itr nend2_1 = end2;
	Itr nout_1;
	stable_inner_rotate_merge(strt, nbeg1_1, nbeg2_1, nend2_1, begidx, nout_1);

	if(sze >= 2) {
		//call rotate merge on bottom half of the left side (also advance 1 forward as this is now in the correct place)
		Itr nbeg1_2 = out + 1;
		Itr nbeg2_2 = out + sze / 2;
		Itr nend2_2 = nout_1;
		Itr nout_2;
		stable_inner_rotate_merge(strt, nbeg1_2, nbeg2_2, nend2_2, begidx, nout_2);
	}
}
template<typename Itr, typename IdxItr>
void stable_rotate_merge(Itr strt, Itr beg1, Itr beg2, Itr end2, IdxItr begidx) {
	//half the left, do this before doing rotate merge (reduces initial rotate cost considerably)
	uint64_t sze = distance(beg1, beg2);
	if(sze == 0)
		return;

	Itr nout_1 = end2;
	while(sze > 0) {
		Itr nbeg1_1 = beg1 + sze / 2;
		Itr nbeg2_1 = beg1 + sze;
		Itr nend2_1 = nout_1;
		stable_inner_rotate_merge(strt, nbeg1_1, nbeg2_1, nend2_1, begidx, nout_1);
		sze /= 2;
	}
}
template<typename Itr, typename IdxItr>
void stable_rotate_merge_sort(Itr strt, Itr beg, Itr end, IdxItr begidx) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;

	//go through all of the lengths starting at 1 doubling
	uint64_t len = 1;
	while(len < sze) {
		uint64_t pos = 0;
		//go through all of the sorted sublists, merge them together
		while(pos + len < sze) {
			//make the two halves
			Itr cleft = beg + pos;
			Itr cright = cleft + len;
			Itr cend = (pos + (len * 2) > sze ? end : cleft + (len * 2));

			//do rotate merge
			stable_rotate_merge(strt, cleft, cright, cend, begidx);
			pos += (len * 2);
		}
		len *= 2;
	}
}


template<typename Itr, typename IdxItr, typename Comp>
void stable_inner_rotate_merge(Itr strt, Itr beg1, Itr beg2, Itr end2, IdxItr begidx, Comp cmp, Itr& out) {
	//if size == 0 return
	uint64_t sze = distance(beg1, beg2);
	if(sze == 0)
		return;

	//search through the right for the position of the first from the left hand side
	Itr tmp;
	bool found = stable_binary_search(strt, beg2, end2, beg1, begidx, cmp, tmp);
	if(found) {
		//if we are moving in an equal item, move to the left while we are looking at equal items (ensures stable ordering)
		--tmp;
		for(; tmp != (beg2 - 1) && stable_quick_sort_equal_func(strt, beg1, tmp, begidx, cmp); --tmp);
		++tmp;
	}
	uint64_t pos = distance(beg2, tmp);

	//rotate so that all of the left is in the correct position
	stable_rotate(strt, beg1, beg2, tmp, begidx);
	out = beg1 + pos;

	if(sze == 1) return;

	//call rotate merge on top half of the left side
	Itr nbeg1_1 = out + sze / 2;
	Itr nbeg2_1 = out + sze;
	Itr nend2_1 = end2;
	Itr nout_1;
	stable_inner_rotate_merge(strt, nbeg1_1, nbeg2_1, nend2_1, begidx, cmp, nout_1);

	if(sze >= 2) {
		//call rotate merge on bottom half of the left side (also advance 1 forward as this is now in the correct place)
		Itr nbeg1_2 = out + 1;
		Itr nbeg2_2 = out + sze / 2;
		Itr nend2_2 = nout_1;
		Itr nout_2;
		stable_inner_rotate_merge(strt, nbeg1_2, nbeg2_2, nend2_2, begidx, cmp, nout_2);
	}
}
template<typename Itr, typename IdxItr, typename Comp>
void stable_rotate_merge(Itr strt, Itr beg1, Itr beg2, Itr end2, IdxItr begidx, Comp cmp) {
	//half the left, do this before doing rotate merge (reduces initial rotate cost considerably)
	uint64_t sze = distance(beg1, beg2);
	if(sze == 0)
		return;

	Itr nout_1 = end2;
	while(sze > 0) {
		Itr nbeg1_1 = beg1 + sze / 2;
		Itr nbeg2_1 = beg1 + sze;
		Itr nend2_1 = nout_1;
		stable_inner_rotate_merge(strt, nbeg1_1, nbeg2_1, nend2_1, begidx, cmp, nout_1);
		sze /= 2;
	}
}
template<typename Itr, typename IdxItr, typename Comp>
void stable_rotate_merge_sort(Itr strt, Itr beg, Itr end, IdxItr begidx, Comp cmp) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;

	//go through all of the lengths starting at 1 doubling
	uint64_t len = 1;
	while(len < sze) {
		uint64_t pos = 0;
		//go through all of the sorted sublists, merge them together
		while(pos + len < sze) {
			//make the two halves
			Itr cleft = beg + pos;
			Itr cright = cleft + len;
			Itr cend = (pos + (len * 2) > sze ? end : cleft + (len * 2));

			//do rotate merge
			stable_rotate_merge(strt, cleft, cright, cend, begidx, cmp);
			pos += (len * 2);
		}
		len *= 2;
	}
}
}


namespace stlib_internal {
template<typename Itr>
struct zip_sort_stk_data {
	bool complete = false;
	Itr beg;
	Itr end;
};
template<typename Itr>
void zip_merge(Itr left, Itr right, Itr end) {
	//the swap buffer, uninitialised memory buffer
	using valueof = typename stlib_internal::value_for<Itr>::value_type;
	constexpr uint16_t buffer_count = (2048/sizeof(valueof) > 0 ? 2048/sizeof(valueof) : 1);
	alignas(valueof) char bufr[buffer_count * sizeof(valueof)];
	valueof* swapbufr = (valueof*)bufr;

	//all of the middle sections
	Itr mdlstart = right;
	Itr mdltop = right;
	uint64_t movecounttotal = 0;

	while((left != right) & (right != end)) {
		if(mdltop != right) {
			//if we have a middle section test the middle against the right - long run optimisation
			if(less_func(*right, *mdltop)) {
				if(mdltop != mdlstart) {
					//move in a buffers worth at a time
					uint16_t count = 0;
					do {
						construct(swapbufr[count++], std::move(*left));
						construct(*left, std::move(*right));
						++right;
						++left;
					} while(((count < buffer_count) & (left != mdlstart) & (right != end)) && less_func(*right, *mdltop));

					//move the new smallest into the correct place in the middle section
					//move the middle section right
					Itr mdlend = right;
					Itr mdl = mdlend - count;
					do {
						--mdlend;
						--mdl;
						construct(*mdlend, std::move(*mdl));
					} while(mdl != mdltop);

					//move in the new data
					for(uint16_t i = 0; i < count; ++i, ++mdltop)
						construct(*mdltop, std::move(swapbufr[i]));
					movecounttotal += count;

					if((count >= buffer_count) | (left == mdlstart) | (right == end))
						//if we exit because we reach the end of the input we can move across
						--left;
					else {
						//swap the middle with the left
						std::swap(*left, *mdltop);
						++mdltop;
						if(mdltop == right) {
							mdltop = mdlstart;
							movecounttotal = 0;
						}
					}
				} else {
					std::swap(*left, *right);
					++right;
				}
			} else {
				std::swap(*left, *mdltop);
				++mdltop;
				if(mdltop == right) {
					mdltop = mdlstart;
					movecounttotal = 0;
				}
			}
		} else {
			//test the right against the left
			if(less_func(*right, *left)) {
				std::swap(*left, *right);
				++right;
			}
		}

		++left;
		if(left == mdlstart) {
			//if the left reaches the middle, re-order the middle section so smallest first
			stlib_internal::rotate(mdlstart, mdltop, right);
			if(left == mdlstart) {
				mdlstart = right;
				mdltop = right;
			} else
				mdltop = mdlstart;

			movecounttotal = 0;
		}
	}

	if(left != right) {
		//if the right has reached the end before the left
		stlib_internal::rotate(mdlstart, mdltop, right);
		stlib_internal::rotate(left, mdlstart, right);
	}
}
}
template<typename Itr>
void zip_sort(Itr beg, Itr end) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;

	//go through all of the lengths starting at 2 doubling
	uint64_t len = 1;
	while(len < sze) {
		uint64_t pos = 0;
		//go through all of the sorted sublists, zip them together
		while(pos + len < sze) {
			//make the two halves
			Itr cleft = beg + pos;
			Itr cright = cleft + len;
			Itr cend = cleft + (len * 2);
			if(pos + (len * 2) > sze) cend = end;

			//do zip merge
			stlib_internal::zip_merge(cleft, cright, cend);
			pos += (len * 2);
		}
		len *= 2;
	}
}
template<typename Itr>
void hybrid_zip_sort(Itr beg, Itr end) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;
	//sort small runs with insertion sort before doing merge
	uint64_t insert_count = 16;
	{
		uint64_t len = insert_count;
		uint64_t count = 0;
		for(Itr bg = beg; bg != end; count+=len) {
			Itr ed = (count + len > sze ? end : bg + len);
			multi_insertion_sort(bg, ed);
			bg = ed;
		}
	}
	if(sze <= insert_count)
		return;

	//go through all of the lengths starting at 1 doubling
	uint64_t len = insert_count;
	while(len < sze) {
		uint64_t pos = 0;
		//go through all of the sorted sublists, zip them together
		while(pos + len < sze) {
			//make the two halves
			Itr cleft = beg + pos;
			Itr cright = cleft + len;
			Itr cend = (pos + (len * 2) > sze ? end : cleft + (len * 2));

			//do zip merge
			stlib_internal::zip_merge(cleft, cright, cend);
			pos += (len * 2);
		}
		len *= 2;
	}
}
template<typename Itr>
void zip_sort_rec(Itr beg, Itr end) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;

	sze /= 2;

	zip_sort_rec(beg, beg + sze);
	zip_sort_rec(beg + sze, end);

	stlib_internal::zip_merge(beg, beg + sze, end);
}
template<typename Itr>
void zip_sort_rec2(Itr beg, Itr end) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;

	size_t idx = 0;
	std::vector<stlib_internal::zip_sort_stk_data<Itr>> stk;
	stk.resize(20);
	stk[idx++] = stlib_internal::zip_sort_stk_data<Itr>{false, beg, end};

	while(idx > 0) {
		stlib_internal::zip_sort_stk_data<Itr>& tmp = stk[idx - 1];
		sze = distance(tmp.beg, tmp.end);
		if(sze < 2) {
			--idx;
			continue;
		}
		if(tmp.complete || sze <= 2) {
			stlib_internal::zip_merge(tmp.beg, tmp.beg + (sze/2), tmp.end);
			--idx;
		} else {
			//split this
			tmp.complete = true;

			if(idx == stk.size())
				stk.resize(stk.size() * 2);
			stk[idx++] = stlib_internal::zip_sort_stk_data<Itr>{false, tmp.beg + (sze/2), tmp.end};

			if(idx == stk.size())
				stk.resize(stk.size() * 2);
			stk[idx++] = stlib_internal::zip_sort_stk_data<Itr>{false, tmp.beg, tmp.beg + (sze/2)};
		}
	}
}


namespace stlib_internal {
template<typename Itr, typename Comp>
void zip_merge(Itr left, Itr right, Itr end, Comp cmp) {
	//the swap buffer, uninitialised memory buffer
	using valueof = typename stlib_internal::value_for<Itr>::value_type;
	constexpr uint16_t buffer_count = (2048/sizeof(valueof) > 0 ? 2048/sizeof(valueof) : 1);
	alignas(valueof) char bufr[buffer_count * sizeof(valueof)];
	valueof* swapbufr = (valueof*)bufr;

	//all of the middle sections
	Itr mdlstart = right;
	Itr mdltop = right;
	uint64_t movecounttotal = 0;

	while((left != right) & (right != end)) {
		if(mdltop != right) {
			//if we have a middle section test the middle against the right - long run optimisation
			if(less_func(*right, *mdltop, cmp)) {
				if(mdltop != mdlstart) {
					//move in a buffers worth at a time
					uint16_t count = 0;
					do {
						construct(swapbufr[count++], std::move(*left));
						construct(*left, std::move(*right));
						++right;
						++left;
					} while(((count < buffer_count) & (left != mdlstart) & (right != end)) && less_func(*right, *mdltop, cmp));

					//move the new smallest into the correct place in the middle section
					//move the middle section right
					Itr mdlend = right;
					Itr mdl = mdlend - count;
					do {
						--mdlend;
						--mdl;
						construct(*mdlend, std::move(*mdl));
					} while(mdl != mdltop);

					//move in the new data
					for(uint16_t i = 0; i < count; ++i, ++mdltop)
						construct(*mdltop, std::move(swapbufr[i]));
					movecounttotal += count;

					if((count >= buffer_count) | (left == mdlstart) | (right == end))
						//if we exit because we reach the end of the input we can move across
						--left;
					else {
						//swap the middle with the left
						std::swap(*left, *mdltop);
						++mdltop;
						if(mdltop == right) {
							mdltop = mdlstart;
							movecounttotal = 0;
						}
					}
				} else {
					std::swap(*left, *right);
					++right;
				}
			} else {
				std::swap(*left, *mdltop);
				++mdltop;
				if(mdltop == right) {
					mdltop = mdlstart;
					movecounttotal = 0;
				}
			}
		} else {
			//test the right against the left
			if(less_func(*right, *left, cmp)) {
				std::swap(*left, *right);
				++right;
			}
		}

		++left;
		if(left == mdlstart) {
			//if the left reaches the middle, re-order the middle section so smallest first
			stlib_internal::rotate(mdlstart, mdltop, right);
			if(left == mdlstart) {
				mdlstart = right;
				mdltop = right;
			} else
				mdltop = mdlstart;

			movecounttotal = 0;
		}
	}

	if(left != right) {
		//if the right has reached the end before the left
		stlib_internal::rotate(mdlstart, mdltop, right);
		stlib_internal::rotate(left, mdlstart, right);
	}
}
}
template<typename Itr, typename Comp>
void zip_sort(Itr beg, Itr end, Comp cmp) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;

	//go through all of the lengths starting at 2 doubling
	uint64_t len = 1;
	while(len < sze) {
		uint64_t pos = 0;
		//go through all of the sorted sublists, zip them together
		while(pos + len < sze) {
			//make the two halves
			Itr cleft = beg + pos;
			Itr cright = cleft + len;
			Itr cend = cleft + (len * 2);
			if(pos + (len * 2) > sze) cend = end;

			//do zip merge
			stlib_internal::zip_merge(cleft, cright, cend, cmp);
			pos += (len * 2);
		}
		len *= 2;
	}
}
template<typename Itr, typename Comp>
void hybrid_zip_sort(Itr beg, Itr end, Comp cmp) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;
	//sort small runs with insertion sort before doing merge
	uint64_t insert_count = 16;
	{
		uint64_t len = insert_count;
		uint64_t count = 0;
		for(Itr bg = beg; bg != end; count+=len) {
			Itr ed = (count + len > sze ? end : bg + len);
			multi_insertion_sort(bg, ed, cmp);
			bg = ed;
		}
	}
	if(sze <= insert_count)
		return;

	//go through all of the lengths starting at 1 doubling
	uint64_t len = insert_count;
	while(len < sze) {
		uint64_t pos = 0;
		//go through all of the sorted sublists, zip them together
		while(pos + len < sze) {
			//make the two halves
			Itr cleft = beg + pos;
			Itr cright = cleft + len;
			Itr cend = (pos + (len * 2) > sze ? end : cleft + (len * 2));

			//do zip merge
			stlib_internal::zip_merge(cleft, cright, cend, cmp);
			pos += (len * 2);
		}
		len *= 2;
	}
}
template<typename Itr, typename Comp>
void zip_sort_rec(Itr beg, Itr end, Comp cmp) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;

	sze /= 2;

	zip_sort_rec(beg, beg + sze, cmp);
	zip_sort_rec(beg + sze, end, cmp);

	stlib_internal::zip_merge(beg, beg + sze, end, cmp);
}

template<typename Itr, typename Comp>
void zip_sort_rec2(Itr beg, Itr end, Comp cmp) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;

	size_t idx = 0;
	std::vector<stlib_internal::zip_sort_stk_data<Itr>> stk;
	stk.resize(20);
	stk[idx++] = stlib_internal::zip_sort_stk_data<Itr>{false, beg, end};

	while(idx > 0) {
		stlib_internal::zip_sort_stk_data<Itr>& tmp = stk[idx - 1];
		sze = distance(tmp.beg, tmp.end);
		if(sze < 2) {
			--idx;
			continue;
		}
		if(tmp.complete || sze <= 2) {
			stlib_internal::zip_merge(tmp.beg, tmp.beg + (sze/2), tmp.end, cmp);
			--idx;
		} else {
			//split this
			tmp.complete = true;

			if(idx == stk.size())
				stk.resize(stk.size() * 2);
			stk[idx++] = stlib_internal::zip_sort_stk_data<Itr>{false, tmp.beg + (sze/2), tmp.end};

			if(idx == stk.size())
				stk.resize(stk.size() * 2);
			stk[idx++] = stlib_internal::zip_sort_stk_data<Itr>{false, tmp.beg, tmp.beg + (sze/2)};
		}
	}
}



enum class NEW_ZIP_MERGE_KIND : uint8_t {
	NZMK_RECURSIVE,
	//possibly slightly faster than above
	NZMK_ROTATE_MERGE,
	//slower than both of the above options, gives constant memory usage
	NZMK_INPLACE_MERGE,
	//calls older zip_merge, gives constant memory usage
	NZMK_ZIP_MERGE
};
namespace stlib_internal {
template<typename Itr>
void inplace_merge(Itr beg1, Itr beg2, Itr end2);
template<typename Itr, typename Comp>
void inplace_merge(Itr beg1, Itr beg2, Itr end2, Comp cmp);
template<typename Itr>
void rotate_merge(Itr beg1, Itr beg2, Itr end2);
template<typename Itr, typename Comp>
void rotate_merge(Itr beg1, Itr beg2, Itr end2, Comp cmp);
template<typename Itr>
void new_zip_merge(Itr left, Itr right, Itr end, NEW_ZIP_MERGE_KIND kind, bool stable, int max_move);
template<typename Itr, typename Comp>
void new_zip_merge(Itr left, Itr right, Itr end, Comp cmp, NEW_ZIP_MERGE_KIND kind, bool stable, int max_move);

struct zip_merge_indexes {
	unsigned index;
	unsigned count;
};
template<typename Itr>
struct zip_merge_section {
	Itr mdltop;
	Itr mdlstart;
};
template<typename Itr>
inline Itr get_end_iterator(zip_merge_section<Itr>* sections, unsigned idx, unsigned sec_pos, Itr right) {
	//the end of this section is marked by the start of the next
	if(idx == sec_pos - 1)
		return right;
	return sections[idx + 1].mdlstart;
}
zip_merge_indexes& get_current_indexes(zip_merge_indexes* indexes, unsigned indexes_end);
void pop_indexes_count(zip_merge_indexes* indexes,
					   unsigned& indexes_start);
void push_indexes_count(unsigned idx, unsigned count,
						zip_merge_indexes* indexes,
						unsigned& indexes_end);
unsigned calculate_index_total(unsigned indexes_start, unsigned indexes_end);
bool indexes_full(unsigned indexes_start, unsigned indexes_end);
template<typename Itr>
void new_zip_sort_do_merge(Itr left, Itr right, Itr end, NEW_ZIP_MERGE_KIND kind, bool stable, int max_move) {
	switch(kind) {
	case NEW_ZIP_MERGE_KIND::NZMK_RECURSIVE:
		new_zip_merge(left, right, end, kind, stable, max_move);
		break;
	case NEW_ZIP_MERGE_KIND::NZMK_ROTATE_MERGE:
		rotate_merge(left, right, end);
		break;
	case NEW_ZIP_MERGE_KIND::NZMK_INPLACE_MERGE:
		stlib_internal::inplace_merge(left, right, end);
		break;
	default:
		zip_merge(left, right, end);
		break;
	}
}
template<typename Itr>
void reorder_middle_sections(zip_merge_section<Itr>* sections,
							 zip_merge_indexes* indexes,
							 Itr right,
							 unsigned& sec_pos,
							 unsigned& indexes_start,
							 unsigned& indexes_end,
							 NEW_ZIP_MERGE_KIND kind, bool stable, int max_move) {
	// we don't have any middle sections
	if(sec_pos == 0)
		return;

	// go through each section rotate them
	for(unsigned i = 0; i < sec_pos; ++i) {
		stlib::stlib_internal::rotate(sections[i].mdlstart, sections[i].mdltop, get_end_iterator(sections, i, sec_pos, right));

		// also update the top of this
		sections[i].mdltop = sections[i].mdlstart;
	}

	// then merge them together
	unsigned pos = 0;
	while((sec_pos - pos) > 1) {
		if((sec_pos - pos) == 2) {
			unsigned Aidx = pos;
			unsigned Bidx = pos + 1;

			// merge them together
			new_zip_sort_do_merge(sections[Aidx].mdlstart, sections[Bidx].mdlstart, get_end_iterator(sections, Bidx, sec_pos, right), kind, stable, max_move);

			// expand this
			sections[Bidx].mdlstart = sections[Aidx].mdlstart;
			sections[Bidx].mdltop = sections[Aidx].mdlstart;
			destruct(sections[Aidx]);
		} else if((sec_pos - pos) == 3) {
			// take the smaller difference in size of AB or BC
			unsigned Aidx = pos;
			unsigned Bidx = pos + 1;
			unsigned Cidx = pos + 2;

			ptrdiff_t distA = distance(sections[Aidx].mdlstart, get_end_iterator(sections, Aidx, sec_pos, right));
			ptrdiff_t distB = distance(sections[Bidx].mdlstart, get_end_iterator(sections, Bidx, sec_pos, right));
			ptrdiff_t distC = distance(sections[Cidx].mdlstart, get_end_iterator(sections, Cidx, sec_pos, right));

			if(abs(distA - distB) < abs(distB - distC)) {
				// merge A and B
				// merge them together
				new_zip_sort_do_merge(sections[Aidx].mdlstart, sections[Bidx].mdlstart, get_end_iterator(sections, Bidx, sec_pos, right), kind, stable, max_move);

				// expand this
				sections[Bidx].mdlstart = sections[Aidx].mdlstart;
				sections[Bidx].mdltop = sections[Aidx].mdlstart;
				destruct(sections[Aidx]);
			} else {
				// merge B and C
				// merge them together
				new_zip_sort_do_merge(sections[Bidx].mdlstart, sections[Cidx].mdlstart, get_end_iterator(sections, Cidx, sec_pos, right), kind, stable, max_move);

				// expand this
				sections[Cidx].mdlstart = sections[Bidx].mdlstart;
				sections[Cidx].mdltop = sections[Bidx].mdlstart;
				destruct(sections[Bidx]);

				// move one forward
				construct(sections[Bidx], std::move(sections[Aidx]));
			}
		} else {
			// there are 4 or more
			// take the smaller difference size of AB, BC or CD
			unsigned Aidx = pos;
			unsigned Bidx = pos + 1;
			unsigned Cidx = pos + 2;
			unsigned Didx = pos + 3;

			ptrdiff_t distA = distance(sections[Aidx].mdlstart, get_end_iterator(sections, Aidx, sec_pos, right));
			ptrdiff_t distB = distance(sections[Bidx].mdlstart, get_end_iterator(sections, Bidx, sec_pos, right));
			ptrdiff_t distC = distance(sections[Cidx].mdlstart, get_end_iterator(sections, Cidx, sec_pos, right));
			ptrdiff_t distD = distance(sections[Didx].mdlstart, get_end_iterator(sections, Didx, sec_pos, right));

			ptrdiff_t diffAB = abs(distA - distB);
			ptrdiff_t diffBC = abs(distB - distC);
			ptrdiff_t diffCD = abs(distC - distD);

			if(diffAB <= diffBC && diffAB <= diffCD) {
				// merge A and B
				// merge them together
				new_zip_sort_do_merge(sections[Aidx].mdlstart, sections[Bidx].mdlstart, get_end_iterator(sections, Bidx, sec_pos, right), kind, stable, max_move);

				// expand this
				sections[Bidx].mdlstart = sections[Aidx].mdlstart;
				sections[Bidx].mdltop = sections[Aidx].mdlstart;
				destruct(sections[Aidx]);
			} else if(diffBC <= diffCD) {
				// merge B and C
				// merge them together
				new_zip_sort_do_merge(sections[Bidx].mdlstart, sections[Cidx].mdlstart, get_end_iterator(sections, Cidx, sec_pos, right), kind, stable, max_move);

				// expand this
				sections[Cidx].mdlstart = sections[Bidx].mdlstart;
				sections[Cidx].mdltop = sections[Bidx].mdlstart;
				destruct(sections[Bidx]);

				// move one forward
				construct(sections[Bidx], std::move(sections[Aidx]));
			} else {
				// merge C and D
				// merge them together
				new_zip_sort_do_merge(sections[Cidx].mdlstart, sections[Didx].mdlstart, get_end_iterator(sections, Didx, sec_pos, right), kind, stable, max_move);

				// expand this
				sections[Didx].mdlstart = sections[Cidx].mdlstart;
				sections[Didx].mdltop = sections[Cidx].mdlstart;
				destruct(sections[Cidx]);

				// move one forward
				construct(sections[Cidx], std::move(sections[Bidx]));
				construct(sections[Bidx], std::move(sections[Aidx]));
			}
		}
		// move forward through the input
		++pos;
	}

	// move this to the front, there is only one ordered list now
	if(pos != 0)
		construct(sections[0], std::move(sections[pos]));

	//set control variables
	indexes[0].index = 0;
	indexes[0].count = distance(sections[0].mdlstart, right);
	indexes_start = 0;
	indexes_end = 1;
	sec_pos = 1;
}
template<typename Itr>
void new_zip_merge(Itr left, Itr right, Itr end, NEW_ZIP_MERGE_KIND kind, bool stable, int max_move) {
	using valueof = typename stlib::stlib_internal::value_for<Itr>::value_type;
	constexpr uint16_t buffer_count = (2048/sizeof(valueof) > 0 ? 2048/sizeof(valueof) : 1);
	alignas(valueof) char bufr[buffer_count * sizeof(valueof)];
	valueof* swapbufr = (valueof*)bufr;

	// data for all of the middle sections
	alignas(zip_merge_section<Itr>) char bufr2[sizeof(zip_merge_section<Itr>) * NEW_ZIP_SORT_ARRAY_SIZE];
	zip_merge_section<Itr>* sections = (zip_merge_section<Itr>*)bufr2;
	zip_merge_indexes indexes[NEW_ZIP_SORT_INDEX_ARRAY_SIZE];
	unsigned sec_pos = 0;
	unsigned indexes_start = 0;
	unsigned indexes_end = 0;
	Itr leftend = right;

	while((left != right) & (right != end)) {
		if(sec_pos > 0) {
			unsigned idx = indexes[indexes_start].index;
			if(less_func(*right, *sections[idx].mdltop)) {
				// right is less than the left
				// copy out the left, move in the right
				unsigned count = 0;
				construct(swapbufr[count++], std::move(*left));
				construct(*left, std::move(*right));

				Itr right_pos = right;
				if(sections[sec_pos - 1].mdlstart == sections[sec_pos - 1].mdltop) {
					// if possible add the right to the end section
					construct(*right, std::move(swapbufr[0]));

					//push count
					push_indexes_count(sec_pos - 1, 1,
									   indexes,
									   indexes_end);
				} else if(distance(sections[sec_pos - 1].mdltop, right) < max_move) {
					// small list optimisation
					// if possible to add to the end of last
					++right;
					++left;
					while(((count < buffer_count) & (left != leftend) & (right != end)) && less_func(*right, *sections[idx].mdltop)) {
						construct(swapbufr[count++], std::move(*left));
						construct(*left, std::move(*right));
						++right;
						++left;
					}

					//move the new smallest into the correct place in the middle section
					//move the middle section right
					Itr mdlend = right;
					Itr mdl = mdlend - count;
					do {
						--mdlend;
						--mdl;
						construct(*mdlend, std::move(*mdl));
					} while(mdl != sections[sec_pos - 1].mdltop);

					//move in the new data
					for(uint16_t i = 0; i < count; ++i, ++sections[sec_pos - 1].mdltop)
						construct(*sections[sec_pos - 1].mdltop, std::move(swapbufr[i]));
					right_pos = sections[sec_pos - 1].mdltop - 1;

					//push count
					push_indexes_count(sec_pos - 1, count,
									   indexes,
									   indexes_end);

					if(!((count == buffer_count) | (left == leftend) | (right == end))) {
						// left is less than the right
						// swap in the middle top
						std::swap(*left, *sections[idx].mdltop);
						++sections[idx].mdltop;
						if(sections[idx].mdltop == get_end_iterator(sections, idx, sec_pos, right))
							sections[idx].mdltop = sections[idx].mdlstart;
						if(idx == sec_pos - 1)
							++right_pos;

						//always push before pop to prevent invalid circular queue
						push_indexes_count(idx, 1,
										   indexes,
										   indexes_end);
						pop_indexes_count(indexes,
										  indexes_start);
					} else
						//go back one - accounts for future changes
						--left;
					--right;
				} else {
					construct(*right, std::move(swapbufr[0]));
					// create a new end section
					construct(sections[sec_pos].mdltop, right);
					construct(sections[sec_pos].mdlstart, right);

					indexes[indexes_end].index = sec_pos;
					indexes[indexes_end].count = 1;

					++sec_pos;
					++indexes_end;
					if(indexes_end == NEW_ZIP_SORT_INDEX_ARRAY_SIZE)
						indexes_end = 0;
				}

				// if this is stable we have things avaliable on the left and
				if(stable && distance(left, leftend) > 1 && equal_func(*(left + 1), *right_pos)) {
					Itr tmp = left + 2;
					unsigned count = 1;
					unsigned idx = sec_pos - 1;

					// how any of these are equal?
					for(; distance(tmp, leftend) > 0 && equal_func(*tmp, *right_pos); ++tmp, ++count);

					// we have the number of equal elements, rotate the elements to the correct positions (after right_pos)
					stlib::stlib_internal::rotate(left + 1, tmp, right_pos + 1);

					// push count on the indexes
					push_indexes_count(idx, count,
									   indexes,
									   indexes_end);

					// move all of the iterators left by count (accounts for rotation)
					sections[idx].mdlstart -= count;
					if(sections[idx].mdltop <= right_pos)
						sections[idx].mdltop -= count;

					while(idx > 0) {
						// go to the next one
						--idx;
						sections[idx].mdlstart -= count;
						sections[idx].mdltop -= count;
					}

					// move left end by count
					leftend -= count;
				}

				++right;
			} else {
				// left is less than the right
				// swap in the middle top
				std::swap(*left, *sections[idx].mdltop);
				++sections[idx].mdltop;
				if(sections[idx].mdltop == get_end_iterator(sections, idx, sec_pos, right))
					sections[idx].mdltop = sections[idx].mdlstart;

				//always push before pop to prevent invalid circular queue
				push_indexes_count(idx, 1,
								   indexes,
								   indexes_end);
				pop_indexes_count(indexes,
								  indexes_start);
			}
		} else if(less_func(*right, *left)) {
			std::swap(*left, *right);
			// create a new end section
			construct(sections[sec_pos].mdltop, right);
			construct(sections[sec_pos].mdlstart, right);

			indexes[indexes_end].index = sec_pos;
			indexes[indexes_end].count = 1;

			++sec_pos;
			++indexes_end;
			if(indexes_end == NEW_ZIP_SORT_INDEX_ARRAY_SIZE)
				indexes_end = 0;
			++right;
		}

		++left;
		if(left == leftend || sec_pos == NEW_ZIP_SORT_ARRAY_SIZE || indexes_full(indexes_start, indexes_end)) {
			// reorder the middle section into the new left
			reorder_middle_sections(sections, indexes, right, sec_pos, indexes_start, indexes_end, kind, stable, max_move);
			if(left == leftend) {
				// zero this, middle section is now left
				sec_pos = 0;
				indexes_start = 0;
				indexes_end = 0;
				leftend = right;
				destruct(sections[0]);
			}
		}
	}

	if(left != right) {
		reorder_middle_sections(sections, indexes, right, sec_pos, indexes_start, indexes_end, kind, stable, max_move);
		stlib::stlib_internal::rotate(left, leftend, right);
	}
}
}
template<typename Itr>
void new_zip_sort(Itr beg, Itr end, NEW_ZIP_MERGE_KIND kind = NEW_ZIP_MERGE_KIND::NZMK_RECURSIVE, bool stable = true, int max_move = INSERTION_SORT_CUTOFF * 4) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;

	//go through all of the lengths starting at 1 doubling
	uint64_t len = 1;
	while(len < sze) {
		uint64_t pos = 0;
		//go through all of the sorted sublists, zip them together
		while(pos + len < sze) {
			//make the two halves
			Itr cleft = beg + pos;
			Itr cright = cleft + len;
			Itr cend = (pos + (len * 2) > sze ? end : cleft + (len * 2));

			//do zip merge
			stlib_internal::new_zip_merge(cleft, cright, cend, kind, stable, max_move);
			pos += (len * 2);
		}
		len *= 2;
	}
}
template<typename Itr>
void hybrid_new_zip_sort(Itr beg, Itr end, NEW_ZIP_MERGE_KIND kind = NEW_ZIP_MERGE_KIND::NZMK_RECURSIVE, bool stable = true, int max_move = INSERTION_SORT_CUTOFF * 4) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;
	//sort small runs with insertion sort before doing merge
	uint64_t insert_count = INSERTION_SORT_CUTOFF;
	{
		uint64_t len = insert_count;
		uint64_t count = 0;
		for(Itr bg = beg; bg != end; count+=len) {
			Itr ed = (count + len > sze ? end : bg + len);
			multi_insertion_sort(bg, ed);
			bg = ed;
		}
	}
	if(sze <= insert_count)
		return;

	//go through all of the lengths starting at 1 doubling
	uint64_t len = insert_count;
	while(len < sze) {
		uint64_t pos = 0;
		//go through all of the sorted sublists, zip them together
		while(pos + len < sze) {
			//make the two halves
			Itr cleft = beg + pos;
			Itr cright = cleft + len;
			Itr cend = (pos + (len * 2) > sze ? end : cleft + (len * 2));

			//do zip merge
			stlib_internal::new_zip_merge(cleft, cright, cend, kind, stable, max_move);
			pos += (len * 2);
		}
		len *= 2;
	}
}
template<typename Itr>
void new_zip_sort_rec(Itr beg, Itr end, NEW_ZIP_MERGE_KIND kind = NEW_ZIP_MERGE_KIND::NZMK_RECURSIVE, bool stable = true, int max_move = INSERTION_SORT_CUTOFF * 4) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;

	sze /= 2;

	new_zip_sort_rec(beg, beg + sze, kind, stable, max_move);
	new_zip_sort_rec(beg + sze, end, kind, stable, max_move);

	stlib_internal::new_zip_merge(beg, beg + sze, end, kind, stable, max_move);
}
template<typename Itr>
void new_zip_sort_rec2(Itr beg, Itr end, NEW_ZIP_MERGE_KIND kind = NEW_ZIP_MERGE_KIND::NZMK_RECURSIVE, bool stable = true, int max_move = INSERTION_SORT_CUTOFF * 4) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;

	size_t idx = 0;
	std::vector<stlib_internal::zip_sort_stk_data<Itr>> stk;
	stk.resize(20);
	stk[idx++] = stlib_internal::zip_sort_stk_data<Itr>{false, beg, end};

	while(idx > 0) {
		stlib_internal::zip_sort_stk_data<Itr>& tmp = stk[idx - 1];
		sze = distance(tmp.beg, tmp.end);
		if(sze < 2) {
			--idx;
			continue;
		}
		if(tmp.complete || sze <= 2) {
			stlib_internal::new_zip_merge(tmp.beg, tmp.beg + (sze/2), tmp.end, kind, stable, max_move);
			--idx;
		} else {
			//split this
			tmp.complete = true;

			if(idx == stk.size())
				stk.resize(stk.size() * 2);
			stk[idx++] = stlib_internal::zip_sort_stk_data<Itr>{false, tmp.beg + (sze/2), tmp.end};

			if(idx == stk.size())
				stk.resize(stk.size() * 2);
			stk[idx++] = stlib_internal::zip_sort_stk_data<Itr>{false, tmp.beg, tmp.beg + (sze/2)};
		}
	}
}

namespace stlib_internal {
template<typename Itr, typename Comp>
void new_zip_sort_do_merge(Itr left, Itr right, Itr end, Comp cmp, NEW_ZIP_MERGE_KIND kind, bool stable, int max_move) {
	switch(kind) {
	case NEW_ZIP_MERGE_KIND::NZMK_RECURSIVE:
		new_zip_merge(left, right, end, cmp, kind, stable, max_move);
		break;
	case NEW_ZIP_MERGE_KIND::NZMK_ROTATE_MERGE:
		rotate_merge(left, right, end, cmp);
		break;
	case NEW_ZIP_MERGE_KIND::NZMK_INPLACE_MERGE:
		stlib_internal::inplace_merge(left, right, end, cmp);
		break;
	default:
		zip_merge(left, right, end, cmp);
		break;
	}
}
template<typename Itr, typename Comp>
void reorder_middle_sections(zip_merge_section<Itr>* sections,
							 zip_merge_indexes* indexes,
							 Itr right,
							 unsigned& sec_pos,
							 unsigned& indexes_start,
							 unsigned& indexes_end,
							 Comp cmp, NEW_ZIP_MERGE_KIND kind, bool stable, int max_move) {
	// we don't have any middle sections
	if(sec_pos == 0)
		return;

	// go through each section rotate them
	for(unsigned i = 0; i < sec_pos; ++i) {
		stlib::stlib_internal::rotate(sections[i].mdlstart, sections[i].mdltop, get_end_iterator(sections, i, sec_pos, right));

		// also update the top of this
		sections[i].mdltop = sections[i].mdlstart;
	}

	// then merge them together
	unsigned pos = 0;
	while((sec_pos - pos) > 1) {
		if((sec_pos - pos) == 2) {
			unsigned Aidx = pos;
			unsigned Bidx = pos + 1;

			// merge them together
			new_zip_sort_do_merge(sections[Aidx].mdlstart, sections[Bidx].mdlstart, get_end_iterator(sections, Bidx, sec_pos, right), cmp, kind, stable, max_move);

			// expand this
			sections[Bidx].mdlstart = sections[Aidx].mdlstart;
			sections[Bidx].mdltop = sections[Aidx].mdlstart;
			destruct(sections[Aidx]);
		} else if((sec_pos - pos) == 3) {
			// take the smaller difference in size of AB or BC
			unsigned Aidx = pos;
			unsigned Bidx = pos + 1;
			unsigned Cidx = pos + 2;

			ptrdiff_t distA = distance(sections[Aidx].mdlstart, get_end_iterator(sections, Aidx, sec_pos, right));
			ptrdiff_t distB = distance(sections[Bidx].mdlstart, get_end_iterator(sections, Bidx, sec_pos, right));
			ptrdiff_t distC = distance(sections[Cidx].mdlstart, get_end_iterator(sections, Cidx, sec_pos, right));

			if(abs(distA - distB) < abs(distB - distC)) {
				// merge A and B
				// merge them together
				new_zip_sort_do_merge(sections[Aidx].mdlstart, sections[Bidx].mdlstart, get_end_iterator(sections, Bidx, sec_pos, right), cmp, kind, stable, max_move);

				// expand this
				sections[Bidx].mdlstart = sections[Aidx].mdlstart;
				sections[Bidx].mdltop = sections[Aidx].mdlstart;
				destruct(sections[Aidx]);
			} else {
				// merge B and C
				// merge them together
				new_zip_sort_do_merge(sections[Bidx].mdlstart, sections[Cidx].mdlstart, get_end_iterator(sections, Cidx, sec_pos, right), cmp, kind, stable, max_move);

				// expand this
				sections[Cidx].mdlstart = sections[Bidx].mdlstart;
				sections[Cidx].mdltop = sections[Bidx].mdlstart;
				destruct(sections[Bidx]);

				// move one forward
				construct(sections[Bidx], std::move(sections[Aidx]));
			}
		} else {
			// there are 4 or more
			// take the smaller difference size of AB, BC or CD
			unsigned Aidx = pos;
			unsigned Bidx = pos + 1;
			unsigned Cidx = pos + 2;
			unsigned Didx = pos + 3;

			ptrdiff_t distA = distance(sections[Aidx].mdlstart, get_end_iterator(sections, Aidx, sec_pos, right));
			ptrdiff_t distB = distance(sections[Bidx].mdlstart, get_end_iterator(sections, Bidx, sec_pos, right));
			ptrdiff_t distC = distance(sections[Cidx].mdlstart, get_end_iterator(sections, Cidx, sec_pos, right));
			ptrdiff_t distD = distance(sections[Didx].mdlstart, get_end_iterator(sections, Didx, sec_pos, right));

			ptrdiff_t diffAB = abs(distA - distB);
			ptrdiff_t diffBC = abs(distB - distC);
			ptrdiff_t diffCD = abs(distC - distD);

			if(diffAB <= diffBC && diffAB <= diffCD) {
				// merge A and B
				// merge them together
				new_zip_sort_do_merge(sections[Aidx].mdlstart, sections[Bidx].mdlstart, get_end_iterator(sections, Bidx, sec_pos, right), cmp, kind, stable, max_move);

				// expand this
				sections[Bidx].mdlstart = sections[Aidx].mdlstart;
				sections[Bidx].mdltop = sections[Aidx].mdlstart;
				destruct(sections[Aidx]);
			} else if(diffBC <= diffCD) {
				// merge B and C
				// merge them together
				new_zip_sort_do_merge(sections[Bidx].mdlstart, sections[Cidx].mdlstart, get_end_iterator(sections, Cidx, sec_pos, right), cmp, kind, stable, max_move);

				// expand this
				sections[Cidx].mdlstart = sections[Bidx].mdlstart;
				sections[Cidx].mdltop = sections[Bidx].mdlstart;
				destruct(sections[Bidx]);

				// move one forward
				construct(sections[Bidx], std::move(sections[Aidx]));
			} else {
				// merge C and D
				// merge them together
				new_zip_sort_do_merge(sections[Cidx].mdlstart, sections[Didx].mdlstart, get_end_iterator(sections, Didx, sec_pos, right), cmp, kind, stable, max_move);

				// expand this
				sections[Didx].mdlstart = sections[Cidx].mdlstart;
				sections[Didx].mdltop = sections[Cidx].mdlstart;
				destruct(sections[Cidx]);

				// move one forward
				construct(sections[Cidx], std::move(sections[Bidx]));
				construct(sections[Bidx], std::move(sections[Aidx]));
			}
		}
		// move forward through the input
		++pos;
	}

	// move this to the front, there is only one ordered list now
	if(pos != 0)
		construct(sections[0], std::move(sections[pos]));

	//set control variables
	indexes[0].index = 0;
	indexes[0].count = distance(sections[0].mdlstart, right);
	indexes_start = 0;
	indexes_end = 1;
	sec_pos = 1;
}
template<typename Itr, typename Comp>
void new_zip_merge(Itr left, Itr right, Itr end, Comp cmp, NEW_ZIP_MERGE_KIND kind, bool stable, int max_move) {
	using valueof = typename stlib::stlib_internal::value_for<Itr>::value_type;
	constexpr uint16_t buffer_count = (2048/sizeof(valueof) > 0 ? 2048/sizeof(valueof) : 1);
	alignas(valueof) char bufr[buffer_count * sizeof(valueof)];
	valueof* swapbufr = (valueof*)bufr;

	// data for all of the middle sections
	alignas(zip_merge_section<Itr>) char bufr2[sizeof(zip_merge_section<Itr>) * NEW_ZIP_SORT_ARRAY_SIZE];
	zip_merge_section<Itr>* sections = (zip_merge_section<Itr>*)bufr2;
	zip_merge_indexes indexes[NEW_ZIP_SORT_INDEX_ARRAY_SIZE];
	unsigned sec_pos = 0;
	unsigned indexes_start = 0;
	unsigned indexes_end = 0;
	Itr leftend = right;

	while((left != right) & (right != end)) {
		if(sec_pos > 0) {
			unsigned idx = indexes[indexes_start].index;
			if(less_func(*right, *sections[idx].mdltop, cmp)) {
				// right is less than the left
				// copy out the left, move in the right
				unsigned count = 0;
				construct(swapbufr[count++], std::move(*left));
				construct(*left, std::move(*right));

				Itr right_pos = right;
				if(sections[sec_pos - 1].mdlstart == sections[sec_pos - 1].mdltop) {
					// if possible add the right to the end section
					construct(*right, std::move(swapbufr[0]));

					//push count
					push_indexes_count(sec_pos - 1, 1,
									   indexes,
									   indexes_end);
				} else if(distance(sections[sec_pos - 1].mdltop, right) < max_move) {
					// small list optimisation
					// if possible to add to the end of last
					++right;
					++left;
					while(((count < buffer_count) & (left != leftend) & (right != end)) && less_func(*right, *sections[idx].mdltop, cmp)) {
						construct(swapbufr[count++], std::move(*left));
						construct(*left, std::move(*right));
						++right;
						++left;
					}

					//move the new smallest into the correct place in the middle section
					//move the middle section right
					Itr mdlend = right;
					Itr mdl = mdlend - count;
					do {
						--mdlend;
						--mdl;
						construct(*mdlend, std::move(*mdl));
					} while(mdl != sections[sec_pos - 1].mdltop);

					//move in the new data
					for(uint16_t i = 0; i < count; ++i, ++sections[sec_pos - 1].mdltop)
						construct(*sections[sec_pos - 1].mdltop, std::move(swapbufr[i]));
					right_pos = sections[sec_pos - 1].mdltop - 1;

					//push count
					push_indexes_count(sec_pos - 1, count,
									   indexes,
									   indexes_end);

					if(!((count == buffer_count) | (left == leftend) | (right == end))) {
						// left is less than the right
						// swap in the middle top
						std::swap(*left, *sections[idx].mdltop);
						++sections[idx].mdltop;
						if(sections[idx].mdltop == get_end_iterator(sections, idx, sec_pos, right))
							sections[idx].mdltop = sections[idx].mdlstart;
						if(idx == sec_pos - 1)
							++right_pos;

						//always push before pop to prevent invalid circular queue
						push_indexes_count(idx, 1,
										   indexes,
										   indexes_end);
						pop_indexes_count(indexes,
										  indexes_start);
					} else
						//go back one - accounts for future changes
						--left;
					--right;
				} else {
					construct(*right, std::move(swapbufr[0]));
					// create a new end section
					construct(sections[sec_pos].mdltop, right);
					construct(sections[sec_pos].mdlstart, right);

					indexes[indexes_end].index = sec_pos;
					indexes[indexes_end].count = 1;

					++sec_pos;
					++indexes_end;
					if(indexes_end == NEW_ZIP_SORT_INDEX_ARRAY_SIZE)
						indexes_end = 0;
				}

				// if this is stable we have things avaliable on the left and
				if(stable && distance(left, leftend) > 1 && equal_func(*(left + 1), *right_pos, cmp)) {
					Itr tmp = left + 2;
					unsigned count = 1;
					unsigned idx = sec_pos - 1;

					// how any of these are equal?
					for(; distance(tmp, leftend) > 0 && equal_func(*tmp, *right_pos, cmp); ++tmp, ++count);

					// we have the number of equal elements, rotate the elements to the correct positions (after right_pos)
					stlib::stlib_internal::rotate(left + 1, tmp, right_pos + 1);

					// push count on the indexes
					push_indexes_count(idx, count,
									   indexes,
									   indexes_end);

					// move all of the iterators left by count (accounts for rotation)
					sections[idx].mdlstart -= count;
					if(sections[idx].mdltop <= right_pos)
						sections[idx].mdltop -= count;

					while(idx > 0) {
						// go to the next one
						--idx;
						sections[idx].mdlstart -= count;
						sections[idx].mdltop -= count;
					}

					// move left end by count
					leftend -= count;
				}

				++right;
			} else {
				// left is less than the right
				// swap in the middle top
				std::swap(*left, *sections[idx].mdltop);
				++sections[idx].mdltop;
				if(sections[idx].mdltop == get_end_iterator(sections, idx, sec_pos, right))
					sections[idx].mdltop = sections[idx].mdlstart;

				//always push before pop to prevent invalid circular queue
				push_indexes_count(idx, 1,
								   indexes,
								   indexes_end);
				pop_indexes_count(indexes,
								  indexes_start);
			}
		} else if(less_func(*right, *left, cmp)) {
			std::swap(*left, *right);
			// create a new end section
			construct(sections[sec_pos].mdltop, right);
			construct(sections[sec_pos].mdlstart, right);

			indexes[indexes_end].index = sec_pos;
			indexes[indexes_end].count = 1;

			++sec_pos;
			++indexes_end;
			if(indexes_end == NEW_ZIP_SORT_INDEX_ARRAY_SIZE)
				indexes_end = 0;
			++right;
		}

		++left;
		if(left == leftend || sec_pos == NEW_ZIP_SORT_ARRAY_SIZE || indexes_full(indexes_start, indexes_end)) {
			// reorder the middle section into the new left
			reorder_middle_sections(sections, indexes, right, sec_pos, indexes_start, indexes_end, cmp, kind, stable, max_move);
			if(left == leftend) {
				// zero this, middle section is now left
				sec_pos = 0;
				indexes_start = 0;
				indexes_end = 0;
				leftend = right;
				destruct(sections[0]);
			}
		}
	}

	if(left != right) {
		reorder_middle_sections(sections, indexes, right, sec_pos, indexes_start, indexes_end, cmp, kind, stable, max_move);
		stlib::stlib_internal::rotate(left, leftend, right);
	}
}
}
template<typename Itr, typename Comp>
void new_zip_sort(Itr beg, Itr end, Comp cmp, NEW_ZIP_MERGE_KIND kind = NEW_ZIP_MERGE_KIND::NZMK_RECURSIVE, bool stable = true, int max_move = INSERTION_SORT_CUTOFF * 4) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;

	//go through all of the lengths starting at 1 doubling
	uint64_t len = 1;
	while(len < sze) {
		uint64_t pos = 0;
		//go through all of the sorted sublists, zip them together
		while(pos + len < sze) {
			//make the two halves
			Itr cleft = beg + pos;
			Itr cright = cleft + len;
			Itr cend = (pos + (len * 2) > sze ? end : cleft + (len * 2));

			//do zip merge
			stlib_internal::new_zip_merge(cleft, cright, cend, cmp, kind, stable, max_move);
			pos += (len * 2);
		}
		len *= 2;
	}
}
template<typename Itr, typename Comp>
void hybrid_new_zip_sort(Itr beg, Itr end, Comp cmp, NEW_ZIP_MERGE_KIND kind = NEW_ZIP_MERGE_KIND::NZMK_RECURSIVE, bool stable = true, int max_move = INSERTION_SORT_CUTOFF * 4) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;
	//sort small runs with insertion sort before doing merge
	uint64_t insert_count = INSERTION_SORT_CUTOFF;
	{
		uint64_t len = insert_count;
		uint64_t count = 0;
		for(Itr bg = beg; bg != end; count+=len) {
			Itr ed = (count + len > sze ? end : bg + len);
			multi_insertion_sort(bg, ed, cmp);
			bg = ed;
		}
	}
	if(sze <= insert_count)
		return;

	//go through all of the lengths starting at 1 doubling
	uint64_t len = insert_count;
	while(len < sze) {
		uint64_t pos = 0;
		//go through all of the sorted sublists, zip them together
		while(pos + len < sze) {
			//make the two halves
			Itr cleft = beg + pos;
			Itr cright = cleft + len;
			Itr cend = (pos + (len * 2) > sze ? end : cleft + (len * 2));

			//do zip merge
			stlib_internal::new_zip_merge(cleft, cright, cend, cmp, kind, stable, max_move);
			pos += (len * 2);
		}
		len *= 2;
	}
}
template<typename Itr, typename Comp>
void new_zip_sort_rec(Itr beg, Itr end, Comp cmp, NEW_ZIP_MERGE_KIND kind = NEW_ZIP_MERGE_KIND::NZMK_RECURSIVE, bool stable = true, int max_move = INSERTION_SORT_CUTOFF * 4) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;

	sze /= 2;

	new_zip_sort_rec(beg, beg + sze, cmp, kind, stable, max_move);
	new_zip_sort_rec(beg + sze, end, cmp, kind, stable, max_move);

	stlib_internal::new_zip_merge(beg, beg + sze, end, cmp, kind, stable, max_move);
}

template<typename Itr, typename Comp>
void new_zip_sort_rec2(Itr beg, Itr end, Comp cmp, NEW_ZIP_MERGE_KIND kind = NEW_ZIP_MERGE_KIND::NZMK_RECURSIVE, bool stable = true, int max_move = INSERTION_SORT_CUTOFF * 4) {
	uint64_t sze = distance(beg, end);
	if(sze <= 1)
		return;

	size_t idx = 0;
	std::vector<stlib_internal::zip_sort_stk_data<Itr>> stk;
	stk.resize(20);
	stk[idx++] = stlib_internal::zip_sort_stk_data<Itr>{false, beg, end};

	while(idx > 0) {
		stlib_internal::zip_sort_stk_data<Itr>& tmp = stk[idx - 1];
		sze = distance(tmp.beg, tmp.end);
		if(sze < 2) {
			--idx;
			continue;
		}
		if(tmp.complete || sze <= 2) {
			stlib_internal::new_zip_merge(tmp.beg, tmp.beg + (sze/2), tmp.end, cmp, kind, stable, max_move);
			--idx;
		} else {
			//split this
			tmp.complete = true;

			if(idx == stk.size())
				stk.resize(stk.size() * 2);
			stk[idx++] = stlib_internal::zip_sort_stk_data<Itr>{false, tmp.beg + (sze/2), tmp.end};

			if(idx == stk.size())
				stk.resize(stk.size() * 2);
			stk[idx++] = stlib_internal::zip_sort_stk_data<Itr>{false, tmp.beg, tmp.beg + (sze/2)};
		}
	}
}



namespace stlib_internal {
template<typename Itr>
void intro_quick_sort_internal(Itr beg, Itr end) {
	if(distance(beg, end) <= 1)
		return;
	unsigned maxdepth = get_depth(distance(beg, end));

	//add a stack item
	std::vector<intro_stack_less_data<Itr>> stk;
	stk.resize(15);
	size_t idx = 0;
	intro_stack_less_data<Itr> dat = {
		beg,
		end - 1,
		maxdepth
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		intro_stack_less_data<Itr> tmp = stk[--idx];
		Itr left = tmp.beg - 1;
		Itr right = tmp.end + 1;
		Itr pivot = middle_of_three(tmp.beg, half_point(tmp.beg, tmp.end + 1), tmp.end);

		do {
			++left;
			--right;
			//pivot goes to the right!!
			while(left != right && left != pivot && less_func(*left, *pivot))
				++left;
			while(left != right && greater_equal_func(*right, *pivot))
				--right;
			if(left == right)
				break;

			std::swap(*left, *right);
			if(left == pivot)
				pivot = right;
		} while(left + 1 != right);

		//if right is on the less side, move back
		if(right != pivot) {
			if(less_func(*right, *pivot))
				++right;
			//move the pivot into place
			if(right != pivot) {
				std::swap(*right, *pivot);
				pivot = right;
			}
		}

		auto dist1 = distance(pivot + 1, tmp.end + 1);
		auto dist2 = distance(tmp.beg, pivot);
		//implements sort shorter first optimisation
		if(dist1 < dist2) {
			if(dist2 > INSERTION_SORT_CUTOFF)
				add_stack_item(tmp.beg, pivot, tmp.beg, pivot - 1, tmp.depth, stk, idx);
			if(dist1 > INSERTION_SORT_CUTOFF)
				add_stack_item(pivot + 1, tmp.end + 1, pivot + 1, tmp.end, tmp.depth, stk, idx);
		} else {
			if(dist1 > INSERTION_SORT_CUTOFF)
				add_stack_item(pivot + 1, tmp.end + 1, pivot + 1, tmp.end, tmp.depth, stk, idx);
			if(dist2 > INSERTION_SORT_CUTOFF)
				add_stack_item(tmp.beg, pivot, tmp.beg, pivot - 1, tmp.depth, stk, idx);
		}
	}
}
template<typename Itr, typename Comp>
void intro_quick_sort_internal(Itr beg, Itr end, Comp cmp) {
	if(distance(beg, end) <= 1)
		return;
	unsigned maxdepth = get_depth(distance(beg, end));

	//add a stack item
	std::vector<intro_stack_less_data<Itr>> stk;
	stk.resize(15);
	size_t idx = 0;
	intro_stack_less_data<Itr> dat = {
		beg,
		end - 1,
		maxdepth
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		intro_stack_less_data<Itr> tmp = stk[--idx];
		Itr left = tmp.beg - 1;
		Itr right = tmp.end + 1;
		Itr pivot = middle_of_three(tmp.beg, half_point(tmp.beg, tmp.end + 1), tmp.end, cmp);

		do {
			++left;
			--right;
			//pivot goes to the right!!
			while(left != right && left != pivot && less_func(*left, *pivot, cmp))
				++left;
			while(left != right && greater_equal_func(*right, *pivot, cmp))
				--right;
			if(left == right)
				break;

			std::swap(*left, *right);
			if(left == pivot)
				pivot = right;
		} while(left + 1 != right);

		//if right is on the less side, move back
		if(right != pivot) {
			if(less_func(*right, *pivot, cmp))
				++right;
			//move the pivot into place
			if(right != pivot) {
				std::swap(*right, *pivot);
				pivot = right;
			}
		}

		auto dist1 = distance(pivot + 1, tmp.end + 1);
		auto dist2 = distance(tmp.beg, pivot);
		//implements sort shorter first optimisation
		if(dist1 < dist2) {
			if(dist2 > INSERTION_SORT_CUTOFF)
				add_stack_item(tmp.beg, pivot, tmp.beg, pivot - 1, tmp.depth, stk, idx, cmp);
			if(dist1 > INSERTION_SORT_CUTOFF)
				add_stack_item(pivot + 1, tmp.end + 1, pivot + 1, tmp.end, tmp.depth, stk, idx, cmp);
		} else {
			if(dist1 > INSERTION_SORT_CUTOFF)
				add_stack_item(pivot + 1, tmp.end + 1, pivot + 1, tmp.end, tmp.depth, stk, idx, cmp);
			if(dist2 > INSERTION_SORT_CUTOFF)
				add_stack_item(tmp.beg, pivot, tmp.beg, pivot - 1, tmp.depth, stk, idx, cmp);
		}
	}
}
}

template<typename Itr>
void intro_quick_sort(Itr beg, Itr end) {
	stlib_internal::intro_quick_sort_internal(beg, end);

	multi_insertion_sort(beg, end);
}
template<typename Itr, typename Comp>
void intro_quick_sort(Itr beg, Itr end, Comp cmp) {
	stlib_internal::intro_quick_sort_internal(beg, end, cmp);

	multi_insertion_sort(beg, end, cmp);
}

namespace stlib_internal {
template<typename Itr>
void adaptive_intro_quick_sort(Itr beg, Itr end) {
	if(distance(beg, end) <= 1)
		return;
	unsigned maxdepth = get_depth(distance(beg, end));

	//add a stack item
	std::vector<intro_stack_less_data<Itr>> stk;
	stk.resize(15);
	size_t idx = 0;
	intro_stack_less_data<Itr> dat = {
		beg,
		end - 1,
		maxdepth
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		intro_stack_less_data<Itr> tmp = stk[--idx];
		Itr left = tmp.beg - 1;
		Itr right = tmp.end + 1;
		unsigned swaps = 0;
		Itr pivot = middle_of_four(tmp.beg, half_point(tmp.beg, tmp.end + 1), tmp.end);

		do {
			++left;
			--right;
			//pivot goes to the right!!
			while(left != right && left != pivot && less_func(*left, *pivot))
				++left;
			while(left != right && greater_equal_func(*right, *pivot))
				--right;
			if(left == right)
				break;

			std::swap(*left, *right);
			++swaps;
			if(left == pivot)
				pivot = right;
		} while(left + 1 != right);

		//if right is on the less side, move back
		if(right != pivot) {
			if(less_func(*right, *pivot))
				++right;
			//move the pivot into place
			if(right != pivot) {
				std::swap(*right, *pivot);
				++swaps;
				pivot = right;
			}
		}

		auto dist1 = distance(pivot + 1, tmp.end + 1);
		auto dist2 = distance(tmp.beg, pivot);
		if(swaps == 0 && ((dist1 > INSERTION_SORT_CUTOFF) | (dist2 > INSERTION_SORT_CUTOFF)) && stlib::is_sorted(tmp.beg, tmp.end + 1)) continue;

		//implements sort shorter first optimisation
		if(dist1 < dist2) {
			if(dist2 > INSERTION_SORT_CUTOFF)
				add_stack_item(tmp.beg, pivot, tmp.beg, pivot - 1, tmp.depth, stk, idx);
			if(dist1 > INSERTION_SORT_CUTOFF)
				add_stack_item(pivot + 1, tmp.end + 1, pivot + 1, tmp.end, tmp.depth, stk, idx);
		} else {
			if(dist1 > INSERTION_SORT_CUTOFF)
				add_stack_item(pivot + 1, tmp.end + 1, pivot + 1, tmp.end, tmp.depth, stk, idx);
			if(dist2 > INSERTION_SORT_CUTOFF)
				add_stack_item(tmp.beg, pivot, tmp.beg, pivot - 1, tmp.depth, stk, idx);
		}
	}
}
template<typename Itr, typename Comp>
void adaptive_intro_quick_sort(Itr beg, Itr end, Comp cmp) {
	if(distance(beg, end) <= 1)
		return;
	unsigned maxdepth = get_depth(distance(beg, end));

	//add a stack item
	std::vector<intro_stack_less_data<Itr>> stk;
	stk.resize(15);
	size_t idx = 0;
	intro_stack_less_data<Itr> dat = {
		beg,
		end - 1,
		maxdepth
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		intro_stack_less_data<Itr> tmp = stk[--idx];
		Itr left = tmp.beg - 1;
		Itr right = tmp.end + 1;
		unsigned swaps = 0;
		Itr pivot = middle_of_four(tmp.beg, half_point(tmp.beg, tmp.end + 1), tmp.end, cmp);

		do {
			++left;
			--right;
			//pivot goes to the right!!
			while(left != right && left != pivot && less_func(*left, *pivot, cmp))
				++left;
			while(left != right && greater_equal_func(*right, *pivot, cmp))
				--right;
			if(left == right)
				break;

			std::swap(*left, *right);
			++swaps;
			if(left == pivot)
				pivot = right;
		} while(left + 1 != right);

		//if right is on the less side, move back
		if(right != pivot) {
			if(less_func(*right, *pivot, cmp))
				++right;
			//move the pivot into place
			if(right != pivot) {
				std::swap(*right, *pivot);
				++swaps;
				pivot = right;
			}
		}

		auto dist1 = distance(pivot + 1, tmp.end + 1);
		auto dist2 = distance(tmp.beg, pivot);
		if(swaps == 0 && ((dist1 > INSERTION_SORT_CUTOFF) | (dist2 > INSERTION_SORT_CUTOFF)) && stlib::is_sorted(tmp.beg, tmp.end + 1, cmp)) continue;

		//implements sort shorter first optimisation
		if(dist1 < dist2) {
			if(dist2 > INSERTION_SORT_CUTOFF)
				add_stack_item(tmp.beg, pivot, tmp.beg, pivot - 1, tmp.depth, stk, idx, cmp);
			if(dist1 > INSERTION_SORT_CUTOFF)
				add_stack_item(pivot + 1, tmp.end + 1, pivot + 1, tmp.end, tmp.depth, stk, idx, cmp);
		} else {
			if(dist1 > INSERTION_SORT_CUTOFF)
				add_stack_item(pivot + 1, tmp.end + 1, pivot + 1, tmp.end, tmp.depth, stk, idx, cmp);
			if(dist2 > INSERTION_SORT_CUTOFF)
				add_stack_item(tmp.beg, pivot, tmp.beg, pivot - 1, tmp.depth, stk, idx, cmp);
		}
	}
}
}


template<typename Itr>
inline void intro_sort(Itr beg, Itr end) {
	stlib_internal::adaptive_intro_quick_sort(beg, end);

	multi_insertion_sort(beg, end);
}
template<typename Itr, typename Comp>
inline void intro_sort(Itr beg, Itr end, Comp cmp) {
	stlib_internal::adaptive_intro_quick_sort(beg, end, cmp);

	multi_insertion_sort(beg, end, cmp);
}

template<typename Itr>
inline void stable_sort(Itr beg, Itr end) {
	hybrid_merge_sort(beg, end);
}
template<typename Itr, typename Comp>
inline void stable_sort(Itr beg, Itr end, Comp cmp) {
	hybrid_merge_sort(beg, end, cmp);
}
template<typename Itr>
inline void sort(Itr beg, Itr end) {
	intro_sort(beg, end);
}
template<typename Itr, typename Comp>
inline void sort(Itr beg, Itr end, Comp cmp) {
	intro_sort(beg, end, cmp);
}

}



