#pragma once

#ifndef _HTL_UNINITIALIZED_H
#define _HTL_UNINITIALIZED_H

#include"htl_micros.h"

#include"htl_iterator.h"
#include"htl_type_traits.h"
#include"htl_allocator.h"

HTL_NS_BEGIN

//.......................uninitialized_copy
template<class InputIterator, class ForwardIterator>
inline ForwardIterator
__uninitialized_copy_aux(InputIterator first, InputIterator last, 
	ForwardIterator result, __true_type) {
	return copy(first, last, result);
}

template<class InputIterator, class ForwardIterator>
inline ForwardIterator
__uninitialized_copy_aux(InputIterator first, InputIterator last,
	ForwardIterator result, __false_type) {
	ForwardIterator cur = result;
	try {
		for (; first != last; ++first, ++cur) {
			construc(&*cur, *first);
		}
		return cur;
	}
	catch (...) {
		destroy(result, cur);
	}
}

template<class InputIterator, class ForwardIterator, class T>
inline ForwardIterator
__uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T*) {
	typedef typename __type_traits<T>::is_POD_type is_POD;
	return __uninitialized_copy_aux(first, last, result, is_POD());
}

template<class InputIterator, class ForwardIterator>
inline ForwardIterator
uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) {
	__uninitialized_copy(first, last, result, value_type(first));
}

inline char* uninitialized_copy(const char* first, const char* last, char* result) {
	memmove(result, first, last - first);
	return result + (last - first);
}

inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
	memmove(result, first, sizeof(wchar_t) * (last - first));
	return result + (last - first);
}


//...............................uninitialized_fill









HTL_NS_END

#endif