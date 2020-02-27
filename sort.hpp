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
	return !(lhs < rhs) & !(rhs < lhs);
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
	return !cmp(lhs, rhs) & !cmp(rhs, lhs);
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
	//add a stack item
	struct stack_less_data {
		Itr beg;
		Itr end;
	};
	std::vector<stack_less_data> stk;
	stk.resize(50);
	size_t idx = 0;
	stack_less_data dat = {
		beg,
		end - 1
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		stack_less_data tmp = stk[--idx];
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
			stack_less_data dat = {
				pivot + 1,
				tmp.end
			};
			stk[idx++] = std::move(dat);
			if(idx == stk.size())
				stk.resize(stk.size() * 2);
		}
		if(distance(tmp.beg, pivot) > 1) {
			stack_less_data dat = {
				tmp.beg,
				pivot
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
	//add a stack item
	struct stack_less_data {
		Itr beg;
		Itr end;
	};
	std::vector<stack_less_data> stk;
	stk.resize(50);
	size_t idx = 0;
	stack_less_data dat = {
		beg,
		end - 1
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		stack_less_data tmp = stk[--idx];
		Itr left = tmp.beg - 1;
		Itr right = tmp.end + 1;
		Itr pivot = middle_of_three(tmp.beg, half_point(tmp.beg, tmp.end + 1), tmp.end, cmp);

		do {
			++left;
			--right;
			//pivot goes to the right!!
			while(left != right && left != pivot && less_equal_func(*left, *pivot, cmp))
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
			stack_less_data dat = {
				pivot + 1,
				tmp.end
			};
			stk[idx++] = std::move(dat);
			if(idx == stk.size())
				stk.resize(stk.size() * 2);
		}
		if(distance(tmp.beg, pivot) > 1) {
			stack_less_data dat = {
				tmp.beg,
				pivot
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
	//add a stack item
	struct stack_less_data {
		Itr beg;
		Itr end;
	};
	std::vector<stack_less_data> stk;
	stk.resize(50);
	size_t idx = 0;
	stack_less_data dat = {
		beg,
		end - 1
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		stack_less_data tmp = stk[--idx];
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
			stack_less_data dat = {
				pivot + 1,
				tmp.end
			};
			stk[idx++] = std::move(dat);
			if(idx == stk.size())
				stk.resize(stk.size() * 2);
		}
		if(distance(tmp.beg, pivot) > 1) {
			stack_less_data dat = {
				tmp.beg,
				pivot
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
	//add a stack item
	struct stack_less_data {
		Itr beg;
		Itr end;
	};
	std::vector<stack_less_data> stk;
	stk.resize(50);
	size_t idx = 0;
	stack_less_data dat = {
		beg,
		end - 1
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		stack_less_data tmp = stk[--idx];
		Itr left = tmp.beg - 1;
		Itr right = tmp.end + 1;
		Itr pivot = middle_of_three(tmp.beg, half_point(tmp.beg, tmp.end + 1), tmp.end);

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
			stack_less_data dat = {
				pivot + 1,
				tmp.end
			};
			stk[idx++] = std::move(dat);
			if(idx == stk.size())
				stk.resize(stk.size() * 2);
		}
		if(distance(tmp.beg, pivot) > 1) {
			stack_less_data dat = {
				tmp.beg,
				pivot
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
	stdlib::rotate(moveleft + 1, pivot, pivot + 1);
	pivot -= distance(moveleft + 1, pivot);

	//rotate the right side over
	stdlib::rotate(moveleft + 1, tmpright, moveright);
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
	//add a stack item
	struct stack_less_data {
		Itr beg;
		Itr end;
	};
	std::vector<stack_less_data> stk;
	stk.resize(50);
	size_t idx = 0;
	stack_less_data dat = {
		beg,
		end
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		stack_less_data item = stk[--idx];

		Itr pivot = half_point(item.beg, item.end);

		sweep_sort_internal(pivot, item.beg, item.end);

		if(distance(pivot + 1, item.end) > 1) {
			stack_less_data dat = {
				pivot + 1,
				item.end
			};
			stk[idx++] = std::move(dat);
			if(idx == stk.size())
				stk.resize(stk.size() * 2);
		}
		if(distance(item.beg, pivot) > 1) {
			stack_less_data dat = {
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
	stdlib::rotate(moveleft + 1, pivot, pivot + 1);
	pivot -= distance(moveleft + 1, pivot);

	//rotate the right side over
	stdlib::rotate(moveleft + 1, tmpright, moveright);
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
	//add a stack item
	struct stack_less_data {
		Itr beg;
		Itr end;
	};
	std::vector<stack_less_data> stk;
	stk.resize(50);
	size_t idx = 0;
	stack_less_data dat = {
		beg,
		end
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		stack_less_data item = stk[--idx];

		Itr pivot = half_point(item.beg, item.end);

		sweep_sort_internal(pivot, item.beg, item.end, cmp);

		if(distance(pivot + 1, item.end) > 1) {
			stack_less_data dat = {
				pivot + 1,
				item.end
			};
			stk[idx++] = std::move(dat);
			if(idx == stk.size())
				stk.resize(stk.size() * 2);
		}
		if(distance(item.beg, pivot) > 1) {
			stack_less_data dat = {
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
	stlib_internal::merge_sweep_sort_recurse(pivot, beg, end, nhalf);

	stlib_internal::move_pivot(nhalf, pivot);

	stack_merge_sweep_sort(beg, nhalf);
	stack_merge_sweep_sort(pivot + 1, end);
}
template<typename Itr>
void merge_sweep_sort(Itr beg, Itr end) {
	if(distance(beg, end) <= 1)
		return;
	//add a stack item
	struct stack_less_data {
		Itr beg;
		Itr end;
	};
	std::vector<stack_less_data> stk;
	stk.resize(50);
	size_t idx = 0;
	stack_less_data dat = {
		beg,
		end
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		stack_less_data item = stk[--idx];

		Itr pivot = middle_of_three(item.beg, half_point(item.beg, item.end), item.end - 1);

		Itr nhalf;
		merge_sweep_sort_recurse(pivot, item.beg, item.end, nhalf);

		move_pivot(nhalf, pivot);

		if(distance(pivot + 1, item.end) > 1) {
			stack_less_data dat = {
				pivot + 1,
				item.end
			};
			stk[idx++] = std::move(dat);
			if(idx == stk.size())
				stk.resize(stk.size() * 2);
		}
		if(distance(item.beg, nhalf) > 1) {
			stack_less_data dat = {
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
	stlib_internal::merge_sweep_sort_recurse(pivot, beg, end, nhalf, cmp);

	stlib_internal::move_pivot(nhalf, pivot, cmp);

	stack_merge_sweep_sort(beg, nhalf, cmp);
	stack_merge_sweep_sort(pivot + 1, end, cmp);
}
template<typename Itr, typename Comp>
void merge_sweep_sort(Itr beg, Itr end, Comp cmp) {
	if(distance(beg, end) <= 1)
		return;
	//add a stack item
	struct stack_less_data {
		Itr beg;
		Itr end;
	};
	std::vector<stack_less_data> stk;
	stk.resize(50);
	size_t idx = 0;
	stack_less_data dat = {
		beg,
		end
	};
	stk[idx++] = std::move(dat);

	while(idx > 0) {
		stack_less_data item = stk[--idx];

		Itr pivot = middle_of_three(item.beg, half_point(item.beg, item.end), item.end - 1, cmp);

		Itr nhalf;
		merge_sweep_sort_recurse(pivot, item.beg, item.end, nhalf, cmp);

		move_pivot(nhalf, pivot, cmp);

		if(distance(pivot + 1, item.end) > 1) {
			stack_less_data dat = {
				pivot + 1,
				item.end
			};
			stk[idx++] = std::move(dat);
			if(idx == stk.size())
				stk.resize(stk.size() * 2);
		}
		if(distance(item.beg, nhalf) > 1) {
			stack_less_data dat = {
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
template<typename Itr>
void merge_internal(Itr beg1, Itr end1, Itr beg2, Itr end2, char* buf) {
	Itr begrng = beg1;
	typename stlib::value_for<Itr>::value_type* tmp = (typename stlib::value_for<Itr>::value_type*)buf;
	typename stlib::value_for<Itr>::value_type* tmpbeg = tmp;

	//go through both lists, build the sorted list
	for(; beg1 != end1 && beg2 != end2; ++tmp) {
		if(less_func(*beg2, *beg1)) {
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
		if(less_func(*beg2, *beg1, cmp)) {
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
void bubble_sort(Itr beg, Itr end) {
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
	std::cout << "insert sort" << std::endl;
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
