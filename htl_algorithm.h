#pragma once

#ifndef  _HTL_ALGORITHM_H
#define  _HTL_ALGORITHM_H

#include"htl_micros.h"
#include"htl_iterator.h"
#include"htl_type_traits.h"

HTL_NS_BEGIN

//-----------------------copy
template<class InputIterator, class OutputIterator>
inline OutputIterator __copy(InputIterator first, InputIterator last,
	OutputIterator result, input_iterator_tag) {
	for (; first != last; ++result, ++first)
		*result = *first;
	return result;
}

template<class RandomAccessIterator, class OutputIterator, class Distance>
inline OutputIterator
__copy_d(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, Distance*) {
	for (Distance n = last - first; n > 0; --n, ++result, ++first)
		*result = *first;
	return result;
}

template<class RandomAccessIterator, class OutputIterator>
inline OutputIterator
__copy(RandomAccessIterator first, RandomAccessIterator last, 
	OutputIterator result, random_access_iterator_tag) {
	return __copy_d(first, last, result, distance_type(first));
}
//泛化
template<class InputIterator, class OutputIterator>
struct __copy_dispatch {
	OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result) {
		return __copy(first, last, result, iterator_category(first));
	}
};

// partial specialization
template<class T>
inline T* __copy_t(const T* first, const T* last, T* result, __true_type) {
	memmove(result, first, sizeof(T) * (last - first));
	return result + (last - first);
}

template<class T>
inline T* __copy_t(const T* first, const T* last, T* result, __false_type) {
	return __copy_d(first, last, result, (ptrdiff_t*)0);
}
//特化
template<class T>
struct __copy_dispatch<T*, T*>{
	T* operator()(T* first, T* last, T* result) {
		typedef typename __type_traits<T>::has_trivial_assignment_operator t;
		return __copy_t(first, last, result, t());
	}
};
//特化
template<class T>
struct __copy_dispatch<const T*, T*>{
	T* operator()(const T* first, const T* last, T* result) {
		typedef typename __type_traits<T>::has_trivial_assignment_operator t;
		return __copy_t(first, last, result, t());
	}
};

template<class InputIterator, class OutputIterator>
inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result) {
	return __copy_dispatch<InputIterator, OutputIterator>()(first, last, result);
}

//特化
inline char* copy(const char* first, const char* last, char* result) {
	memmove(result, first, last - first);
	return result + (last - first);
}

inline wchar_t* copy(const wchar_t* first, const wchar_t* last,
	wchar_t* result) {
	memmove(result, first, sizeof(wchar_t) * (last - first));
	return result + (last - first);
}

//---------------------------copy_backward

template <class BidirectionalIterator1, class BidirectionalIterator2>
inline BidirectionalIterator2 
__copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, 
	BidirectionalIterator2 result) {
	while (first != last) *--result = *--last;
	return result;
}


template <class BidirectionalIterator1, class BidirectionalIterator2>
struct __copy_backward_dispatch
{
	BidirectionalIterator2 operator()(BidirectionalIterator1 first, BidirectionalIterator1 last,
		BidirectionalIterator2 result) {
		return __copy_backward(first, last, result);
	}
};

template <class T>
inline T* __copy_backward_t(const T* first, const T* last, T* result, __true_type) {
	const ptrdiff_t N = last - first;
	memmove(result - N, first, sizeof(T) * N);
	return result - N;
}

template <class T>
inline T* __copy_backward_t(const T* first, const T* last, T* result, __false_type) {
	return __copy_backward(first, last, result);
}

template <class T>
struct __copy_backward_dispatch<T*, T*>
{
	T* operator()(T* first, T* last, T* result) {
		typedef typename __type_traits<T>::has_trivial_assignment_operator t;
		return __copy_backward_t(first, last, result, t());
	}
};

template <class T>
struct __copy_backward_dispatch<const T*, T*>
{
	T* operator()(const T* first, const T* last, T* result) {
		typedef typename __type_traits<T>::has_trivial_assignment_operator t;
		return __copy_backward_t(first, last, result, t());
	}
};

template <class BidirectionalIterator1, class BidirectionalIterator2>
inline BidirectionalIterator2 
copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last,
	BidirectionalIterator2 result) {
	return __copy_backward_dispatch<BidirectionalIterator1,
		BidirectionalIterator2>()(first, last, result);
}

//---TODO-----------------copy_n



//-----------------------fill

template<class ForwardIterator, class T>
void fill(ForwardIterator first, ForwardIterator last, const T& value) {
	for (; first != last; ++first)
		*first = value;
}

template<class OutputIterator, class Size, class T>
OutputIterator fill_n(OutputIterator first, Size n, const T& value) {
	for (; n >= ; --n, ++first)
		*first = value;
	return first;
}


//-----------other small funcitons


template <class InputIterator1, class InputIterator2>
inline bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2) {
	for (; first1 != last1; ++first1, ++first2)
		if (*first1 != *first2)
			return false;
	return true;
}

template <class InputIterator1, class InputIterator2, class BinaryPredicate>
inline bool equal(InputIterator1 first1, InputIterator1 last1, 
	InputIterator2 first2, BinaryPredicate binary_pred) {
	for (; first1 != last1; ++first1, ++first2)
		if (!binary_pred(*first1, *first2))
			return false;
	return true;
}


template <class InputIterator1, class InputIterator2>
bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, InputIterator2 last2) {
	for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
		if (*first1 < *first2)
			return true;
		if (*first2 < *first1)
			return false;
	}
	return first1 == last1 && first2 != last2;
}

template <class InputIterator1, class InputIterator2, class Compare>
bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, InputIterator2 last2,
	Compare comp) {
	for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
		if (comp(*first1, *first2))
			return true;
		if (comp(*first2, *first1))
			return false;
	}
	return first1 == last1 && first2 != last2;
}




// other utility

template<class T>
T max(T a, T b) {
	return a < b ? b : a;
}








HTL_NS_END

#endif // ! _HTL_ALGORITHM_H
