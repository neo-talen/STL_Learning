#pragma once

#ifndef _HTL_LIST_H
#define _HTL_LIST_H

#include"htl_micros.h"
#include"htl_iterator.h"
#include"htl_allocator.h"

HTL_NS_BEGIN

template<class T>
struct __list_node {
	typedef void* void_pointer;
	void_pointer next;
	void_pointer prev;
	T	data;
};


template<class T, class Ref, class Ptr>
struct __list_iterator {
	typedef __list_iterator<T, T&, T*>				iterator;
	typedef __list_iterator<T, const T&, const T*>	const_iterator;
	typedef __list_iterator<T, Ref, Ptr>			self;

	typedef bidirection_iterator_tag				iterator_category;
	typedef T										value_type;
	typedef Ptr										pointer;
	typedef Ref										reference;
	typedef __list_node<T>*							link_type;
	typedef size_t									size_type;
	typedef ptrdiff_t								difference_type;

	link_type node;

	//constructor
	__list_iterator(link_type x):node(x){}
	__list_iterator() {}
	__list_iterator(const iterator& x):node(x.node){}

	//operators
	bool operator==(const self& x)const { return node == x.node; }
	bool operator!=(const self& x)const { return node != x.node; }
	reference operator*()const { return (*node).data; }

		//operator ->
	pointer operator->()const { return &(operator*()); }

	self& operator++() {
		node = (link_type)((*node).next);
		return *this;
	}
	self operator++(int) {
		self tmp = *this;
		++*this;
		return tmp;
	}

	self& operator--() {
		node = (link_type)((*node).prev);
		return *this;
	}
	self operator--(int) {
		self tmp = *this;
		--*this;
		return tmp;
	}
};

// partial specialization
template<class T, class Ref, class Ptr>
inline bidirection_iterator_tag iterator_category(const __list_iterator<T, Ref, Ptr>&) {
	return bidirection_iterator_tag();
}

template<class T, class Ref, class Ptr>
inline T* value_type(const __list_iterator<T, Ref, Ptr>&) {
	return 0;
}

template <class T, class Ref, class Ptr>
inline ptrdiff_t*
distance_type(const __list_iterator<T, Ref, Ptr>&) {
	return 0;
}
//

template<class T, class Alloc = alloc>
class MyList {
protected:
	typedef void*							void_pointer;
	typedef __list_node<T>					list_node;
	typedef simple_alloc<list_node, Alloc>	list_node_allocator;

public:
	typedef T				value_type;
	typedef T*				pointer;
	typedef const T*		const_pointer;
	typedef T&				reference;
	typedef const T&		const_reference;
	typedef list_node*		link_type;
	typedef size_t			size_type;
	typedef ptrdiff_t		difference_type;
	
public:
	typedef __list_iterator<T, T&, T*>				iterator;
	typedef __list_iterator<T, const T&, const T*>	const_iterator;

	//reverse reference TODO
	
	//

protected:
	link_type get_node() { return list_node_allocator::allocate(); }
	void put_node(link_type p) { list_node_allocator::deallocate(p); }

	link_type create_node(const T& x) {
		link_type p = get_node();
		try{
			construct(&p->data, x);
		}
		catch (...) {
			put_node(p);
			throw;
		}
		return p;
	}

	void destroy_node(link_type p) {
		destroy(&p->data);
		put_node(p);
	}

protected:
	void empty_initialize() {
		node = get_node();
		node->next = node;
		node->prev = node;
	}

	void fill_initialize(size_type n, const T& value) {
		empty_initialize();
		try {
			insert(begin(), n, value);
		}
		catch (...) {
			clear(); put_node(node); throw;
		}
	}

#if MEMBER_TEMPLATE
	template<class InputIterator>
	void range_initialize(InputIterator first, InputIterator last) {
		empty_initialize();
		try {
			insert(begin(), first, last);
		}
		catch (...) {
			clear(); put_node(node); throw;
		}
	}
#endif

	//用一段数据初始化MyList
	void range_initialize(const T* first, const T* last) {
		empty_initialize();
		try {
			insert(begin(), first, last);
		}
		catch (...) {
			clear(); put_node(node); throw;
		}
	}
	//用一段MyList初始化MyList
	void range_initialize(const_iterator first, const_iterator last) {
		empty_initialize();
		try {
			insert(begin(), first, last);
		}
		catch (...) {
			clear(); put_node(node); throw;
		}
	}

protected:
	link_type node;

public:
	MyList() { empty_initialize(); }
	
	iterator begin() { return (link_type)((*node).next); }
	const_iterator begin()const { return (link_type)((*node).next); }

	iterator end() { return node; }
	const_iterator end() const { return node; }

	//reverse_iterator TODO

	//

	bool empty() const { return node->next == node; }
	size_type size()const {
		size_type result = distance(begin(), end());
		return result;
	}
	size_type max_size()const { return size_type(-1); }
	
	reference front() { return *begin(); }
	const_reference front() const { return *begin(); }

	reference back() { return *(--end()); }
	const_reference back()const { return *(--end()); }

	void swap(MyList<T, Alloc>& x) { std::swap(node, x.node); }

public:
	iterator insert(iterator position, const T& x) {
		link_type tmp = create_node(x);
		tmp->next = position.node;
		tmp->prev = position.node->prev;
		(link_type(position.node->prev))->next = tmp;
		position.node->prev = tmp;
		return tmp;
	}
	iterator insert(iterator position) { return insert(position, T()); }

#if MEMBER_TEMPLATE
	template<class InputIterator>
	void insert(iterator position, InputIterator first, InputIterator last);
#endif
	void insert(iterator position, const T* first, const T* last);
	void insert(iterator position, const_iterator first, const_iterator last);

	void insert(iterator position, size_type n, const T& x);
	void insert(iterator position, int n, const T& x) {
		insert(position, (size_type)n, x);
	}

	void push_front(const T& x) {
		insert(begin(), x);
	}
	void push_back(const T& x) {
		insert(end(), x);
	}

	iterator erase(iterator position) {
		link_type next_node = link_type(position.node->next);
		link_type prev_node = link_type(position.node->prev);

		prev_node->next = next_node;
		next_node->prev = prev_node;
		destroy_node(position.node);
		return iterator(next_node);
	}
	iterator erase(iterator first, iterator last);

	void resize(size_type new_size, const T& x);
	void resize(size_type new_size) { resize(new_size, T()); }
	void clear();

	void pop_front() { erase(begin()); }
	void pop_back() {
		iterator tmp = end();
		erase(--tmp);
	}


	MyList(size_type n, const T& value) { fill_initialize(n, value); }
	MyList(int n, const T& value) { fill_initialize(n, value); }
	MyList(long n, const T& value) { fill_initialize(n, value); }
	explicit MyList(size_type n) { fill_initialize(n, T()); }
#if MEMBER_TEMPLATE
	template <class InputIterator>
	MyList(InputIterator first, InputIterator last) {
		range_initialize(first, last);
	}
#endif
	MyList(const T* first, const T* last) { range_initialize(first, last); }
	MyList(const_iterator first, const_iterator last) {
		range_initialize(first, last);
	}

	MyList(const MyList<T, Alloc>& x) {
		range_initialize(x.begin(), x.end());
	}
	~MyList() {
		clear();
		put_node(node);
	}
	MyList<T, Alloc>& operator=(const MyList<T, Alloc>& x);

protected:
	void transfer(iterator position, iterator first, iterator last) {
		if (position != last) {
			link_type(last.node->prev)->next = position.node;
			link_type(first.node->prev)->next = last.node;
			link_type(position.node->pre)->next = first.node;
			link_type tmp = link_type(position.node->prev);
			position.node->prev = last.node->prev;
			last.node->prev = first.node->prev;
			first.node->prev = tmp;
		}
	}

public:
	void splice(iterator position, MyList& x) {
		if (!x.empty())
			transfer(position, x.begin(), x.end());
	}

	void splice(iterator position, MyList&, iterator i) {
		iterator j = i;
		++j;
		if (position == i || position == j)return;
		transfer(position, i, j);
	}

	void splice(iterator position, MyList&, iterator first, iterator last) {
		if (first != last)
			transfer(position, first, last);
	}

	void remove(const T& value);
	void unique();
	void merge(MyList& x);
	void reverse();
	void sort();

#if MEMBER_TEMPLATE
	template <class Predicate> void remove_if(Predicate);
	template <class BinaryPredicate> void unique(BinaryPredicate);
	template <class StrictWeakOrdering> void merge(MyList&, StrictWeakOrdering);
	template <class StrictWeakOrdering> void sort(StrictWeakOrdering);
#endif

	friend bool operator==(const MyList& x, const MyList& y);
};

//my
template<class T, class Alloc>
inline bool operator==(const MyList<T, Alloc>& lhs, const MyList<T, Alloc>& rhs) {
	if (lhs.size() != rhs.begin()) return false;
	typedef typename MyList<T, Alloc>::iterator iterator;
	iterator left = lhs.begin();
	iterator right = rhs.begin();
	
	while (*left == *right) { ++left, ++right; }
	if (left != lhs.end())
		return false;
	return true;
}

template <class T, class Alloc>
inline bool operator<(const MyList<T, Alloc>& x, const MyList<T, Alloc>& y) {
	return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template <class T, class Alloc>
inline void swap(MyList<T, Alloc>& x, MyList<T, Alloc>& y) {
	x.swap(y);
}

#if MEMBER_TEMPLATE
template <class T, class Alloc> template <class InputIterator>
void MyList<T, Alloc>::insert(iterator position,
	InputIterator first, InputIterator last) {
	for (; first != last; ++first)
		insert(position, *first);
}
#endif

template <class T, class Alloc>
void MyList<T, Alloc>::insert(iterator position, const T* first, const T* last) {
	for (; first != last; ++first)
		insert(position, *first);
}

template <class T, class Alloc>
void MyList<T, Alloc>::insert(iterator position, const_iterator first, const_iterator last) {
	for (; first != last; ++first)
		insert(position, *first);
}


template <class T, class Alloc>
void MyList<T, Alloc>::insert(iterator position, size_type n, const T& x) {
	for (; n > 0; --n)
		insert(position, x);
}

template <class T, class Alloc>
typename MyList<T, Alloc>::iterator MyList<T, Alloc>::erase(iterator first, iterator last) {
	while (first != last) erase(first++);
	return last;
}

template <class T, class Alloc>
void MyList<T, Alloc>::resize(size_type new_size, const T& x)
{
	iterator i = begin();
	size_type len = 0;
	for (; i != end() && len < new_size; ++i, ++len)
		;
	if (len == new_size)
		erase(i, end());
	else                          // i == end()
		insert(end(), new_size - len, x);
}

template <class T, class Alloc>
void MyList<T, Alloc>::clear(){
	link_type cur = (link_type)node->next;
	while (cur != node) {
		link_type tmp = cur;
		cur = (link_type)cur->next;
		destroy_node(tmp);
	}
	node->next = node;
	node->prev = node;
}

template <class T, class Alloc>
MyList<T, Alloc>& MyList<T, Alloc>::operator=(const MyList<T, Alloc>& x) {
	if (this != &x) {
		iterator first1 = begin();
		iterator last1 = end();
		const_iterator first2 = x.begin();
		const_iterator last2 = x.end();
		while (first1 != last1 && first2 != last2) *first1++ = *first2++;
		if (first2 == last2)
			erase(first1, last1);
		else
			insert(last1, first2, last2);
	}
	return *this;
}

template <class T, class Alloc>
void MyList<T, Alloc>::remove(const T& value) {
	iterator first = begin();
	iterator last = end();
	while (first != last) {
		iterator next = first;
		++next;
		if (*first == value) erase(first);
		first = next;
	}
}

template <class T, class Alloc>
void MyList<T, Alloc>::unique() {
	iterator first = begin();
	iterator last = end();
	if (first == last) return;
	iterator next = first;
	while (++next != last) {
		if (*first == *next)
			erase(next);
		else
			first = next;
		next = first;
	}
}

template <class T, class Alloc>
void MyList<T, Alloc>::merge(MyList<T, Alloc>& x) {
	iterator first1 = begin();
	iterator last1 = end();
	iterator first2 = x.begin();
	iterator last2 = x.end();
	while (first1 != last1 && first2 != last2)
		if (*first2 < *first1) {
			iterator next = first2;
			transfer(first1, first2, ++next);
			first2 = next;
		}
		else
			++first1;
	if (first2 != last2) transfer(last1, first2, last2);
}

template <class T, class Alloc>
void MyList<T, Alloc>::reverse() {
	if (node->next == node || link_type(node->next)->next == node) return;
	iterator first = begin();
	++first;
	while (first != end()) {
		iterator old = first;
		++first;
		transfer(begin(), old, first);
	}
}

template <class T, class Alloc>
void MyList<T, Alloc>::sort() {
	if (node->next == node || link_type(node->next)->next == node) return;
	MyList<T, Alloc> carry;
	MyList<T, Alloc> counter[64];
	int fill = 0;
	while (!empty()) {
		carry.splice(carry.begin(), *this, begin());
		int i = 0;
		while (i < fill && !counter[i].empty()) {
			counter[i].merge(carry);
			carry.swap(counter[i++]);
		}
		carry.swap(counter[i]);
		if (i == fill) ++fill;
	}

	for (int i = 1; i < fill; ++i) counter[i].merge(counter[i - 1]);
	swap(counter[fill - 1]);
}

#if MEMBER_TEMPLATE
template <class T, class Alloc> template <class Predicate>
void MyList<T, Alloc>::remove_if(Predicate pred) {
	iterator first = begin();
	iterator last = end();
	while (first != last) {
		iterator next = first;
		++next;
		if (pred(*first)) erase(first);
		first = next;
	}
}

template <class T, class Alloc> template <class BinaryPredicate>
void MyList<T, Alloc>::unique(BinaryPredicate binary_pred) {
	iterator first = begin();
	iterator last = end();
	if (first == last) return;
	iterator next = first;
	while (++next != last) {
		if (binary_pred(*first, *next))
			erase(next);
		else
			first = next;
		next = first;
	}
}

template <class T, class Alloc> template <class StrictWeakOrdering>
void MyList<T, Alloc>::merge(MyList<T, Alloc>& x, StrictWeakOrdering comp) {
	iterator first1 = begin();
	iterator last1 = end();
	iterator first2 = x.begin();
	iterator last2 = x.end();
	while (first1 != last1 && first2 != last2)
		if (comp(*first2, *first1)) {
			iterator next = first2;
			transfer(first1, first2, ++next);
			first2 = next;
		}
		else
			++first1;
	if (first2 != last2) transfer(last1, first2, last2);
}

template <class T, class Alloc> template <class StrictWeakOrdering>
void MyList<T, Alloc>::sort(StrictWeakOrdering comp) {
	if (node->next == node || link_type(node->next)->next == node) return;
	list<T, Alloc> carry;
	list<T, Alloc> counter[64];
	int fill = 0;
	while (!empty()) {
		carry.splice(carry.begin(), *this, begin());
		int i = 0;
		while (i < fill && !counter[i].empty()) {
			counter[i].merge(carry, comp);
			carry.swap(counter[i++]);
		}
		carry.swap(counter[i]);
		if (i == fill) ++fill;
	}

	for (int i = 1; i < fill; ++i) counter[i].merge(counter[i - 1], comp);
	swap(counter[fill - 1]);
}

#endif 


HTL_NS_END

#endif