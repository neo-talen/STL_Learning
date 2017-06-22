#pragma once

#ifndef _HTL_QUEUE_H
#define _HTL_QUEUE_H

#include"htl_micros.h"
#include"htl_deque.h"
#include"htl_vector.h"
#include"htl_function.h"

HTL_NS_BEGIN

template<class T, class Sequence = deque<T>>
class queue {
	friend operator==(const queue&, const queue&);
	friend operator<(const queue&, const queue&);

public:
	typedef typename Sequence::value_type		value_type;
	typedef typename Sequence::size_type		size_type;
	typedef typename Sequence::reference		reference;
	typedef typename Sequence::const_reference	const_reference;

protected:
	Sequence c;

public:
	bool empty() const { return c.empty(); }
	size_type size() const { return c.size(); }
	reference front() { return c.front(); }
	const_reference front() { return c.front(); }
	reference back() { return c.back(); }
	const_reference back() { return c.back(); }

	void push(const value_type& x) {
		c.push_back(x);
	}
	void pop() {
		c.pop_front();
	}

public:
	const Sequence& Container() const { return c; }
};

template<class T, class Sequence>
bool operator==(const queue<T, Sequence>& x, const queue<T, Sequence>& y) {
	return x.c == y.c;
}

template<class T, class Sequence>
bool operator<(const queue<T, Sequence>& x, const queue<T, Sequence>& y) {
	return x.c < y.c;
}

// priority_queue

template<class T, class Sequence = MyVector<T>, class Compare = less<typename Sequence::value_type> >
class priority_queue {
public:
	typedef typename Sequence::value_type		value_type;
	typedef typename Sequence::size_type		size_type;
	typedef typename Sequence::reference		reference;
	typedef typename Sequence::const_reference	const_reference;

protected:
	Sequence c;
	Compare  comp;

public:
	priority_queue():c(){}
	explicit priority_queue(const Compare& x):c(), comp(x){}

#if MEMBER_TEMPLATE
	template <class InputIterator>
	priority_queue(InputIterator first, InputIterator last, const Compare& x)
		: c(first, last), comp(x) {
		make_heap(c.begin(), c.end(), comp);
	}
	template <class InputIterator>
	priority_queue(InputIterator first, InputIterator last)
		: c(first, last) {
		make_heap(c.begin(), c.end(), comp);
	}
#endif
	priority_queue(const value_type* first, const value_type* last,
		const Compare& x) : c(first, last), comp(x) {
		make_heap(c.begin(), c.end(), comp);
	}
	priority_queue(const value_type* first, const value_type* last)
		: c(first, last) {
		make_heap(c.begin(), c.end(), comp);
	}

	bool empty() const { return c.empty(); }
	size_type size() const { return c.size(); }
	const_reference top() const { return c.front(); }
	void push(const value_type& x) {
		try{
			c.push_back(x);
			push_heap(c.begin(), c.end(), comp);
		}
		catch (...) { c.clear(); throw; }
	}
	void pop() {
		try{
			pop_heap(c.begin(), c.end(), comp);
			c.pop_back();
		}
		catch (...) {
			c.clear(); throw;
		}
	}

};



HTL_NS_END

#endif // ! _HTL_QUEUE_H
