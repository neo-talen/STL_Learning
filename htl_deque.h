#pragma once

#ifndef  _HTL_DEQUE_H
#define _HTL_DEQUE_H

#include"htl_micros.h"
#include"htl_iterator.h"
#include"htl_allocator.h"
#include"htl_unintialized.h"
#include"htl_algorithm.h"
HTL_NS_BEGIN


// return number of objs a buffer, sz is sizeof(T).
// if set n, return n; ifnot return 512Byte/sz or 1(sz > 512B); 
inline size_t __deque_buf_size(size_t n, size_t sz) {
	return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : 1);
}

// deque_iterator
template<class T, class Ref, class Ptr, size_t BufSize>
class __deque_iterator {
public:
	typedef __deque_iterator<T, T&, T*, BufSize>				iterator;
	typedef __deque_iterator<T, const T&, const T*, BufSize>	const_iteraror;
	static size_t buffer_size() { return __deque_buf_size(BufSize, sizeof(T)); }


	typedef random_access_iterator_tag		iterator_category;
	typedef T								value_type;
	typedef Ptr								pointer;
	typedef Ref								reference;
	typedef size_t							size_type;
	typedef ptrdiff_t						difference_type;
	typedef T**								map_pointer;

	typedef __deque_iterator				self;

	// data member
	T*			cur;
	T*			first;
	T*			last;
	map_pointer node;

	//constructor
	__deque_iterator(T* x, map_pointer y)
		:cur(x), first(*y), last(*y + buffer_size()), node(y){}
	__deque_iterator()
		:cur(0), first(0), last(0), node(0){}
	__deque_iterator(const iterator& x)
		:cur(x.cur), first(x.first), last(x.last), node(x.node){}

	//
	reference operator*()const { return *cur; }
	pointer operator->()const { return &(operator*()); }


	// iterator1 - iterator2 ？ what if < 0
	difference_type operator-(const self& x)const {
		return difference_type(buffer_size()) * (node - x.node - 1) + 
			(cur - first) + (x.last - x.cur);
	}
	// critial func
	void set_node(map_pointer new_node) {
		node = new_node;
		first = *new_node;
		last = first + difference_type(buffer_size());
	}

	self& operator++() {
		++cur;
		if (cur == last) {
			set_node(node + 1);
			cur = first;
		}
		return *this;
	}
	self operator++(int) {
		self tmp = *this;
		++*this;
		return tmp;
	}

	self& operator--() {
		if (cur == first) {
			set_node(node - 1);
			cur = last;
		}
		--cur;
		return *this;
	}
	self operator--(int) {
		self tmp = *this;
		--*this;
		return tmp;
	}
	
	self& operator+=(difference_type n) {
		// 此处offset以 first 为基准
		difference_type offset = n + (cur - first);
		if (offset >= 0 && offset < difference_type(buffer_size()))
			cur += n;
		else {
			difference_type node_offset =
				offset > 0 ? offset / difference_type(buffer_size())
				: -difference_type((-offset - 1) / buffer_size()) - 1;
			set_node(node + node_offset);
			cur = first + (offset - node_offset * difference_type(buffer_size()));
		}
		return *this;
	}

	self operator+(difference_type n)const{
		self tmp = *this;
		return tmp += n;
	}

	self& operator-=(difference_type n) {
		return *this += -n;
	}

	self operator-(difference_type n) {
		self tmp = *this;
		return tmp += -n;
	}

	// ?
	reference operator[](difference_type n)const {
		return *(*this + n);
	}

	bool operator==(const self& x) { return cur == x.cur; }
	bool operator!=(const self& x) { return !(*this == x); }
	bool operator<(const self& x) {
		return (node == x.node) ? cur < x.cur : node < x.node;
	}
};
//-------------
//template<template <class T, class Ref, class Ptr, size_t BufSize>
//inline random_access_iterator_tag
//iterator_category(const __deque_iterator<T, Ref, Ptr, BufSize>&) {
//	return random_access_iterator_tag();
//}
//
//template<template <class T, class Ref, class Ptr, size_t BufSize>
//inline T* 
//value_type(const __deque_iterator<T, Ref, Ptr, BufSize>&) {
//	return 0;
//}
//
//template <class T, class Ref, class Ptr, size_t BufSize>
//inline ptrdiff_t* distance_type(const __deque_iterator<T, Ref, Ptr, BufSize>&) {
//	return 0;
//}

// deque
template<class T, class Alloc = alloc, size_t BufSize = 0>
class deque {
public:
	typedef T					value_type;
	typedef value_type*			pointer;
	typedef const value_type*	const_pointer;
	typedef value_type&			reference;
	typedef const value_type&	const_reference;
	typedef size_t				size_type;
	typedef ptrdiff_t			difference_type;

	typedef __deque_iterator<T, T&, T*, BufSize>				iterator;
	typedef __deque_iterator<T, const T&, const T*, BufSize>	const_iterator;

	//TODO-----reverse reference

	//

protected:
	typedef pointer*						map_pointer;
	typedef simple_alloc<value_type, Alloc> data_allocator;
	typedef simple_alloc<pointer, Alloc>	map_allocator;

	static size_type buffer_size() {
		return __deque_buf_size(BufSize, sizeof(value_type));
	}
	static size_type initial_map_size() { return 8; }

protected:
	iterator start;
	iterator finish;

	map_pointer map;
	size_type map_size;

public:
	iterator begin() { return start; }
	iterator end() { return finish; }

	const_iterator begin() const { return start; }
	const_iterator end() const { return finish; }

	//TODO reverse

	//

	reference operator[](size_type n) { return start[difference_type(n)]; }//
	const_reference operator[](size_type n) const {
		return start[difference_type(n)];
	}

	reference front() { return *start; }
	reference back() {
		iterator tmp = finish;
		--tmp;
		return *tmp;
	}

	const_reference front() const { return *start; }
	const_reference back() const {
		const_iterator tmp = finish;
		--tmp;
		return *tmp;
	}

	size_type size()const { return finish - start; }
	size_type max_size()const { return size_type(-1); }
	bool empty()const { return finish == start; }

public:
	//constructor and deconstruct
	deque()
		:start(), finish(), map(0), map_size(0) 
	{
		create_map_and_nodes(0);
	}

	deque(const deque& x)
		:start(), finish(), map(0), map_size(0)
	{
		create_map_and_nodes(0);
		try {
			uninitialized_copy(x.begin(), x.end(), start);
		}
		catch (...) {
			destroy_map_and_nodes();
			throw;
		}
	}

	deque(size_type n, const value_type& value)
		: start(), finish(), map(0), map_size(0)
	{
		fill_initialize(n, value);
	}

	deque(int n, const value_type& value)
		: start(), finish(), map(0), map_size(0)
	{
		fill_initialize(n, value);
	}

	deque(long n, const value_type& value)
		: start(), finish(), map(0), map_size(0)
	{
		fill_initialize(n, value);
	}

	explicit deque(size_type n)
		: start(), finish(), map(0), map_size(0)
	{
		fill_initialize(n, value_type());
	}

	deque(const value_type* first, const value_type* last)
		:start(), finish(), map(0), map_size(0)
	{
		create_map_and_nodes(last - first);
		try {
			uninitialized_copy(first, last, start);
		}
		catch (...) {
			destroy_map_and_nodes();
			throw;
		}
	}

	deque(const_iterator first, const_iterator last)
		:start(), finish(), map(0), map_size(0)
	{
		create_map_and_nodes(last - first);
		try {
			uninitialized_copy(first, last, start);
		}
		catch (...) {
			destroy_map_and_nodes();
			throw;
		}
	}

	~deque() {
		destroy(start, finish);
		destroy_map_and_nodes();
	}

//----

	deque& operator=(const deque& x) {
		const size_type len = size();
		if (&x != this) {
			if (len >= x.size())
				erase(copy(x.begin(), x.end(), start), finish);
			else {
				const_iterator mid = x.begin() + difference_type(len);
				copy(x.begin(), mid, start);
				insert(finish, mid, x.end());
			}
		}
		return *this;
	}

	void swap(deque& x) {
		std::swap(start, x.start);
		std::swap(finish, x.finish);
		std::swap(map, x.map);
		std::swap(map_size, x.map_size);
	}

public:			//push_* and pop_*

	void push_back(const value_type& v) {
		if (finish.cur != finish.last - 1) {
			construct(finish.cur, v);
			++finish.cur;
		}
		else {
			push_back_aux(v);
		}
	}

	void push_front(const value_type& v) {
		if (start.cur != start.first) {
			construct(start.cur - 1, v);
			--start.cur;
		}
		else {
			push_front_aux(v);
		}
	}

	void pop_back() {
		if (finish.cur != finish.first) {
			--finish.cur;
			destroy(finish.cur);
		}
		else {
			pop_back_aux();
		}
	}

	void pop_front() {
		if (start.cur != start.last - 1) {
			destroy(start.cur);
			++start.cur;
		}
		else {
			pop_front_aux();
		}
	}

public:	// insert

	iterator insert(iterator position, const value_type& x) {
		if (position.cur == start.cur) {
			push_front(x);
			return start;
		}
		else if (position.cur == finish.cur) {
			push_back(x);
			iterator tmp = finish;
			--tmp;
			return tmp;
		}
		else {
			return insert_aux(position, x);
		}
	}

	iterator insert(iterator position) {
		return insert(position, value_type());
	}

	void insert(iterator pos, size_type n, const value_type& x);

	void insert(iterator pos, int n, const value_type& x) {
		insert(pos, (size_type)n, x);
	}
	void insert(iterator pos, long n, const value_type& x) {
		insert(pos, (size_type)n, x);
	}

#if MEMBER_TEMPLATE
	template<class InputIterator>
	void insert(iterator pos, InputIterator first, InputIterator last) {
		insert(pos, first, last, iterator_category(first));
	}

#endif

	void insert(iterator pos, const value_type* first, const value_type* last);
	void insert(iterator pos, const_iterator first, const_iterator last);

	//
	void resize(size_type new_size, const value_type& x) {
		const size_type len = size();
		if (new_size < len) {
			erase(start + new_size, finish);
		}
		else {
			insert(finish, new_size - len, x);
		}
	}

	void resize(size_type new_size) {
		return resize(new_size, value_type());
	}


public://erase
	iterator erase(iterator pos) {
		iterator next = pos;
		++next;
		difference_type index = pos - start;
		if (index < (size() >> 1)) {
			copy_backward(start, pos, next);
			pop_front();
		}
		else {
			copy(next, finish, pos);
			pop_back();
		}
		return start + index;
	}

	iterator erase(iterator first, iterator last);
	void clear();

protected:		// internal construction/destruction

	void create_map_and_nodes(size_type num_elements);
	void destroy_map_and_nodes();
	void fill_initialize(size_type n, const value_type& value);

#if MEMBER_TEMPLATE

	template<class InputIterator>
	void range_initialize(InputIterator first, InputIterator last, input_iterator_tag);

	template<class ForwardIterator>
	void range_initialize(ForwardIterator first, ForwardIterator last, forward_iterator_tag);

#endif

protected:		// internal push_aux, pop_aux
	void push_back_aux(const value_type& x);
	void push_front_aux(const value_type& x);
	void pop_back_aux();
	void pop_front_aux();

protected:	// internal insert_aux

#if MEMBER_TEMPLATE

	template<class InputIterator>
	void insert(iterator pos, InputIterator first, InputIterator last, input_iterator_tag);

	template<class ForwardIterator>
	void insert(iterator pos, ForwardIterator first, ForwardIterator last, forward_iterator_tag);

#endif

	iterator insert_aux(iterator pos, const value_type& x);
	void insert_aux(iterator pos, size_type n, const value_type& x);

#if MEMBER_TEMPLATE

	template<class ForwardIterator>
	void insert_aux(iterator pos, ForwardIterator first, ForwardIterator last, size_type n);

#endif

	void insert_aux(iterator pos, const value_type* first, const value_type* last, size_type n);

	void insert_aux(iterator pos, const_iterator first, const_iterator last, size_type n);

	
	//在队列头部和尾部预留指定大小的空间
	iterator reserve_elements_at_front(size_type n) {
		size_type vacancies = start.cur - start.first;
		if (n > vacancies) 
			new_elements_at_front(n - vacancies);
		return start - difference_type(n);
	}

	iterator reserve_elements_at_back(size_type n) {
		size_type vacancies = (finish.last - finish.cur) - 1;
		if (n > vacancies)
			new_elements_at_back(n - vacancies);
		return finish + difference_type(n);
	}

	void new_elements_at_front(size_type new_elements);
	void new_elements_at_back(size_type new_elements);

	void destroy_nodes_at_front(iterator before_start);
	void destroy_nodes_at_back(iterator after_finish);

	// allocation of map and nodes
	void reserve_map_at_back(size_type nodes_to_add = 1) {
		if (nodes_to_add + 1 > map_size - (finish.node - map))
			reallocate_map(nodes_to_add, false);
	}
	void reserve_map_at_front(size_type nodes_to_add = 1) {
		if (nodes_to_add > start.node - map)
			reallocate_map(nodes_to_add, true);
	}


	void reallocate_map(size_type nodes_to_add, bool add_at_front);

	pointer allocate_node() { return data_allocator::allocate(buffer_size()); }
	void deallocate_node(pointer n) {
		data_allocator::deallocate(n, buffer_size());
	}

	////..
	//public:
	//	bool operator==(const deque<T, Alloc, 0>& x) const {
	//		return size() == x.size() && equal(begin(), end(), x.begin());
	//	}
	//	bool operator!=(const deque<T, Alloc, 0>& x) const {
	//		return size() != x.size() || !equal(begin(), end(), x.begin());
	//	}
	//	bool operator<(const deque<T, Alloc, 0>& x) const {
	//		return lexicographical_compare(begin(), end(), x.begin(), x.end());
	//	}
	////
};
//-------


// non-inline function

template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::insert(iterator pos, size_type n, const value_type& x) {
	if (pos.cur == start.cur) {
		iterator new_start = reserve_elements_at_front(n);
		uninitialized_fill(new_start, start, x);
		start = new_start;
	}
	else if (pos.cur == finish.cur) {
		iterator new_finish = reserve_elements_at_back(n);
		uninitialized_fill(finish, new_finish, x);
		finish = new_finish;
	}
	else
		insert_aux(pos, n, x);
}

#if MEMBER_TEMPLATE
	template<class T, class Alloc, size_t BufSize>
	void deque<T, Alloc, BufSize>::insert(iterator pos,
		const value_type* first, 
		const value_type* last) 
	{
		size_type n = last - first;
		if (pos.cur == start.cur) {
			iterator new_start = reserve_elements_at_front(n);
			try {
				uninitialized_copy(first, last, new_start);
				start = new_start;
			}
			catch (...) {
				destroy_nodes_at_front(new_start);
				throw;
			}
		}
		else if (pos.cur == finish.cur) {
			iterator new_finish = reserve_elements_at_back(n);
			try {
				uninitialized_copy(first, last, finish);
				finish = new_finish;
			}
			catch (...) {
				destroy_nodes_at_back(new_finish);
				throw;
			}
		}
		else
			insert_aux(pos, first, last, n);
	}
#endif

	
template<class T, class Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::iterator 
deque<T, Alloc, BufSize>::erase(iterator first, iterator last) {
	if (first == start && last == finish) {
		clear();
		return finish;
	}
	else {
		difference_type n = last - first;
		difference_type elems_before = first - start;

		if (elems_before < ((size() - n) >> 1)) {
			copy_backward(start, first, last);
			iterator new_start = start + n;
			destroy(start, new_start);
			for (map_pointer cur = start.node; cur < new_start.node; ++cur)
				data_allocator::deallocate(*cur, buffer_size());
			start = new_start;
		}
		else {
			copy(last, finish, first);
			iterator new_finish = finish - n;
			destroy(new_finish, finish);
			for (map_pointer cur = new_finish.node; cur <= finish.node; ++cur)
				data_allocator::deallocate(*cur, buffer_size());
			finish = new_finish;
		}
		return start + elems_before;
	}
}


template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::clear() {
	for (map_pointer node = start.node + 1; node < finish.node; ++node) {
		destroy(*node, *node + buffer_size());
		data_allocator::deallocate(*node, buffer_size());
	}

	if (start.node != finish.node) {
		destroy(start.cur, start.last);
		destroy(finish.first, finish.cur);
		data_allocator::deallocate(finish.first, buffer_size());
	}
	else 
		destroy(start.cur, finish.cur);

	finish = start;
}



template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::create_map_and_nodes(size_type num_elements) {

	size_type num_nodes = num_elements / buffer_size() + 1;

	map_size = max(initial_map_size(), num_nodes + 2);
	map = map_allocator::allocate(map_size);

	map_pointer node_start = map + (map_size - num_nodes) / 2;
	map_pointer node_finish = node_start + num_nodes - 1;

	map_pointer cur;

	try {
		for (cur = node_start; cur <= node_finish; ++cur)
			*cur = allocate_node();
	}
	catch (...) {
		for (map_pointer n = node_start; n < cur; ++n)
			deallocate_node(*n);
		map_allocator::deallocate(map, map_size);
		throw;
	}

	start.set_node(node_start);
	finish.set_node(node_finish);
	start.cur = start.first;
	finish.cur = finish.first + num_elements % buffer_size();
}

template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::destroy_map_and_nodes() {
	for (map_pointer cur = start.node; cur <= finish.node; ++cur)
		deallocate_node(*cur);
	map_allocator::deallocate(map, map_size);
}

template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::fill_initialize(size_type n, const value_type& value) {
	create_map_and_nodes(n);//根据n个元素创建map和缓存区
	map_pointer cur;
	try {
		for (cur = start.node; cur < finish.node; ++cur)
			uninitialized_fill(*cur, *cur + buffer_size(), value);
		uninitialized_fill(finish.first, finish.cur, value);
	}
	catch (...) {
		for (map_pointer n = start.node; n < cur; ++n)
			destroy(*n, *n + buffer_size());
		destroy_map_and_nodes();
		throw;
	}
}

#if MEMBER_TEMPLATE
template <class T, class Alloc, size_t BufSize>
template <class InputIterator>
void deque<T, Alloc, BufSize>::range_initialize(InputIterator first,
	InputIterator last,
	input_iterator_tag) 
{
	create_map_and_nodes(0);
	for (; first != last; ++first)
		push_back(*first);
}

template <class T, class Alloc, size_t BufSize>
template <class ForwardIterator>
void deque<T, Alloc, BufSize>::range_initialize(ForwardIterator first,
	ForwardIterator last,
	forward_iterator_tag) 
{
	size_type n = distance(first, last);
	create_map_and_nodes(n);
	try{
		uninitialized_copy(first, last, start);
	}
	catch (...) {
		destroy_map_and_nodes();
		throw;
	}
}

#endif // 

template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_back_aux(const value_type& x) {
	value_type x_copy = x;
	reserve_map_at_back();
	*(finish.node + 1) = allocate_node();
	try {
		construct(finish.cur, x_copy);
		finish.set_node(finish.node + 1);
		finish.cur = finish.first;
	}
	catch (...) {
		deallocate_node(*(finish.node + 1));
		throw;
	}
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_front_aux(const value_type& t) {
	value_type t_copy = t;
	reserve_map_at_front();
	*(start.node - 1) = allocate_node();
	try {
		start.set_node(start.node - 1);
		start.cur = start.last - 1;
		construct(start.cur, t_copy);
	}
	catch (...) {
		start.set_node(start.node + 1);
		start.cur = start.first;
		deallocate_node(*(start.node - 1));
		throw;
	}
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::pop_back_aux() {
	deallocate_node(finish.first);
	finish.set_node(finish.node - 1);
	finish.cur = finish.last - 1;
	destroy(finish.cur);
}
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::pop_front_aux() {
	destroy(start.cur);
	deallocate_node(start.first);
	start.set_node(start.node + 1);
	start.cur = start.first;
}

#if MEMBER_TEMPLATE
template <class T, class Alloc, size_t BufSize>
template <class InputIterator>
void deque<T, Alloc, BufSize>::insert(iterator pos,
	InputIterator first, InputIterator last,
	input_iterator_tag) {
	copy(first, last, inserter(*this, pos));
}

template <class T, class Alloc, size_t BufSize>
template <class ForwardIterator>
void deque<T, Alloc, BufSize>::insert(iterator pos,
	ForwardIterator first,
	ForwardIterator last,
	forward_iterator_tag) {
	size_type n = 0;
	distance(first, last, n);
	if (pos.cur == start.cur) {
		iterator new_start = reserve_elements_at_front(n);
		__STL_TRY{
			uninitialized_copy(first, last, new_start);
		start = new_start;
		}
		__STL_UNWIND(destroy_nodes_at_front(new_start));
	}
	else if (pos.cur == finish.cur) {
		iterator new_finish = reserve_elements_at_back(n);
		__STL_TRY{
			uninitialized_copy(first, last, finish);
		finish = new_finish;
		}
		__STL_UNWIND(destroy_nodes_at_back(new_finish));
	}
	else
		insert_aux(pos, first, last, n);
}

#endif

template<class T, class Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::iterator
deque<T, Alloc, BufSize>::insert_aux(iterator pos, const value_type& x) {
	difference_type index = pos - start;

	value_type x_copy = x;
	if (index < (size() >> 1)) {
		push_front(front());
		iterator front1 = start;
		++front1;					// ++ 操作比 +1 简单，具体看函数实现
		iterator front2 = front1;
		++front2;
		pos = start + index;		//??? 为什么要用start + index来得到pos, 而不是直接就用pos
		iterator pos1 = pos;
		++pos1;
		copy(front2, pos1, front1);
	}
	else {
		push_back(back());
		iterator back1 = finish;
		--back1;
		iterator back2 = back1;
		--back2;
		pos = start + index;
		copy_backward(pos, back2, back1);
	}
	*pos = x_copy;
	return pos;
}

//
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::insert_aux(iterator pos, size_type n, const value_type& x) {

	const difference_type elems_before = pos - start;
	size_type length = size();
	value_type x_copy = x;
	if (elems_before < length / 2) {
		iterator new_start = reserve_elements_at_front(n);
		iterator old_start = start;
		pos = start + elems_before;
		try {
			if (elems_before >= difference_type(n)) {
				iterator start_n = start + difference_type(n);
				uninitialized_copy(start, start_n, new_start);
				start = new_start;
				copy(start_n, pos, old_start);
				fill(pos - difference_type(n), pos, x_copy);
			}
			else {
				__uninitialized_copy_fill(start, pos, new_start, start, x_copy);
				start = new_start;
				fill(old_start, pos, x_copy);
			}
		}
		catch (...) { destroy_nodes_at_front(new_start); throw; }
	}
	else {
		iterator new_finish = reserve_elements_at_back(n);
		iterator old_finish = finish;
		const difference_type elems_after = difference_type(length) - elems_before;
		pos = finish - elems_after;
		__STL_TRY{
			if (elems_after > difference_type(n)) {
				iterator finish_n = finish - difference_type(n);
				uninitialized_copy(finish_n, finish, finish);
				finish = new_finish;
				copy_backward(pos, finish_n, old_finish);
				fill(pos, pos + difference_type(n), x_copy);
			}
			else {
				__uninitialized_fill_copy(finish, pos + difference_type(n),
					x_copy,
					pos, finish);
				finish = new_finish;
				fill(pos, old_finish, x_copy);
			}
		}
		catch (...) { destroy_nodes_at_back(new_finish); throw; }
	}

}

#if MEMBER_TEMPLATE

template <class T, class Alloc, size_t BufSize>
template <class ForwardIterator>
void deque<T, Alloc, BufSize>::insert_aux(iterator pos,
	ForwardIterator first,
	ForwardIterator last,
	size_type n)
{
	const difference_type elems_before = pos - start;
	size_type length = size();
	if (elems_before < length / 2) {
		iterator new_start = reserve_elements_at_front(n);
		iterator old_start = start;
		pos = start + elems_before;
		__STL_TRY{
			if (elems_before >= difference_type(n)) {
				iterator start_n = start + difference_type(n);
				uninitialized_copy(start, start_n, new_start);
				start = new_start;
				copy(start_n, pos, old_start);
				copy(first, last, pos - difference_type(n));
			}
			else {
				ForwardIterator mid = first;
				advance(mid, difference_type(n) - elems_before);
				__uninitialized_copy_copy(start, pos, first, mid, new_start);
				start = new_start;
				copy(mid, last, old_start);
			}
		}
		__STL_UNWIND(destroy_nodes_at_front(new_start));
	}
	else {
		iterator new_finish = reserve_elements_at_back(n);
		iterator old_finish = finish;
		const difference_type elems_after = difference_type(length) - elems_before;
		pos = finish - elems_after;
		__STL_TRY{
			if (elems_after > difference_type(n)) {
				iterator finish_n = finish - difference_type(n);
				uninitialized_copy(finish_n, finish, finish);
				finish = new_finish;
				copy_backward(pos, finish_n, old_finish);
				copy(first, last, pos);
			}
			else {
				ForwardIterator mid = first;
				advance(mid, elems_after);
				__uninitialized_copy_copy(mid, last, pos, finish, finish);
				finish = new_finish;
				copy(first, mid, pos);
			}
		}
		__STL_UNWIND(destroy_nodes_at_back(new_finish));
	}
}
#endif

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::insert_aux(iterator pos,
	const value_type* first,
	const value_type* last,
	size_type n)
{
	const difference_type elems_before = pos - start;
	size_type length = size();
	if (elems_before < length / 2) {
		iterator new_start = reserve_elements_at_front(n);
		iterator old_start = start;
		pos = start + elems_before;
		__STL_TRY{
			if (elems_before >= difference_type(n)) {
				iterator start_n = start + difference_type(n);
				uninitialized_copy(start, start_n, new_start);
				start = new_start;
				copy(start_n, pos, old_start);
				copy(first, last, pos - difference_type(n));
			}
			else {
				const value_type* mid = first + (difference_type(n) - elems_before);
				__uninitialized_copy_copy(start, pos, first, mid, new_start);
				start = new_start;
				copy(mid, last, old_start);
			}
		}
		__STL_UNWIND(destroy_nodes_at_front(new_start));
	}
	else {
		iterator new_finish = reserve_elements_at_back(n);
		iterator old_finish = finish;
		const difference_type elems_after = difference_type(length) - elems_before;
		pos = finish - elems_after;
		__STL_TRY{
			if (elems_after > difference_type(n)) {
				iterator finish_n = finish - difference_type(n);
				uninitialized_copy(finish_n, finish, finish);
				finish = new_finish;
				copy_backward(pos, finish_n, old_finish);
				copy(first, last, pos);
			}
			else {
				const value_type* mid = first + elems_after;
				__uninitialized_copy_copy(mid, last, pos, finish, finish);
				finish = new_finish;
				copy(first, mid, pos);
			}
		}
		__STL_UNWIND(destroy_nodes_at_back(new_finish));
	}
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::insert_aux(iterator pos,
	const_iterator first,
	const_iterator last,
	size_type n)
{
	const difference_type elems_before = pos - start;
	size_type length = size();
	if (elems_before < length / 2) {
		iterator new_start = reserve_elements_at_front(n);
		iterator old_start = start;
		pos = start + elems_before;
		try{
			if (elems_before >= n) {
				iterator start_n = start + n;
				uninitialized_copy(start, start_n, new_start);
				start = new_start;
				copy(start_n, pos, old_start);
				copy(first, last, pos - difference_type(n));
			}
			else {
				const_iterator mid = first + (n - elems_before);
				__uninitialized_copy_copy(start, pos, first, mid, new_start);
				start = new_start;
				copy(mid, last, old_start);
			}
		}
		catch (...) { destroy_nodes_at_front(new_start); throw; }
	}
	else {
		iterator new_finish = reserve_elements_at_back(n);
		iterator old_finish = finish;
		const difference_type elems_after = length - elems_before;
		pos = finish - elems_after;
		try{
			if (elems_after > n) {
				iterator finish_n = finish - difference_type(n);
				uninitialized_copy(finish_n, finish, finish);
				finish = new_finish;
				copy_backward(pos, finish_n, old_finish);
				copy(first, last, pos);
			}
			else {
				const_iterator mid = first + elems_after;
				__uninitialized_copy_copy(mid, last, pos, finish, finish);
				finish = new_finish;
				copy(first, mid, pos);
			}
		}
		catch (...) { destroy_nodes_at_back(new_finish); throw; }
	}
}

template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::new_elements_at_front(size_type new_elements) {
	size_type new_nodes = (new_elements + buffer_size() - 1) / buffer_size();
	reserve_map_at_front(new_nodes);
	size_type i;
	try {
		for (i = 1; i <= new_nodes; ++i)
			*(start.node - i) = allocate_node();
	}
	catch (...) {
		for (size_type j = 1; j < i; ++j)
			deallocate_node(*(start.node - j));
		throw;
	}
}

template<class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::new_elements_at_back(size_type new_elements) {
	size_type new_nodes = (new_elements + buffer_size() - 1) / buffer_size();
	reserve_elements_at_back(new_nodes);
	size_type i;
	try {
		for (i = 1; i <= new_nodes; ++i)
			*(finish.node + i) = allocate_node();
	}
	catch (...) {
		for (size_type j = 1; j < i; ++j)
			deallocate_node(*(finish.node + j));
		throw;
	}
}


template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::destroy_nodes_at_front(iterator before_start) {
	for (map_pointer n = before_start.node; n < start.node; ++n)
		deallocate_node(*n);
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::destroy_nodes_at_back(iterator after_finish) {
	for (map_pointer n = after_finish.node; n > finish.node; --n)
		deallocate_node(*n);
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::reallocate_map(size_type nodes_to_add, bool add_at_front) {

	size_type old_num_nodes = finish.node - start.node + 1;
	size_type new_num_nodes = old_num_nodes + nodes_to_add;

	map_pointer new_node_start;
	if (map_size > 2 * new_num_nodes) {
		new_node_start = map + (map_size - new_num_nodes) / 2
			+ (add_at_front ? nodes_to_add : 0);
		if (new_node_start < start.node)
			copy(start.node, finish.node + 1, new_node_start);
		else
			copy_backward(start.node, finish.node + 1, new_node_start + old_num_nodes);
	}
	else {
		size_type new_map_size = map_size + max(map_size, nodes_to_add) + 2;

		map_pointer new_map = map_allocator::allocate(new_map_size);
		new_node_start = new_map + (new_map_size - new_num_nodes) / 2
			+ (add_at_front ? nodes_to_add : 0);
		copy(start.node, finish.node + 1, new_node_start);
		map_allocator::deallocate(map, map_size);

		map = new_map;
		map_size = new_map_size;
	}
	start.set_node(new_node_start);
	finish.set_node(new_node_start + old_num_nodes + 1);
}

template <class T, class Alloc, size_t BufSiz>
bool operator==(const deque<T, Alloc, BufSiz>& x,
	const deque<T, Alloc, BufSiz>& y) {
	return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
}
template <class T, class Alloc, size_t BufSiz>
bool operator!=(const deque<T, Alloc, BufSiz>& x,
	const deque<T, Alloc, BufSiz>& y) {
	return !(x == y);
}

template <class T, class Alloc, size_t BufSiz>
bool operator<(const deque<T, Alloc, BufSiz>& x,
	const deque<T, Alloc, BufSiz>& y) {
	return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}









HTL_NS_END

#endif // ! _HTL_DEQUE_H
