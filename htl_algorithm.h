#pragma once

#ifndef  _HTL_ALGORITHM_H
#define  _HTL_ALGORITHM_H

#include"htl_micros.h"
#include"htl_iterator.h"
#include"htl_type_traits.h"

HTL_NS_BEGIN

// swap
template<class ForwardIterator1, class ForwardIterator2 class T >
inline void __iter_swap(ForwardIterator1 left, ForwardIterator2 right, T*) {
	T tmp = *left;
	*left = *right;
	*right = tmp;
}
template<class ForwardIterator1, class ForwardIterator2>
inline void iter_swap(ForwardIterator1 left, ForwardIterator2 right) {
	__iter_swap(left, right, value_type(left));
}

template<class T>
inline void swap(T& left, T& right) {
	T tmp = left;
	left = right; 
	right = tmp;
}

// min, max
template<class T>
inline const T& min(const T& a, const T& b) {
	return a < b ? a : b;
}

template<class T>
inline const T& max(const T& a, const T& b) {
	return a < b ? b : a;
}

template<class T, class Compare>
inline const T& min(const T& a, const T& b, Compare comp) {
	return comp(a, b) ? a : b;
}

template<class T, class Compare>
inline const T& max(const T& a, const T& b, Compare comp) {
	return comp(a, b) ? b : a;
}

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

//---------------------copy_n

template<class InputIterator, class Size, class OutputIterator>
pair<InputIterator, OutputIterator> __copy_n(InputIterator first, Size count,
	OutputIterator result, input_iterator_tag) {
	for (; count > 0; --count)
		*result++ = *first++;

	return make_pair(first, result);
}

template<class InputIterator, class Size, class OutputIterator>
inline pair<InputIterator, OutputIterator> __copy_n(InputIterator first, Size count,
	OutputIterator result, random_access_iterator_tag) {
	InputIterator last = first + count;
	
	return make_pair(last, copy(first, last, result);
}

template<class InputIterator, class Size, class OutputIterator>
inline pair<InputIterator, OutputIterator> copy_n(InputIterator first, Size count, OutputIterator result) {
	return __copy_n(first, count, result, iterator_category(first));
}


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

// mismatch
template <class InputIterator1, class InputIterator2>
pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1,
	InputIterator1 last1,
	InputIterator2 first2) {
	while (first1 != last1 && *first1 == *first2) {
		++first1;
		++first2;
	}
	return pair<InputIterator1, InputIterator2>(first1, first2);
}

template <class InputIterator1, class InputIterator2, class BinaryPredicate>
pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1,
	InputIterator1 last1,
	InputIterator2 first2,
	BinaryPredicate binary_pred) {
	while (first1 != last1 && binary_pred(*first1, *first2)) {
		++first1;
		++first2;
	}
	return pair<InputIterator1, InputIterator2>(first1, first2);
}

// equal
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

// lexicographical_compare//辞典比较
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

inline bool
lexicographical_compare(const unsigned char* first1,
	const unsigned char* last1,
	const unsigned char* first2,
	const unsigned char* last2)
{
	const size_t len1 = last1 - first1;
	const size_t len2 = last2 - first2;
	const int result = memcmp(first1, first2, min(len1, len2));
	return result != 0 ? result < 0 : len1 < len2;
}

inline bool lexicographical_compare(const char* first1, const char* last1,
	const char* first2, const char* last2)
{
#if CHAR_MAX == SCHAR_MAX
	return lexicographical_compare(	(const signed char*)first1,
									(const signed char*)last1,
									(const signed char*)first2,
									(const signed char*)last2	);
#else
	return lexicographical_compare(	(const unsigned char*)first1,
									(const unsigned char*)last1,
									(const unsigned char*)first2,
									(const unsigned char*)last2	);
#endif
}

// lexicographical_compare_3way
template <class InputIterator1, class InputIterator2>
int lexicographical_compare_3way(InputIterator1 first1, InputIterator1 last1,
	InputIterator2 first2, InputIterator2 last2)
{
	while (first1 != last1 && first2 != last2) {
		if (*first1 < *first2) return -1;
		if (*first2 < *first1) return 1;
		++first1; ++first2;
	}
	if (first2 == last2) {
		return !(first1 == last1);
	}
	else {
		return -1;
	}
}

inline int
lexicographical_compare_3way(const unsigned char* first1,
	const unsigned char* last1,
	const unsigned char* first2,
	const unsigned char* last2)
{
	const ptrdiff_t len1 = last1 - first1;
	const ptrdiff_t len2 = last2 - first2;
	const int result = memcmp(first1, first2, min(len1, len2));
	return result != 0 ? result : (len1 == len2 ? 0 : (len1 < len2 ? -1 : 1));
}

inline int lexicographical_compare_3way(const char* first1, const char* last1,
	const char* first2, const char* last2)
{
#if CHAR_MAX == SCHAR_MAX
	return lexicographical_compare_3way(
		(const signed char*)first1,
		(const signed char*)last1,
		(const signed char*)first2,
		(const signed char*)last2);
#else
	return lexicographical_compare_3way((const unsigned char*)first1,
		(const unsigned char*)last1,
		(const unsigned char*)first2,
		(const unsigned char*)last2);
#endif
}

// lower_bound
template <class ForwardIterator, class T, class Distance>
ForwardIterator __lower_bound(ForwardIterator first, ForwardIterator last,
	const T& value, Distance*,
	forward_iterator_tag) {
	Distance len = 0;
	distance(first, last, len);
	Distance half;
	ForwardIterator middle;

	while (len > 0) {
		half = len >> 1;
		middle = first;
		advance(middle, half);
		if (*middle < value) {
			first = middle;
			++first;
			len = len - half - 1;
		}
		else
			len = half;
	}
	return first;
}

template <class RandomAccessIterator, class T, class Distance>
RandomAccessIterator __lower_bound(RandomAccessIterator first,
	RandomAccessIterator last, const T& value,
	Distance*, random_access_iterator_tag) {
	Distance len = last - first;
	Distance half;
	RandomAccessIterator middle;

	while (len > 0) {
		half = len >> 1;
		middle = first + half;
		if (*middle < value) {
			first = middle + 1;
			len = len - half - 1;
		}
		else
			len = half;
	}
	return first;
}

template <class ForwardIterator, class T>
inline ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last,
	const T& value) {
	return __lower_bound(first, last, value, distance_type(first),
		iterator_category(first));
}

template <class ForwardIterator, class T, class Compare, class Distance>
ForwardIterator __lower_bound(ForwardIterator first, ForwardIterator last,
	const T& value, Compare comp, Distance*,
	forward_iterator_tag) {
	Distance len = 0;
	distance(first, last, len);
	Distance half;
	ForwardIterator middle;

	while (len > 0) {
		half = len >> 1;
		middle = first;
		advance(middle, half);
		if (comp(*middle, value)) {
			first = middle;
			++first;
			len = len - half - 1;
		}
		else
			len = half;
	}
	return first;
}

template <class RandomAccessIterator, class T, class Compare, class Distance>
RandomAccessIterator __lower_bound(RandomAccessIterator first,
	RandomAccessIterator last,
	const T& value, Compare comp, Distance*,
	random_access_iterator_tag) {
	Distance len = last - first;
	Distance half;
	RandomAccessIterator middle;

	while (len > 0) {
		half = len >> 1;
		middle = first + half;
		if (comp(*middle, value)) {
			first = middle + 1;
			len = len - half - 1;
		}
		else
			len = half;
	}
	return first;
}

template <class ForwardIterator, class T, class Compare>
inline ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last,
	const T& value, Compare comp) {
	return __lower_bound(first, last, value, comp, distance_type(first),
		iterator_category(first));
}

// median
template<class T>
inline const T& __median(const T& a, const T& b, const T& c) {
	if (a < b)
		if (b < c)
			return b;
		else if (a < c)
			return c;
		else
			return a;
	else if (a < c)
		return a;
	else if (b < c)
		return c;
	else
		return b;
}

template<class T, class Compare>
inline const T& __median(const T& a, const T& b, const T& c, Compare comp) {
	if (comp(a, c))
		if (comp(b, c))
			return b;
		else if (comp(a, c))
			return c;
		else
			return a;
	else if (comp(a, c))
		return a;
	else if (comp(b, c))
		return c;
	else
		return b;
}

template<class InputIterator, class Function>
Function for_each(InpputIterator first, InputIterator last, Function f) {
	for (; first != last; ++first)
		f(*first);
	return f;
}

// find
template<class InputIterator, class T>
InputIterator find(InputIterator first, InputIterator last, const T& value) {
	while (first != last && *first != value)++first;
	return first;
}

template<class II, class Predicate>
II find_if(II first, II last, Predicate pred) {
	while (first != last && !pred(*first))++first;
	return first;
}

// adjacent_find
template<class FI>
FI adjacent_find(FI first, FI last) {
	if (first == last)return last;
	FI next = first;
	while (++next != last) {
		if (*first == *next)return first;
		first = next;
	}
	return last;
}

template<class FI, class BinaryPredicate>
FI adjacent_find(FI first, FI last, BinaryPredicate binary_pred) {
	if (first == last)return last;
	FI next = first;
	while (++next != last) {
		if (binary_pred(*first, *next))return first;
		first = next;
	}
	return last;
}

// count
template<class II, class T, class Size>
void count(II first, II last, const T& value, Size& n) {
	for (; first != last; ++first)
		if (*first == value)
			++n;
}

template<class II, class Predicate, class Size>
void count_if(II first, II last, Predicate pred, Size& n) {
	for (; first != last; ++first)
		if (pred(*first))
			++n;
}

template <class InputIterator, class T>
typename iterator_traits<InputIterator>::difference_type
count(InputIterator first, InputIterator last, const T& value) {
	typename iterator_traits<InputIterator>::difference_type n = 0;
	for (; first != last; ++first)
		if (*first == value)
			++n;
	return n;
}

template <class InputIterator, class Predicate>
typename iterator_traits<InputIterator>::difference_type
count_if(InputIterator first, InputIterator last, Predicate pred) {
	typename iterator_traits<InputIterator>::difference_type n = 0;
	for (; first != last; ++first)
		if (pred(*first))
			++n;
	return n;
}

// search











HTL_NS_END

#endif // ! _HTL_ALGORITHM_H
