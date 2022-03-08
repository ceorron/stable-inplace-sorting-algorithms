/*----------------------------------------------------------------------------------*\
 |																					|
 | sort.cpp		 																	|
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

#include "sort.hpp"

namespace stlib {
namespace stlib_internal {

void* aligned_storage_new(size_t sze) {
	//use new to get some aligned data
	return new double[sze/sizeof(double) + (sze%sizeof(double) == 0 ? 0 : 1)];
}
void aligned_storage_delete(size_t sze, void* ptr) {
	delete[] (double*)ptr;
}

bool equal_func_bool(const bool less, const bool greater) {
	return !less && !greater;
}
bool less_func_bool(const bool less) {
	return less;
}
bool greater_func_bool(const bool greater) {
	return greater;
}
bool less_equal_func_bool(const bool greater) {
	return !greater;
}
bool greater_equal_func_bool(const bool less) {
	return !less;
}

bool start_out_of_place_merge(uint64_t sze, uint64_t insert_count) {
	//no merge done (early exit)
	if(sze <= insert_count)
		return false;
	//how many times do we need to merge until we are finished?
	unsigned count = 0;
	while(insert_count < sze) {
		++count;
		insert_count *= 2;
	}
	//if this is odd then we need to start out of place, as we need to move it back to the original array
	return count % 2 == 1;
}

zip_merge_indexes& get_current_indexes(zip_merge_indexes* indexes, unsigned indexes_end) {
	if(indexes_end == 0)
		return indexes[NEW_ZIP_SORT_INDEX_ARRAY_SIZE - 1];
	return indexes[indexes_end - 1];
}
void pop_indexes_count(zip_merge_indexes* indexes,
					   unsigned& indexes_start) {
	--indexes[indexes_start].count;
	if(indexes[indexes_start].count == 0) {
		++indexes_start;
		if(indexes_start == NEW_ZIP_SORT_INDEX_ARRAY_SIZE)
			indexes_start = 0;
	}
}
void push_indexes_count(unsigned idx, unsigned count,
						zip_merge_indexes* indexes,
						unsigned& indexes_end) {
	//increment the current index
	zip_merge_indexes& crnt = get_current_indexes(indexes, indexes_end);
	if(crnt.index == idx) {
		crnt.count += count;
		return;
	}

	//add a new index
	indexes[indexes_end].index = idx;
	indexes[indexes_end].count = count;
	++indexes_end;
	if(indexes_end == NEW_ZIP_SORT_INDEX_ARRAY_SIZE)
		indexes_end = 0;
}
unsigned calculate_index_total(unsigned indexes_start, unsigned indexes_end) {
	if(indexes_start <= indexes_end)
		return indexes_end - indexes_start;
	return NEW_ZIP_SORT_INDEX_ARRAY_SIZE - (indexes_start - indexes_end);
}
bool indexes_full(unsigned indexes_start, unsigned indexes_end) {
	return NEW_ZIP_SORT_INDEX_ARRAY_SIZE - calculate_index_total(indexes_start, indexes_end) <= 1;
}

}
}
