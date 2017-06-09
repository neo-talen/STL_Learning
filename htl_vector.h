#pragma once

#ifndef _HTL_VECTOR_H
#define _HTL_VECTOR_H

#include"htl_micros.h"
#include"htl_alloc.h"

HTL_NS_BEGIN

template<class T, class Alloc = allocator<T>>
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

	typedef simple_alloc<value_type, Alloc> data_allocator;

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

#ifdef XXXXXXXXXXXXXXXXXXXXXXX
	template<class InputIterator>
	MyVector(InputIterator first, InputIterator last)
		:start(0), finish(0), end_of_storage(0)
	{
		range_initialize(first, last, iterator_category(first));
	}
#endif

	MyVector(const_iterator first, const_iterator last) {
		size_type n = 0;
		//distance(first, last, n); //seperate by iterator_category for efficient, 
		// but here just loop it; for calculate n 
		iterator tmp = first; while (tmp++ != last) { ++n; }

		start = allocate_and_copy(n, first, last);
		finish = start + n;
		end_of_storage = finish;
	}

	~MyVector() {
		destroy(start, finish);
		deallocate();
	}

	MyVector<T, Alloc>& operator=(const MyVector<T, Alloc>& x);

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

#ifdef XXXXXXXXXXXXXXXXXXXXXX
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
		}
	}

#ifdef XXXXXXXXXXXXXXXXXXXXXXX
	iterator allocate_and_copy(size_type n, const_iterator first, const_iterator last) {
		iterator result = data_allocator::allocate(n);
		try {
			uninitialized_copy(first, last, result);
		}
		catch (...) {
			data_allocator::deallocate(result, n);
		}
	}
#endif






};




HTL_NS_END

#endif // !_HTL_VECTOR_H
