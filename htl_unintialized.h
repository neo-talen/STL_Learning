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
			construct(&*cur, *first);
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
	return __uninitialized_copy(first, last, result, value_type(first));
}

inline char* uninitialized_copy(const char* first, const char* last, char* result) {
	memmove(result, first, last - first);
	return result + (last - first);
}

inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
	memmove(result, first, sizeof(wchar_t) * (last - first));
	return result + (last - first);
}

			//xxxxxxxxx_copy_n
template<class InputIterator, class Size, class ForwardIterator>
std::pair<InputIterator, ForwardIterator>
__uninitialized_copy_n(InputIterator first, Size count, ForwardIterator result, input_iterator_tag) {
	ForwardIterator cur = result;
	try {
		for (; count > 0; --count, ++first, ++cur)
			construct(&*cur, *first);
		return std::pair<InputIterator, ForwardIterator>(first, cur);
	}
	catch (...) {
		destroy(result, cur);
		throw;
	}
}

template<class RandomAccessIterator, class Size, class ForwardIterator>
std::pair<RandomAccessIterator, ForwardIterator>
__uninitialized_copy_n(RandomAccessIterator first, Size count,
	ForwardIterator result, random_access_iterator_tag) {
	RandomAccessIterator last = first + count;
	return std::make_pair(last, uninitialized_copy(first, last, result));
}

template <class InputIterator, class Size, class ForwardIterator>
inline std::pair<InputIterator, ForwardIterator> 
uninitialized_copy_n(InputIterator first, Size count, ForwardIterator result) {
	return __uninitialized_copy_n(first, count, result, iterator_category(first));
}

//...............................uninitialized_fill

template<class ForwardIterator, class T>
inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
	const T& x, __true_type) {
	fill(first, last, x);
}

template<class ForwardIterator, class T>
void __unintialized_fill_aux(ForwardIterator first, ForwardIterator last,
	const T& x, __false_type) {
	ForwardIterator cur = first;
	try {
		for (; cur != last; ++cur) 
			construct(&*cur, x);
	}
	catch (...) {
		destroy(first, cur);
		throw;
	}

}

template<class ForwardIterator, class T, class T1>
inline void __uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x, T1*) {
	typedef typename __type_traits<T1>::is_POD_type is_POD;
	__uninitialized_fill_aux(first, last, x, is_POD());
}

template<class ForwardIterator, class T>
inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x) {
	__uninitialized_fill(first, last, x, value_type(first));
}
			//XXXXXXXXXXXX_fill_n
template<class ForwardIterator, class Size, class T>
inline ForwardIterator
__uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __true_type) {
	return fill_n(first, n, x);
}

template<class ForwardIterator, class Size, class T>
inline ForwardIterator
__uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __false_type) {
	ForwardIterator cur = first;
	try {
		for (; n > 0; --n, ++cur)
			construct(&*cur, x);
		return cur;
	}
	catch (...) {
		destroy(first, cur);
		throw;
	}
}

template <class ForwardIterator, class Size, class T, class T1>
inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*) {
	typedef typename __type_traits<T1>::is_POD_type is_POD;
	return __uninitialized_fill_n_aux(first, n, x, is_POD());

}

template<class ForwardIterator, class Size, class T>
inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x) {
	return __uninitialized_fill_n(first, n, x, value_type(first));
}

//...TODO.............fill_copy/ copy_copy/ copy_fill 





HTL_NS_END

#endif