#pragma once

#ifndef _HTL_VECTOR_H
#define _HTL_VECTOR_H

#include"htl_micros.h"
#include"htl_allocator.h"
#include"htl_unintialized.h"
HTL_NS_BEGIN

#define ITERATOR_CALSS 1

template<class T, class Alloc = alloc>
class MyVector
{
public:
	typedef T			value_type;
	typedef T*			pointer;
	typedef const T*	const_pointer;
	typedef T*			iterator;
	typedef const T*	const_iterator;
	typedef T&			reference;
	typedef const T&	const_reference;
	typedef size_t		size_type;
	typedef ptrdiff_t	difference_type;

protected:
	iterator start;
	iterator finish;
	iterator end_of_storage;
	typedef simple_alloc<T, Alloc> data_allocator;

protected:
	void insert_aux(iterator position, const T& x);
	void deallocate() {
		if (start) data_allocator::deallocate(start, end_of_storage - start);
	}
	void fill_initialize(size_type n, const T& value) {
		start = allocate_and_fill(n, value);
		finish = start + n;
		end_of_storage = finish;
	}

public:
	iterator begin() { return start; }
	const_iterator cbegin() const { return start; }

	iterator end() { return finish; }
	const_iterator cend() const { return finish; }

	//reverse_reference TODO


	size_type size() const { return size_type(end() - begin()); }
	size_type max_size() const { return size_type(-1) / sizeof(T); }
	size_type capacity() const { return size_type(end_of_storage - begin()); }
	bool empty() const { return begin() == end(); }

	reference operator[](size_type n) { return *(begin() + n); }
	const_reference operator[](size_type n) const { return *(begin() + n); }


public:
	MyVector():start(0), finish(0), end_of_storage(0){}
	MyVector(size_type n, const T& value) { fill_initialize(n, value); }
	MyVector(int n, const T& value) { fill_initialize(n, value); }
	MyVector(long n, const T& value) { fill_initialize(n, value); }
	explicit MyVector(size_type n) { fill_initialize(n, T()); }

	MyVector(const MyVector<T, Alloc>& x) {
		start = allocate_and_copy(x.end() - x.begin(), x.begin(), x.end());
		finish = start + (x.end() - x.begin());
		end_of_storage = finish;
	}

#if ITERATOR_CALSS
	template<class InputIterator>
	MyVector(InputIterator first, InputIterator last)
		:start(0), finish(0), end_of_storage(0)
	{
		range_initialize(first, last, iterator_category(first));
	}
#endif

	MyVector(const_iterator first, const_iterator last) {
		size_type n = distance(first, last);
		//iterator tmp = first; while (tmp++ != last) { ++n; }

		start = allocate_and_copy(n, first, last);
		finish = start + n;
		end_of_storage = finish;
	}

	~MyVector() {
		destroy(start, finish);
		deallocate();
	}

	MyVector<T, Alloc>& operator=(const MyVector<T, Alloc>& x);

	//expand capacity
	void reserve(size_tyep n) {
		if (capacity() < n) {
			const size_type old_size = size();
			iterator tmp = allocate_and_copy(n, start, finish);
			destroy(start, finish);
			deallocate();
			start = tmp;
			finish = tmp + old_size;
			end_of_storage = start + n;
		}
	}

public:
	reference front() { return *begin(); }
	const_reference cfront() const { return *begin(); }

	reference back() { return *(end() - 1); }
	const_reference cback() const { return *(end() - 1); }

public:
	void push_back(const T& x) {
		if (finish != end_of_storage) {
			construct(finish, x);
			++finish;
		}
		else
			insert_aux(end(), x);
	}

	void swap(MyVector<T, Alloc>& x) {
		std::swap(start, x.start);
		std::swap(finish, x.finish);
		std::swap(end_of_storage, x.end_of_storage);
	}

	iterator insert(iterator position, const T& x) {
		size_type n = position - begin();
		if (finish != end_of_storage && position != end()) {
			construct(finish, x);
			++finish;
		}
		else
			insert_aux(position, x);
		return begin() + n;
	}
	iterator insert(iterator position) {
		return insert(position, T());
	}

#if ITERATOR_CALSS
	template<class InputIterator>
	void insert(iterator position, InputIterator first, InputIterator last) {
		range_insert(position, first, last, iterator_category(first));
	}
#endif // XXXXXXXXXXXXXXXXXXXXXX

	void insert(iterator position, const_iterator first, const_iterator last);

	void insert(iterator position, size_type n, const T& x);
	void insert(iterator position, int n, const T& x) { insert(position, size_type(n), x); }
	void insert(iterator position, long n, const T& x) { insert(position, size_type(n), x); }


	void pop_back() {
		--finish;
		destroy(finish);
	}


	iterator erase(iterator position) {
		if (position + 1 != end()) 
			copy(position + 1, finish, position);
		--finish;
		destroy(finish);
		return position;
	}
	iterator erase(iterator first, iterator last) {
		iterator i = copy(last, finish, first);
		destroy(i, finish);
		finish = finish - (last - first);
		return first;
	}

	void resize(size_type new_size, const T& x) {
		if (new_size < size())
			erase(begin() + new_size, end());
		else
			insert(end(), new_size - size(), x);
	}
	void resize(size_type new_size) { resize(new_size, T()); }

	void clear() { erase(begin(), end()); }

protected:
	iterator allocate_and_fill(size_type n, const T& x) {
		iterator result = data_allocator::allocate(n);
		try {
			uninitialized_fill_n(result, n, x);
			return result;
		}
		catch (...) {
			data_allocator::deallocate(result, n);
			throw;
		}
	}

#if ITERATOR_CALSS
	template<class ForwardIterator>
	iterator allocate_and_copy(size_type n, ForwardIterator first, ForwardIterator last) {
		iterator result = data_allocator::allocate(n);
		try {
			uninitialized_copy(first, last, result);
			return result;
		}
		catch (...) {
			data_allocator::deallocate(result, n);
			throw;
		}
	}
#endif
	iterator allocate_and_copy(size_type n, const_iterator first, const_iterator last) {
		iterator result = data_allocator::allocate(n);
		try {
			uninitialized_copy(first, last, result);
		}
		catch (...) {
			data_allocator::deallocate(result, n);
			throw;
		}
	}

#if ITERATOR_CALSS
	template<class ForwardIterator>
	void range_initialize(ForwardIterator first, ForwardIterator last, forward_iterator_tag) {
		size_type n = distance(first, last);
		start = allocate_and_copy(n, first, last);
		finish = start + n;
		end_of_storage = finish;
	}

	template<class InputIterator>
	void range_insert(iterator pos, InputIterator first, InputIterator last, input_iterator_tag);

	template<class ForwardIterator>
	void range_insert(iterator pos, ForwardIterator first, ForwardIterator last, forward_iterator_tag);

#endif

	// 
};

template<class T, class Alloc>
inline bool operator==(const MyVector<T, Alloc>& x, const MyVector<T, Alloc>& y) {
	return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
}

template <class T, class Alloc>
inline bool operator<(const MyVector<T, Alloc>& x, const MyVector<T, Alloc>& y) {
	return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template<class T, class Alloc>
inline void swap(const MyVector<T, Alloc>& x, const MyVector<T, Alloc>& y) {
	x.swap(y);
}

template<class T, class Alloc>
MyVector<T, Alloc>& MyVector<T, Alloc>::operator=(const MyVector<T, Alloc>& x) {
	if (&x != this) {
		if (x.size() > capacity()) {
			iterator tmp = allocate_and_copy(x.end() - x.begin(), x.begin(), x.end());
			destroy(start, finish);
			deallocate();
			start = tmp;
			end_of_storage = start + (x.end() - x.begin());
		}
		else if (size() >= x.size()) {
			iterator i = copy(x.begin(), x.end(), begin());
			destroy(i, finish);
		}
		else{
			copy(x.begin(), x.end() + size(), start);
			uninitialize_copy(x.begin() + size(), x.end(), finish);
		}
		finish = start + x.size();
	}
	return *this;
}

template<class T, class Alloc>
void MyVector<T, Alloc>::insert_aux(iterator position, const T& x) {
	if (finish != end_of_storage) {
		construct(finish, *(finish - 1));
		++finish;
		T x_copy = x; /// why need x_copy
		copy_backward(position, finish - 2, finish - 1);
		*position = x_copy;
	}
	else {
		const size_type old_size = size();
		const size_type len = old_size != 0 ? 2 * old_size : 1;

		iterator new_start = data_allocator::allocate(n);
		iterator new_finish = new_start;
		try {
			new_finish = uninitialized_copy(start, position, new_start);
			construct(new_finish, x);
			++new_finish;
			new_finish = uninitialized_copy(position, finish, new_finish);
		}
		catch (...) {
			destroy(new_start, new_finish);
			data_allocator::deallocate(new_start, len);
			throw;
		}

		destroy(begin(), end());
		deallocate();
		start = new_start;
		finish = new_finish;
		end_of_storage = new_start + len;
	}
}

template<class T, class Alloc>
void MyVector<T, Alloc>::insert(iterator position, size_type n, const T& x) {
	if (n != 0) {
		if (size_type(end_of_storage - finish) >= n) {
			T x_copy = x;
			const size_type elems_after = finish - position;
			iterator old_finish = finish;
			if (elems_after > n) {
				uninitialized_copy(finish - n, finish, finish);
				finish += n;
				copy_backward(position, old_finish - n, old_finish);
				fill(position, position + n, x_copy);
			}
			else {
				uninitialized_fill_n(finish, n - elems_after, x_copy);
				finish += n - elems_after;
				uninitialized_copy(position, old_finish, finish);
				finish += elems_after;
				fill(position, old_finish, x_copy);
			}
		}
		else {
			const size_type old_size = size();
			const size_type len = old_size + max(old_size, n);
			iterator new_start = data_allocator::allocate(len);
			iterator new_finish = new_start;
			try{
				new_finish = uninitialized_copy(start, position, new_start);
				new_finish = uninitialized_fill_n(new_finish, n, x);
				new_finish = uninitialized_copy(position, finish, new_finish);
			}
			catch (...) {
				destroy(new_start, new_finish);
				data_allocator::deallocate(new_start, len);
				throw;
			}
			destroy(start, finish);
			deallocate();
			start = new_start;
			finish = new_finish;
			end_of_storage = new_start + len;
		}
	}
}

#if ITERATOR_CALSS
//?这两个range_insert为什么分开来，有什么不同
template<class T, class Alloc> template<class InputIterator> 
void MyVector<T, Alloc>::range_insert(iterator pos, InputIterator first,
	InputIterator last, input_iterator_tag) {
	for (; first != last; ++first) {
		pos = insert(pos, *first);
		++pos;
	}
}

template<class T, class Alloc> template<class ForwardIterator>
void MyVector<T, Alloc>::range_insert(iterator position, ForwardIterator first,
	ForwardIterator last, forward_iterator_tag) {
	if (first != last) {
		size_type n = distance(first, last);
		if (size_type(end_of_storage - finish) >= n) {
			const size_type elems_after = finish - position;
			iterator old_finish = finish;
			if (elems_after > n) {
				uninitialized_copy(finish - n, finish, finish);
				finish += n;
				copy_backward(position, old_finish - n, old_finish);
				copy(first, last, pos)
			}
			else {
				ForwardIterator mid = first;
				advance(mid, elems_after);
				uninitialized_copy(mid, last, finish);
				finish += n - elems_after;
				uninitialized_copy(position, old_finish, finish);
				finish += elems_after;
				copy(first, mid, position);
			}
		}
		else {
			const size_type old_size = size();
			const size_type len = old_size + max(old_size, n);
			iterator new_start = data_allocator::allocate(len);
			iterator new_finish = new_start;
			try{
				new_finish = uninitialized_copy(start, position, new_start);
			new_finish = uninitialized_copy(first, last, new_finish);
			new_finish = uninitialized_copy(position, finish, new_finish);
			}
			catch (...) {
				destroy(new_start, new_finish);
				data_allocator::deallocate(new_start, len);
				throw;
			}

			destroy(start, finish);
			deallocate();
			start = new_start;
			finish = new_finish;
			end_of_storage = new_start + len;
		}
	}
}

#endif

template <class T, class Alloc>
void MyVector<T, Alloc>::insert(iterator position, const_iterator first, const_iterator last) {
	if (first != last) {
		size_type n = 0;
		distance(first, last, n);
		if (size_type(end_of_storage - finish) >= n) {
			const size_type elems_after = finish - position;
			iterator old_finish = finish;
			if (elems_after > n) {
				uninitialized_copy(finish - n, finish, finish);
				finish += n;
				copy_backward(position, old_finish - n, old_finish);
				copy(first, last, position);
			}
			else {
				uninitialized_copy(first + elems_after, last, finish);
				finish += n - elems_after;
				uninitialized_copy(position, old_finish, finish);
				finish += elems_after;
				copy(first, first + elems_after, position);
			}
		}
		else {
			const size_type old_size = size();
			const size_type len = old_size + max(old_size, n);
			iterator new_start = data_allocator::allocate(len);
			iterator new_finish = new_start;
			try{
				new_finish = uninitialized_copy(start, position, new_start);
			new_finish = uninitialized_copy(first, last, new_finish);
			new_finish = uninitialized_copy(position, finish, new_finish);
			}
			catch (...) {
				destroy(new_start, new_finish);
				data_allocator::deallocate(new_start, len);
				throw;
			}

			destroy(start, finish);
			deallocate();
			start = new_start;
			finish = new_finish;
			end_of_storage = new_start + len;
		}
	}
}

HTL_NS_END

#endif // !_HTL_VECTOR_H
