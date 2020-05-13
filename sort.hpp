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
#include <vector>

namespace stlib {

namespace stlib_internal {
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
template<typename Itr1, typename Itr2>
void copy_buffers(Itr1 beg, Itr1 end, Itr2& out) {
	//found to be faster then memcpy!!!
	for(; beg != end; ++beg, ++out)
		construct(*out, std::move(*beg));
}

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
bool middle_of_four(Itr first, Itr middle, Itr last, Itr& out) {
	//do upfront comparison
	bool first_middle = less_func(*first, *middle);
	bool middle_first = less_func(*middle, *first);

	bool middle_last = less_func(*middle, *last);
	bool last_middle = less_func(*last, *middle);

	bool last_first = less_func(*last, *first);
	bool first_last = less_func(*first, *last);

	//if two of them are the same then choose the other one
	if(equal_func_bool(first_middle, middle_first) && !equal_func_bool(middle_last, last_middle)) {
		out = last;
		return true;
	}
	if(equal_func_bool(middle_last, last_middle) && !equal_func_bool(first_middle, middle_first)) {
		out = first;
		return true;
	}
	if(equal_func_bool(last_first, first_last) && !equal_func_bool(middle_last, last_middle)) {
		out = middle;
		return true;
	}

	//choose the middle one
	if(less_func_bool(middle_first)) {
		if(less_func_bool(last_middle)) {
			out = middle;
			return true;
		} else if(less_func_bool(last_first)) {
			out = last;
			return true;
		}
		if(equal_func_bool(first_middle, middle_first) && equal_func_bool(middle_last, last_middle)) {
			//break ties by selecting a new pivot - any pivot that is not equal to these
			//must be atleast 3 big
			if(first + 1 == last - 1)
				//just exit only three in the list, all equal!
				return false;
			for(Itr beg = first + 1; beg != last; ++beg)
				if(!equal_func(*first, *beg)) {
					out = beg;
					return true;
				}
			return false;
		}
		out = first;
		return true;
	} else {
		if(less_func_bool(middle_last)) {
			out = middle;
			return true;
		} else if(less_func_bool(last_first)) {
			out = first;
			return true;
		}
		if(equal_func_bool(first_middle, middle_first) && equal_func_bool(middle_last, last_middle)) {
			//break ties by selecting a new pivot - any pivot that is not equal to these
			//must be atleast 3 big
			if(first + 1 == last - 1)
				//just exit only three in the list, all equal!
				return false;
			for(Itr beg = first + 1; beg != last; ++beg)
				if(!equal_func(*first, *beg)) {
					out = beg;
					return true;
				}
			return false;
		}
		out = last;
		return true;
	}
}
template<typename Itr, typename Comp>
bool middle_of_four(Itr first, Itr middle, Itr last, Itr& out, Comp cmp) {
	//do upfront comparison
	bool first_middle = less_func(*first, *middle, cmp);
	bool middle_first = less_func(*middle, *first, cmp);

	bool middle_last = less_func(*middle, *last, cmp);
	bool last_middle = less_func(*last, *middle, cmp);

	bool last_first = less_func(*last, *first, cmp);
	bool first_last = less_func(*first, *last, cmp);

	//if two of them are the same then choose the other one
	if(equal_func_bool(first_middle, middle_first) && !equal_func_bool(middle_last, last_middle)) {
		out = last;
		return true;
	}
	if(equal_func_bool(middle_last, last_middle) && !equal_func_bool(first_middle, middle_first)) {
		out = first;
		return true;
	}
	if(equal_func_bool(last_first, first_last) && !equal_func_bool(middle_last, last_middle)) {
		out = middle;
		return true;
	}

	//choose the middle one
	if(less_func_bool(middle_first)) {
		if(less_func_bool(last_middle)) {
			out = middle;
			return true;
		} else if(less_func_bool(last_first)) {
			out = last;
			return true;
		}
		if(equal_func_bool(first_middle, middle_first) && equal_func_bool(middle_last, last_middle)) {
			//break ties by selecting a new pivot - any pivot that is not equal to these
			//must be atleast 3 big
			if(first + 1 == last - 1)
				//just exit only three in the list, all equal!
				return false;
			for(Itr beg = first + 1; beg != last; ++beg)
				if(!equal_func(*first, *beg, cmp)) {
					out = beg;
					return true;
				}
			return false;
		}
		out = first;
		return true;
	} else {
		if(less_func_bool(middle_last)) {
			out = middle;
			return true;
		} else if(less_func_bool(last_first)) {
			out = first;
			return true;
		}
		if(equal_func_bool(first_middle, middle_first) && equal_func_bool(middle_last, last_middle)) {
			//break ties by selecting a new pivot - any pivot that is not equal to these
			//must be atleast 3 big
			if(first + 1 == last - 1)
				//just exit only three in the list, all equal!
				return false;
			for(Itr beg = first + 1; beg != last; ++beg)
				if(!equal_func(*first, *beg, cmp)) {
					out = beg;
					return true;
				}
			return false;
		}
		out = last;
		return true;
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
			rotate(it + 1, it + (1 + greater_count), moveit);
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
			rotate(it + 1, it + (1 + greater_count), moveit);
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

template<typename Itr>
bool is_sorted(Itr beg, Itr end) {
	for(; beg != end - 1; ++beg)
		if(less_func(*(beg + 1), *beg))
			return false;
	return true;
}
template<typename Itr, typename Comp>
bool is_sorted(Itr beg, Itr end, Comp cmp) {
	for(; beg != end - 1; ++beg)
		if(less_func(*(beg + 1), *beg, cmp))
			return false;
	return true;
}
template<typename Itr>
bool is_reverse_sorted(Itr beg, Itr end) {
	for(; beg != end - 1; ++beg)
		if(greater_func(*(beg + 1), *beg))
			return false;
	return true;
}
template<typename Itr, typename Comp>
bool is_reverse_sorted(Itr beg, Itr end, Comp cmp) {
	for(; beg != end - 1; ++beg)
		if(greater_func(*(beg + 1), *beg, cmp))
			return false;
	return true;
}
	
template<typename Itr>
void bubble_sort(Itr beg, Itr end) {
	if(distance(beg, end) <= 1)
		return;
	bool sorted = false;
	Itr ed = end; --ed;
	while(!sorted) {
		sorted = true;
		for(Itr bg = beg; bg != ed; ++bg) {
			Itr nxt = bg;
			++nxt;
			if(stlib_internal::less_func(*nxt, *bg)) {
				std::swap(*bg, *nxt);
				sorted = false;
			}
		}
		--ed;
	}
}
template<typename Itr, typename Comp>
void bubble_sort(Itr beg, Itr end, Comp cmp) {
	if(distance(beg, end) <= 1)
		return;
	bool sorted = false;
	Itr ed = end; --ed;
	while(!sorted) {
		sorted = true;
		for(Itr bg = beg; bg != ed; ++bg) {
			Itr nxt = bg;
			++nxt;
			if(stlib_internal::less_func(*nxt, *bg, cmp)) {
				std::swap(*bg, *nxt);
				sorted = false;
			}
		}
		--ed;
	}
}


template<typename Itr>
void insertion_sort(Itr beg, Itr end) {
	if(distance(beg, end) <= 1)
		return;
	Itr strt = beg + 1;
	for(; strt != end; ++strt) {
		Itr crnt = strt;
		while(crnt != beg && stlib_internal::greater_func(*(crnt - 1), *crnt)) {
			std::swap(*crnt, *(crnt - 1));
			--crnt;
		}
	}
}
template<typename Itr, typename Comp>
void insertion_sort(Itr beg, Itr end, Comp cmp) {
	if(distance(beg, end) <= 1)
		return;
	Itr strt = beg + 1;
	for(; strt != end; ++strt) {
		Itr crnt = strt;
		while(crnt != beg && stlib_internal::greater_func(*(crnt - 1), *crnt, cmp)) {
			std::swap(*crnt, *(crnt - 1));
			--crnt;
		}
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

			for(Itr c = strt; c != out; --c)
				std::swap(*c, *(c - 1));
		} else if(out != strt) {
			for(Itr c = strt; c != out; --c)
				std::swap(*c, *(c - 1));
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

			for(Itr c = strt; c != out; --c)
				std::swap(*c, *(c - 1));
		} else if(out != strt) {
			for(Itr c = strt; c != out; --c)
				std::swap(*c, *(c - 1));
		}
	}
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
void stable_quick_sort_swap(Itr beg, Itr left, Itr right, IdxItr begidx) {
	size_t lidx = distance(beg, left);
	size_t ridx = distance(beg, right);

	std::swap(*left, *right);
	std::swap(*(begidx + lidx), *(begidx + ridx));
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
	size_t lidx = distance(beg, left);
	size_t ridx = distance(beg, right);
	if(*(begidx + lidx) < *(begidx + ridx))
		return true;
	return *(begidx + lidx) == *(begidx + ridx);
}
template<typename Itr, typename IdxItr>
bool stable_quick_sort_greater_equal_func(Itr beg, Itr left, Itr right, IdxItr begidx) {
	if(less_func(*left, *right))
		return false;
	if(greater_func(*left, *right))
		return true;
	size_t lidx = distance(beg, left);
	size_t ridx = distance(beg, right);
	if(*(begidx + lidx) > *(begidx + ridx))
		return true;
	return *(begidx + lidx) == *(begidx + ridx);
}
template<typename Itr, typename IdxItr>
bool stable_quick_sort_is_sorted(Itr start, Itr beg, Itr end, IdxItr begidx) {
	for(; beg != end - 1; ++beg)
		if(stable_quick_sort_less_func(start, (beg + 1), beg, begidx))
			return false;
	return true;
}
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
		Itr pivot;
		if(!middle_of_four(tmp.beg, half_point(tmp.beg, tmp.end + 1), tmp.end, pivot)) continue;

		//if there are no swaps then most likely already in order, just finish sorting
		unsigned swaps = 0;
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

		//this is already sorted, don't sort any more!
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
namespace stlib_internal {
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
	size_t lidx = distance(beg, left);
	size_t ridx = distance(beg, right);
	if(*(begidx + lidx) < *(begidx + ridx))
		return true;
	return *(begidx + lidx) == *(begidx + ridx);
}
template<typename Itr, typename IdxItr, typename Comp>
bool stable_quick_sort_greater_equal_func(Itr beg, Itr left, Itr right, IdxItr begidx, Comp cmp) {
	if(less_func(*left, *right, cmp))
		return false;
	if(greater_func(*left, *right, cmp))
		return true;
	size_t lidx = distance(beg, left);
	size_t ridx = distance(beg, right);
	if(*(begidx + lidx) > *(begidx + ridx))
		return true;
	return *(begidx + lidx) == *(begidx + ridx);
}
template<typename Itr, typename IdxItr, typename Comp>
bool stable_quick_sort_is_sorted(Itr start, Itr beg, Itr end, IdxItr begidx, Comp cmp) {
	for(; beg != end - 1; ++beg)
		if(stable_quick_sort_less_func(start, (beg + 1), beg, begidx, cmp))
			return false;
	return true;
}
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
		Itr pivot;
		if(!middle_of_four(tmp.beg, half_point(tmp.beg, tmp.end + 1), tmp.end, pivot, cmp)) continue;

		//if there are no swaps then most likely already in order, just finish sorting
		unsigned swaps = 0;
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

		//this is already sorted, don't sort any more!
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
bool stack_quick_sort(Itr beg, Itr end, Comp cmp, uint32_t limit = 100) {
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
		if(left != rend) rtn &= stack_quick_sort(left, end, limit - 1);
		if(beg != left) rtn &= stack_quick_sort(beg, left, limit - 1);
	} else {
		if(beg != left) rtn &= stack_quick_sort(beg, left, limit - 1);
		if(left != rend) rtn &= stack_quick_sort(left, end, limit - 1);
	}
	return rtn;
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
	rotate(lhs.nhalf, lhs.end, rhs.nhalf);

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
	rotate(nhalf1, half, nhalf2);
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
		merge_sweep_sort_iterative(pivot, item.beg, item.end, nhalf);

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
	rotate(nhalf1, half, nhalf2);
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
		merge_sweep_sort_iterative(pivot, item.beg, item.end, nhalf, cmp);

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
		} else {
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
namesapce stlib_internal {
template<typename Itr, typename T, typename Comp>
void hybrid_merge_sort_internal(Itr beg, Itr end, T* buf, Comp cmp) {
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
			insertion_sort(bg, ed, cmp);
			bg = ed;
		}
	}
	if(sze <= insert_count)
		return;

	T* bfrend = buf + sze;
	//go through all of the lengths starting at 1 doubling
	uint64_t len = insert_count;
	bool first = true;
	while(len < sze) {
		uint64_t pos = 0;
		//go through all of the sorted sublists, zip them together
		//which way are we copying this?
		if(first) {
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
		} else {
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

namesapce stlib_internal {
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
		} else {
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
namesapce stlib_internal {
template<typename Itr, typename T>
void hybrid_merge_sort_internal(Itr beg, Itr end, T* buf) {
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
			insertion_sort(bg, ed);
			bg = ed;
		}
	}
	if(sze <= insert_count)
		return;

	T* bfrend = buf + sze;
	//go through all of the lengths starting at 1 doubling
	uint64_t len = insert_count;
	bool first = true;
	while(len < sze) {
		uint64_t pos = 0;
		//go through all of the sorted sublists, zip them together
		//which way are we copying this?
		if(first) {
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
		} else {
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
		}

		len *= 2;
		first = !first;
	}

	//ensure we copy this back at the original buffer if needed
	//note that we calculate the number of iterations so this doesn't happen
	if(!first)
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
	//move all of the right that should be in the left, rotate those in the right to be in the left
	Itr bg1 = beg1;
	Itr bg2 = beg2;

	while(bg1 != bg2 && bg2 != end2) {
		//find any in the right that appear before those in the left (aka less than)
		Itr tmp1 = bg2;
		Itr tmp2 = bg1;
		for(; bg2 != end2 && less_func(*bg2, *bg1); ++bg2, ++tmp2);
		if(bg2 != tmp1) {
			rotate(bg1, tmp1, bg2);
			bg1 = tmp2;
		} else
			++bg1;
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
	uint64_t insert_count = 16;
	{
		uint64_t len = insert_count;
		uint64_t count = 0;
		for(Itr bg = beg; bg != end; count+=len) {
			Itr ed = (count + len > sze ? end : bg + len);
			insertion_sort(bg, ed);
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
	//move all of the right that should be in the left, rotate those in the right to be in the left
	Itr bg1 = beg1;
	Itr bg2 = beg2;

	while(bg1 != bg2 && bg2 != end2) {
		//find any in the right that appear before those in the left (aka less than)
		Itr tmp1 = bg2;
		Itr tmp2 = bg1;
		for(; bg2 != end2 && less_func(*bg2, *bg1, cmp); ++bg2, ++tmp2);
		if(bg2 != tmp1) {
			rotate(bg1, tmp1, bg2);
			bg1 = tmp2;
		} else
			++bg1;
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
	uint64_t insert_count = 16;
	{
		uint64_t len = insert_count;
		uint64_t count = 0;
		for(Itr bg = beg; bg != end; count+=len) {
			Itr ed = (count + len > sze ? end : bg + len);
			insertion_sort(bg, ed);
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
struct zip_sort_stk_data {
	bool complete = false;
	Itr beg;
	Itr end;
};
template<typename Itr>
void zip_merge(Itr left, Itr right, Itr end) {
	//the swap beffer, uninitialised memory buffer
	using valueof = typename stlib::stlib_internal::value_for<Itr>::value_type;
	constexpr uint16_t buffer_count = (2048/sizeof(valueof) > 0 ? 2048/sizeof(valueof) : 1);
	alignas(valueof) char bufr[buffer_count * sizeof(valueof)];
	valueof* swapbufr = (valueof*)bufr;

	//all of the middle sections
	Itr mdlstart = right;
	Itr mdltop = right;

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

					if((count >= buffer_count) | (left == mdlstart) | (right == end))
						//if we exit because we reach the end of the input we can move across
						--left;
					else {
						//swap the middle with the left
						std::swap(*left, *mdltop);
						++mdltop;
						if(mdltop == right)
							mdltop = mdlstart;
					}
				} else {
					std::swap(*left, *right);
					++right;
				}
			} else {
				std::swap(*left, *mdltop);
				++mdltop;
				if(mdltop == right)
					mdltop = mdlstart;
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
			rotate(mdlstart, mdltop, right);
			mdlstart = right;
			mdltop = right;
		}
	}

	if(left != right) {
		//if the right has reached the end before the left
		rotate(mdlstart, mdltop, right);
		rotate(left, mdlstart, right);
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
			insertion_sort(bg, ed);
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
	//the swap beffer, uninitialised memory buffer
	using valueof = typename stlib::stlib_internal::value_for<Itr>::value_type;
	constexpr uint16_t buffer_count = (2048/sizeof(valueof) > 0 ? 2048/sizeof(valueof) : 1);
	alignas(valueof) char bufr[buffer_count * sizeof(valueof)];
	valueof* swapbufr = (valueof*)bufr;

	//all of the middle sections
	Itr mdlstart = right;
	Itr mdltop = right;

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

					if((count >= buffer_count) | (left == mdlstart) | (right == end))
						//if we exit because we reach the end of the input we can move across
						--left;
					else {
						//swap the middle with the left
						std::swap(*left, *mdltop);
						++mdltop;
						if(mdltop == right)
							mdltop = mdlstart;
					}
				} else {
					std::swap(*left, *right);
					++right;
				}
			} else {
				std::swap(*left, *mdltop);
				++mdltop;
				if(mdltop == right)
					mdltop = mdlstart;
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
			rotate(mdlstart, mdltop, right);
			mdlstart = right;
			mdltop = right;
		}
	}

	if(left != right) {
		//if the right has reached the end before the left
		rotate(mdlstart, mdltop, right);
		rotate(left, mdlstart, right);
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
			insertion_sort(bg, ed, cmp);
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


namespace stlib_internal {
template<typename Itr>
struct intro_stack_less_data {
	Itr beg;
	Itr end;
	unsigned depth;
};
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
void add_stack_item(Itr beg1, Itr end1, Itr beg2, Itr end2, unsigned depth,
					std::vector<intro_stack_less_data<Itr>>& stk, size_t& idx) {
	if(depth == 1) {
		//do O(n log n) zip sort if we have reached the maximum depth
		zip_sort(beg1, end1);
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
		//do O(n log n) zip sort if we have reached the maximum depth
		zip_sort(beg1, end1, cmp);
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
}
template<typename Itr>
void intro_quick_sort(Itr beg, Itr end) {
	if(distance(beg, end) <= 1)
		return;
	unsigned maxdepth = stlib_internal::get_depth(distance(beg, end));

	//add a stack item
	std::vector<stlib_internal::intro_stack_less_data<Itr>> stk;
	stk.resize(15);
	size_t idx = 0;
	stlib_internal::intro_stack_less_data<Itr> dat = {
		beg,
		end - 1,
		maxdepth
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		stlib_internal::intro_stack_less_data<Itr> tmp = stk[--idx];
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
			if(dist2 > 32)
				stlib_internal::add_stack_item(tmp.beg, pivot, tmp.beg, pivot - 1, tmp.depth, stk, idx);
			if(dist1 > 32)
				stlib_internal::add_stack_item(pivot + 1, tmp.end + 1, pivot + 1, tmp.end, tmp.depth, stk, idx);
		} else {
			if(dist1 > 32)
				stlib_internal::add_stack_item(pivot + 1, tmp.end + 1, pivot + 1, tmp.end, tmp.depth, stk, idx);
			if(dist2 > 32)
				stlib_internal::add_stack_item(tmp.beg, pivot, tmp.beg, pivot - 1, tmp.depth, stk, idx);
		}
	}
}
template<typename Itr, typename Comp>
void intro_quick_sort(Itr beg, Itr end, Comp cmp) {
	if(distance(beg, end) <= 1)
		return;
	unsigned maxdepth = stlib_internal::get_depth(distance(beg, end));

	//add a stack item
	std::vector<stlib_internal::intro_stack_less_data<Itr>> stk;
	stk.resize(15);
	size_t idx = 0;
	stlib_internal::intro_stack_less_data<Itr> dat = {
		beg,
		end - 1,
		maxdepth
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		stlib_internal::intro_stack_less_data<Itr> tmp = stk[--idx];
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
			if(dist2 > 32)
				stlib_internal::add_stack_item(tmp.beg, pivot, tmp.beg, pivot - 1, tmp.depth, stk, idx, cmp);
			if(dist1 > 32)
				stlib_internal::add_stack_item(pivot + 1, tmp.end + 1, pivot + 1, tmp.end, tmp.depth, stk, idx, cmp);
		} else {
			if(dist1 > 32)
				stlib_internal::add_stack_item(pivot + 1, tmp.end + 1, pivot + 1, tmp.end, tmp.depth, stk, idx, cmp);
			if(dist2 > 32)
				stlib_internal::add_stack_item(tmp.beg, pivot, tmp.beg, pivot - 1, tmp.depth, stk, idx, cmp);
		}
	}
}


template<typename Itr>
void adaptive_intro_quick_sort(Itr beg, Itr end) {
	if(distance(beg, end) <= 1)
		return;
	unsigned maxdepth = stlib_internal::get_depth(distance(beg, end));

	//add a stack item
	std::vector<stlib_internal::intro_stack_less_data<Itr>> stk;
	stk.resize(15);
	size_t idx = 0;
	stlib_internal::intro_stack_less_data<Itr> dat = {
		beg,
		end - 1,
		maxdepth
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		stlib_internal::intro_stack_less_data<Itr> tmp = stk[--idx];
		Itr left = tmp.beg - 1;
		Itr right = tmp.end + 1;
		Itr pivot;
		if(!stlib_internal::middle_of_four(tmp.beg, stlib_internal::half_point(tmp.beg, tmp.end + 1), tmp.end, pivot)) continue;

		//if there are no swaps then most likely already in order, just finish sorting
		unsigned swaps = 0;
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
			++swaps;
			if(left == pivot)
				pivot = right;
		} while(left + 1 != right);

		//this is already sorted, don't sort any more!
		if(swaps == 0 && is_sorted(tmp.beg, tmp.end + 1)) continue;

		//if right is on the less side, move back
		if(right != pivot) {
			if(stlib_internal::less_func(*right, *pivot))
				++right;
			//move the pivot into place
			if(right != pivot) {
				std::swap(*right, *pivot);
				++swaps;
				pivot = right;
			}
		}

		//this is already sorted, don't sort any more!
		auto dist1 = distance(pivot + 1, tmp.end + 1);
		auto dist2 = distance(tmp.beg, pivot);
		if(swaps == 0 && (((dist1 > 32) | (dist2 > 32)) && is_sorted(tmp.beg, tmp.end + 1))) continue;

		//implements sort shorter first optimisation
		if(dist1 < dist2) {
			if(dist2 > 32)
				stlib_internal::add_stack_item(tmp.beg, pivot, tmp.beg, pivot - 1, tmp.depth, stk, idx);
			if(dist1 > 32)
				stlib_internal::add_stack_item(pivot + 1, tmp.end + 1, pivot + 1, tmp.end, tmp.depth, stk, idx);
		} else {
			if(dist1 > 32)
				stlib_internal::add_stack_item(pivot + 1, tmp.end + 1, pivot + 1, tmp.end, tmp.depth, stk, idx);
			if(dist2 > 32)
				stlib_internal::add_stack_item(tmp.beg, pivot, tmp.beg, pivot - 1, tmp.depth, stk, idx);
		}
	}
}
template<typename Itr, typename Comp>
void adaptive_intro_quick_sort(Itr beg, Itr end, Comp cmp) {
	if(distance(beg, end) <= 1)
		return;
	unsigned maxdepth = stlib_internal::get_depth(distance(beg, end));

	//add a stack item
	std::vector<stlib_internal::intro_stack_less_data<Itr>> stk;
	stk.resize(15);
	size_t idx = 0;
	istlib_internal::ntro_stack_less_data<Itr> dat = {
		beg,
		end - 1,
		maxdepth
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		stlib_internal::intro_stack_less_data<Itr> tmp = stk[--idx];
		Itr left = tmp.beg - 1;
		Itr right = tmp.end + 1;
		Itr pivot;
		if(!stlib_internal::middle_of_four(tmp.beg, stlib_internal::half_point(tmp.beg, tmp.end + 1), tmp.end, pivot, cmp)) continue;

		//if there are no swaps then most likely already in order, just finish sorting
		unsigned swaps = 0;
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
			++swaps;
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
				++swaps;
				pivot = right;
			}
		}

		//this is already sorted, don't sort any more!
		auto dist1 = distance(pivot + 1, tmp.end + 1);
		auto dist2 = distance(tmp.beg, pivot);
		if(swaps == 0 && ((dist1 > 32) | (dist2 > 32)) && is_sorted(tmp.beg, tmp.end + 1, cmp)) continue;

		//implements sort shorter first optimisation
		if(dist1 < dist2) {
			if(dist2 > 32)
				stlib_internal::add_stack_item(tmp.beg, pivot, tmp.beg, pivot - 1, tmp.depth, stk, idx, cmp);
			if(dist1 > 32)
				stlib_internal::add_stack_item(pivot + 1, tmp.end + 1, pivot + 1, tmp.end, tmp.depth, stk, idx, cmp);
		} else {
			if(dist1 > 32)
				stlib_internal::add_stack_item(pivot + 1, tmp.end + 1, pivot + 1, tmp.end, tmp.depth, stk, idx, cmp);
			if(dist2 > 32)
				stlib_internal::add_stack_item(tmp.beg, pivot, tmp.beg, pivot - 1, tmp.depth, stk, idx, cmp);
		}
	}
}


template<typename Itr>
inline void intro_sort(Itr beg, Itr end) {
	adaptive_intro_quick_sort(beg, end);

	insertion_sort(beg, end);
}
template<typename Itr, typename Comp>
inline void intro_sort(Itr beg, Itr end, Comp cmp) {
	adaptive_intro_quick_sort(beg, end, cmp);

	insertion_sort(beg, end, cmp);
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
