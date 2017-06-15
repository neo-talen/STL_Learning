#pragma once

#ifndef _HTL_ALLOC_H
#define _HTL_ALLOC_H

#include"htl_micros.h"
#include<new>		// for placement new
#include<cstddef>	// for ptrdiff_t, size_t
#include<cstdlib>	// for exit()
#include<climits>	// for UINT_MAX
#include<iostream>	// for cerr

#include"htl_type_traits.h"
#include"htl_iterator.h"
HTL_NS_BEGIN

//

template<int inst>
class __malloc_alloc_template {
private:
	static void *oom_malloc(size_t);
	static void *oom_realloc(void*, size_t);

	static void(*__malloc_alloc_oom_handler)();

public:
	static void* allocate(size_t n) {
		void* result = malloc(n);
		if (0 == result) result = oom_malloc(n);
		return result;
	}
	static void deallocate(void* p, size_t n) {
			free(p);
	}

	static void* reallocate(void* p, size_t, size_t new_size) {
		void* result = realloc(p, new_size);
		if (0 == result) result = oom_realloc(p, new_size);
		return result;
	}

	static void(*set_malloc_handler(void(*f)()))(){
		void(*old)() = __malloc_alloc_oom_handler;
		__malloc_alloc_oom_handler = f;
		return old;
	}
};
template<int inst>
void(*__malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

template<int inst>
void* __malloc_alloc_template<inst>::oom_malloc(size_t n) {
	void(*my_malloc_handler)();
	void* result;
	for (;;) {
		my_malloc_handler = __malloc_alloc_oom_handler;
		if (0 == my_malloc_handler) { std::cerr << "bad malloc\n"; exit(1); }
		(*my_malloc_handler)();
		result = malloc(n);
		if (result)return result;
	}
}

template <int inst>
void * __malloc_alloc_template<inst>::oom_realloc(void *p, size_t n)
{
	void(*my_malloc_handler)();
	void *result;

	for (;;) {
		my_malloc_handler = __malloc_alloc_oom_handler;
		if (0 == my_malloc_handler) { cerr << "bad realloc\n"; exit(1); }
		(*my_malloc_handler)();
		result = realloc(p, n);
		if (result) return(result);
	}
}
//////////////////////////////////////////////////////////////////////
typedef __malloc_alloc_template<0> malloc_alloc;

template<class T, class Alloc>
class simple_alloc {
public:
	static T* allocate(size_t n) {
#if DEBUG_FLAG 
		std::cout << "alloc "<< n <<" objects space...\n";
#endif
		return 0 == n ? 0 : (T*)Alloc::allocate(n * sizeof(T));
	}
	static T* allocate(void) {
#if DEBUG_FLAG 
		std::cout << "alloc " <<1<<" object space...\n";
#endif
		return (T*)Alloc::allocate(sizeof(T));
	}
	static void deallocate(T* p, size_t n) {
#if DEBUG_FLAG 
		std::cout << "dealloc " <<n<<" objects space...\n";
#endif
		if (0 != n)Alloc::deallocate(p, n * sizeof(T));
#if DEBUG_FLAG 
		std::cout << "dealloc " << n << " objects space..successed.\n";
#endif
	}
	static void deallocate(T* p) {
#if DEBUG_FLAG 
		std::cout << "dealloc " <<1<<" object space...\n";
#endif
		Alloc::deallocate(p, sizeof(T));
	}
};
//user typedef
typedef malloc_alloc alloc;

///////////////////constructor and destructor/////////////////////////
template<class T1, class T2>
inline void construct(T1* p, const T2& value){
#if DEBUG_FLAG 
	std::cout << "constructing(1)......\n";
#endif
	new(p) T1(value);
}

template<class T>
inline void destroy(T* ptr) {
#if DEBUG_FLAG 
	std::cout << "deconstructing(1)......\n";
#endif
	ptr->~T();
}

template<class ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __false_type) {
#if DEBUG_FLAG 
	std::cout << "nontrivial deconstructing(n).....__false_type.\n";
#endif
	for (; first != last; ++first)
		destroy(&*first);
}

template<class ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __true_type) {
#if DEBUG_FLAG 
	std::cout << "trivial deconstructing(n).....__true_type.\n";
#endif
}

template<class ForwardIterator, class T>
inline void __destroy(ForwardIterator first, ForwardIterator last, T*) {
	typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
	__destroy_aux(first, last, trivial_destructor());
}

template<class ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
	__destroy(first, last, value_type(first));
}

inline void destroy(char*, char*){
#if DEBUG_FLAG 
	std::cout << "trivial deconstructing(n).....speciallized char.\n";
#endif
}
inline void destroy(wchar_t*, wchar_t*){
#if DEBUG_FLAG 
	std::cout << "trivial deconstructing(n).....speciallized wchar_t.\n";
#endif
}


HTL_NS_END

#endif