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

inline void* aligned_storage_new(size_t sze) {
	//use new to get some aligned data
	return new double[sze/sizeof(double) + (sze%sizeof(double) == 0 ? 0 : 1)];
}
inline void aligned_storage_delete(size_t sze, void* ptr) {
	delete[] (double*)ptr;
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
struct stack_less_data {
	Itr beg;
	Itr end;
};

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

namespace stlib_internal {
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
			stlib::rotate(it + 1, it + (1 + greater_count), moveit);
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
			stlib::rotate(it + 1, it + (1 + greater_count), moveit);
			moveit -= greater_count;
			pivot -= greater_count;

			greater_count = 0;
			if(it != nhalf - 1)
				--it;
		}
	}
}
}

template<typename Itr>
void quick_sort(Itr beg, Itr end) {
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

		if(distance(pivot + 1, tmp.end + 1) > 1) {
			stack_less_data<Itr> dat = {
				pivot + 1,
				tmp.end
			};
			stk[idx++] = std::move(dat);
			if(idx == stk.size())
				stk.resize(stk.size() * 2);
		}
		if(distance(tmp.beg, pivot) > 1) {
			stack_less_data<Itr> dat = {
				tmp.beg,
				pivot - 1
			};
			stk[idx++] = std::move(dat);
			if(idx == stk.size())
				stk.resize(stk.size() * 2);
		}
	}
}
template<typename Itr, typename Comp>
void quick_sort(Itr beg, Itr end, Comp cmp) {
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

		if(distance(pivot + 1, tmp.end + 1) > 1) {
			stack_less_data<Itr> dat = {
				pivot + 1,
				tmp.end
			};
			stk[idx++] = std::move(dat);
			if(idx == stk.size())
				stk.resize(stk.size() * 2);
		}
		if(distance(tmp.beg, pivot) > 1) {
			stack_less_data<Itr> dat = {
				tmp.beg,
				pivot - 1
			};
			stk[idx++] = std::move(dat);
			if(idx == stk.size())
				stk.resize(stk.size() * 2);
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
bool stable_quick_sort_greater_equal_func(Itr beg, Itr left, Itr right, IdxItr begidx) {
	if(less_func(*left, *right))
		return false;
	if(greater_func(*left, *right))
		return true;
	size_t lidx = distance(beg, left);
	size_t ridx = distance(beg, right);
	return *(begidx + lidx) == *(begidx + ridx);
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

		if(distance(pivot + 1, tmp.end + 1) > 1) {
			stack_less_data<Itr> dat = {
				pivot + 1,
				tmp.end
			};
			stk[idx++] = std::move(dat);
			if(idx == stk.size())
				stk.resize(stk.size() * 2);
		}
		if(distance(tmp.beg, pivot) > 1) {
			stack_less_data<Itr> dat = {
				tmp.beg,
				pivot - 1
			};
			stk[idx++] = std::move(dat);
			if(idx == stk.size())
				stk.resize(stk.size() * 2);
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
bool stable_quick_sort_greater_equal_func(Itr beg, Itr left, Itr right, IdxItr begidx, Comp cmp) {
	if(less_func(*left, *right, cmp))
		return false;
	if(greater_func(*left, *right, cmp))
		return true;
	size_t lidx = distance(beg, left);
	size_t ridx = distance(beg, right);
	return *(begidx + lidx) == *(begidx + ridx);
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

		if(distance(pivot + 1, tmp.end + 1) > 1) {
			stack_less_data<Itr> dat = {
				pivot + 1,
				tmp.end
			};
			stk[idx++] = std::move(dat);
			if(idx == stk.size())
				stk.resize(stk.size() * 2);
		}
		if(distance(tmp.beg, pivot) > 1) {
			stack_less_data<Itr> dat = {
				tmp.beg,
				pivot - 1
			};
			stk[idx++] = std::move(dat);
			if(idx == stk.size())
				stk.resize(stk.size() * 2);
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

	Itr pivot = middle_of_three(left, half_point(beg, end), right);
	while(left != right) {
		//all that are equal go to the left!!
		while(left != pivot && less_equal_func(*left, *pivot))
			++left;
		while(right != pivot && greater_func(*right, *pivot))
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
	if(beg != left) rtn &= stack_quick_sort(beg, left, limit - 1);
	if(left != rend) rtn &= stack_quick_sort(left, end, limit - 1);
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

	Itr pivot = middle_of_three(left, half_point(beg, end), right, cmp);
	while(left != right) {
		//all that are equal go to the left!!
		while(left != pivot && less_equal_func(*left, *pivot, cmp))
			++left;
		while(right != pivot && greater_func(*right, *pivot, cmp))
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
	if(beg != left) rtn &= stack_quick_sort(beg, left, cmp, limit - 1);
	if(left != rend) rtn &= stack_quick_sort(left, end, cmp, limit - 1);
	return rtn;
}

namespace stlib_internal {
template<typename Itr>
void sweep_sort_internal(Itr& pivot, Itr beg, Itr end) {
	//record to pivot + 1 for later use
	Itr pivotstore = pivot;
	Itr begleft = pivot;
	Itr begright = pivot + 1;

	//go forwards move those less than pivot to right side of pivot
	Itr moveright = begright;
	Itr tmpright = begright;
	for(; begright != end; ++begright) {
		if(less_func(*begright, *pivot)) {
			if(moveright != begright) {
				//swap the inbetween values that are equal to maintain stable orderering
				Itr moveequal = moveright + 1;
				for(; moveequal != begright; ++moveequal)
					if(equal_func(*moveequal, *moveright))
						std::swap(*moveequal, *moveright);
				std::swap(*moveright, *begright);
			}
			++moveright;
		}
	}
	//go backwards move those greater than pivot to left side of pivot
	Itr moveleft = begleft - 1;
	if(pivot != beg) {
		do {
			--begleft;
			if(greater_func(*begleft, *pivot)) {
				if(moveleft != begleft) {
					//swap the inbetween values that are equal to maintain stable orderering
					Itr moveequal = moveleft - 1;
					for(; moveequal != begleft; --moveequal)
						if(equal_func(*moveequal, *moveleft))
							std::swap(*moveequal, *moveleft);
					std::swap(*moveleft, *begleft);
				}
				--moveleft;
			}
		} while(begleft != (beg - 1));
	}

	//do two rotates here,
	//put the pivot in place (at beginning of left, could be at end of right)
	stlib::rotate(moveleft + 1, pivot, pivot + 1);
	pivot -= distance(moveleft + 1, pivot);

	//rotate the right side over
	stlib::rotate(moveleft + 1, tmpright, moveright);
	pivot += distance(tmpright, moveright);
}
}
template<typename Itr>
void stack_sweep_sort(Itr beg, Itr end) {
	if(distance(beg, end) <= 1)
		return;
	Itr pivot = half_point(beg, end);

	stlib_internal::sweep_sort_internal(pivot, beg, end);

	stack_sweep_sort(beg, pivot);
	stack_sweep_sort(pivot + 1, end);
}
template<typename Itr>
void sweep_sort(Itr beg, Itr end) {
	if(distance(beg, end) <= 1)
		return;
	std::vector<stack_less_data<Itr>> stk;
	stk.resize(15);
	size_t idx = 0;
	stack_less_data<Itr> dat = {
		beg,
		end
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		stack_less_data<Itr> item = stk[--idx];

		Itr pivot = half_point(item.beg, item.end);

		sweep_sort_internal(pivot, item.beg, item.end);

		if(distance(pivot + 1, item.end) > 1) {
			stack_less_data<Itr> dat = {
				pivot + 1,
				item.end
			};
			stk[idx++] = std::move(dat);
			if(idx == stk.size())
				stk.resize(stk.size() * 2);
		}
		if(distance(item.beg, pivot) > 1) {
			stack_less_data<Itr> dat = {
				item.beg,
				pivot
			};
			stk[idx++] = std::move(dat);
			if(idx == stk.size())
				stk.resize(stk.size() * 2);
		}
	}
}

namespace stlib_internal {
template<typename Itr, typename Comp>
void sweep_sort_internal(Itr& pivot, Itr beg, Itr end, Comp cmp) {
	//record to pivot + 1 for later use
	Itr pivotstore = pivot;
	Itr begleft = pivot;
	Itr begright = pivot + 1;

	//go forwards move those less than pivot to right side of pivot
	Itr moveright = begright;
	Itr tmpright = begright;
	for(; begright != end; ++begright) {
		if(less_func(*begright, *pivot, cmp)) {
			if(moveright != begright) {
				//swap the inbetween values that are equal to maintain stable orderering
				Itr moveequal = moveright + 1;
				for(; moveequal != begright; ++moveequal)
					if(equal_func(*moveequal, *moveright, cmp))
						std::swap(*moveequal, *moveright);
				std::swap(*moveright, *begright);
			}
			++moveright;
		}
	}
	//go backwards move those greater than pivot to left side of pivot
	Itr moveleft = begleft - 1;
	if(pivot != beg) {
		do {
			--begleft;
			if(greater_func(*begleft, *pivot, cmp)) {
				if(moveleft != begleft) {
					//swap the inbetween values that are equal to maintain stable orderering
					Itr moveequal = moveleft - 1;
					for(; moveequal != begleft; --moveequal)
						if(equal_func(*moveequal, *moveleft, cmp))
							std::swap(*moveequal, *moveleft);
					std::swap(*moveleft, *begleft);
				}
				--moveleft;
			}
		} while(begleft != (beg - 1));
	}

	//do two rotates here,
	//put the pivot in place (at beginning of left, could be at end of right)
	stlib::rotate(moveleft + 1, pivot, pivot + 1);
	pivot -= distance(moveleft + 1, pivot);

	//rotate the right side over
	stlib::rotate(moveleft + 1, tmpright, moveright);
	pivot += distance(tmpright, moveright);
}
}
template<typename Itr, typename Comp>
void stack_sweep_sort(Itr beg, Itr end, Comp cmp) {
	if(distance(beg, end) <= 1)
		return;
	Itr pivot = half_point(beg, end);

	stlib_internal::sweep_sort_internal(pivot, beg, end, cmp);

	stack_sweep_sort(beg, pivot);
	stack_sweep_sort(pivot + 1, end);
}
template<typename Itr, typename Comp>
void sweep_sort(Itr beg, Itr end, Comp cmp) {
	if(distance(beg, end) <= 1)
		return;
	std::vector<stack_less_data<Itr>> stk;
	stk.resize(15);
	size_t idx = 0;
	stack_less_data<Itr> dat = {
		beg,
		end
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		stack_less_data<Itr> item = stk[--idx];

		Itr pivot = half_point(item.beg, item.end);

		sweep_sort_internal(pivot, item.beg, item.end, cmp);

		if(distance(pivot + 1, item.end) > 1) {
			stack_less_data<Itr> dat = {
				pivot + 1,
				item.end
			};
			stk[idx++] = std::move(dat);
			if(idx == stk.size())
				stk.resize(stk.size() * 2);
		}
		if(distance(item.beg, pivot) > 1) {
			stack_less_data<Itr> dat = {
				item.beg,
				pivot
			};
			stk[idx++] = std::move(dat);
			if(idx == stk.size())
				stk.resize(stk.size() * 2);
		}
	}
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
	stdlib::rotate(lhs.nhalf, lhs.end, rhs.nhalf);

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
void merge_sweep_sort_add_item(Itr& bg, Itr end, Itr& pivot, vector<merge_sweep_stack_less_data<Itr>>& stk, size_t& idx) {
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
	vector<merge_sweep_stack_less_data<Itr>> stk;
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
	stlib::rotate(nhalf1, half, nhalf2);
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
	Itr pivot = half_point(beg, end);

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
	std::vector<stack_less_data<Itr>> stk;
	stk.resize(15);
	size_t idx = 0;
	stack_less_data<Itr> dat = {
		beg,
		end
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		stack_less_data<Itr> item = stk[--idx];

		Itr pivot = middle_of_three(item.beg, half_point(item.beg, item.end), item.end - 1);

		Itr nhalf;
		merge_sweep_sort_iterative(pivot, item.beg, item.end, nhalf);

		move_pivot(nhalf, pivot);

		if(distance(pivot + 1, item.end) > 1) {
			stack_less_data<Itr> dat = {
				pivot + 1,
				item.end
			};
			stk[idx++] = std::move(dat);
			if(idx == stk.size())
				stk.resize(stk.size() * 2);
		}
		if(distance(item.beg, nhalf) > 1) {
			stack_less_data<Itr> dat = {
				item.beg,
				nhalf
			};
			stk[idx++] = std::move(dat);
			if(idx == stk.size())
				stk.resize(stk.size() * 2);
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
void merge_sweep_sort_add_item(Itr& bg, Itr end, Itr& pivot, vector<merge_sweep_stack_less_data<Itr>>& stk, size_t& idx, Comp cmp) {
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
	vector<merge_sweep_stack_less_data<Itr>> stk;
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
	stlib::rotate(nhalf1, half, nhalf2);
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
	Itr pivot = half_point(beg, end);

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
	std::vector<stack_less_data<Itr>> stk;
	stk.resize(15);
	size_t idx = 0;
	stack_less_data<Itr> dat = {
		beg,
		end
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		stack_less_data<Itr> item = stk[--idx];

		Itr pivot = middle_of_three(item.beg, half_point(item.beg, item.end), item.end - 1, cmp);

		Itr nhalf;
		merge_sweep_sort_iterative(pivot, item.beg, item.end, nhalf, cmp);

		move_pivot(nhalf, pivot, cmp);

		if(distance(pivot + 1, item.end) > 1) {
			stack_less_data<Itr> dat = {
				pivot + 1,
				item.end
			};
			stk[idx++] = std::move(dat);
			if(idx == stk.size())
				stk.resize(stk.size() * 2);
		}
		if(distance(item.beg, nhalf) > 1) {
			stack_less_data<Itr> dat = {
				item.beg,
				nhalf
			};
			stk[idx++] = std::move(dat);
			if(idx == stk.size())
				stk.resize(stk.size() * 2);
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
	for(; beg1 != end1; ++beg1, ++begout)
		construct(*begout, std::move(*beg1));
	for(; beg2 != end2; ++beg2, ++begout)
		construct(*begout, std::move(*beg2));
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
		}

		len *= 2;
		first = !first;
	}

	//ensure we copy this back at the original buffer if needed
	if(!first)
		for(; buf != bfrend; ++buf, ++beg)
			construct(*beg, std::move(*buf));
}
}
template<typename Itr, typename Comp>
bool merge_sort(Itr beg, Itr end, Comp cmp) {
	if(distance(beg, end) <= 1)
		return true;
	using valueof = typename stlib::value_for<Itr>::value_type;
	valueof* buf = (valueof*)aligned_storage_new(distance(beg, end) * sizeof(valueof));
	if(buf) {
		stlib_internal::merge_sort_internal(beg, end, buf, cmp);

		aligned_storage_delete(distance(beg, end) * sizeof(valueof), buf);
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
	for(; beg1 != end1; ++beg1, ++begout)
		construct(*begout, std::move(*beg1));
	for(; beg2 != end2; ++beg2, ++begout)
		construct(*begout, std::move(*beg2));
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
		}

		len *= 2;
		first = !first;
	}

	//ensure we copy this back at the original buffer if needed
	if(!first)
		for(; buf != bfrend; ++buf, ++beg)
			construct(*beg, std::move(*buf));
}
}
template<typename Itr>
bool merge_sort(Itr beg, Itr end) {
	if(distance(beg, end) <= 1)
		return true;
	using valueof = typename stlib::value_for<Itr>::value_type;
	valueof* buf = (valueof*)aligned_storage_new(distance(beg, end) * sizeof(valueof));
	if(buf) {
		stlib_internal::merge_sort_internal(beg, end, buf);

		aligned_storage_delete(distance(beg, end) * sizeof(valueof), buf);
		return true;
	}
	return false;
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
	using valueof = typename stlib::value_for<Itr>::value_type;
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
						construct(*mdlend, *mdl);
					} while(mdl != mdltop);

					//move in the new data
					for(uint16_t i = 0; i < count; ++i, ++mdltop)
						construct(*mdltop, swapbufr[i]);
					--left;
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
			stlib::rotate(mdlstart, mdltop, right);
			mdlstart = right;
			mdltop = right;
		}
	}

	if(left != right) {
		//if the right has reached the end before the left
		stlib::rotate(mdlstart, mdltop, right);
		stlib::rotate(left, mdlstart, right);
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
	using valueof = typename stlib::value_for<Itr>::value_type;
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
						construct(*mdlend, *mdl);
					} while(mdl != mdltop);

					//move in the new data
					for(uint16_t i = 0; i < count; ++i, ++mdltop)
						construct(*mdltop, swapbufr[i]);
					--left;
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
			stlib::rotate(mdlstart, mdltop, right);
			mdlstart = right;
			mdltop = right;
		}
	}

	if(left != right) {
		//if the right has reached the end before the left
		stlib::rotate(mdlstart, mdltop, right);
		stlib::rotate(left, mdlstart, right);
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
			if(less_func(*nxt, *bg)) {
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
			if(less_func(*nxt, *bg, cmp)) {
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
		while(crnt != beg && greater_func(*(crnt - 1), *crnt)) {
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
		while(crnt != beg && greater_func(*(crnt - 1), *crnt, cmp)) {
			std::swap(*crnt, *(crnt - 1));
			--crnt;
		}
	}
}

template<typename Itr>
inline void intro_sort(Itr beg, Itr end) {
	if(!stack_quick_sort(beg, end))
		insertion_sort(beg, end);
}
template<typename Itr, typename Comp>
inline void intro_sort(Itr beg, Itr end, Comp cmp) {
	if(!stack_quick_sort(beg, end, cmp))
		insertion_sort(beg, end, cmp);
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
		if(less_func(*(beg + 1), *beg))
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

}
