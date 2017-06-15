#pragma once

#ifndef  _HTL_DEQUE_H
#define _HTL_DEQUE_H

#include"htl_micros.h"

HTL_NS_BEGIN


// return number of objs a buffer, sz is sizeof(T).
// if set n, return n; ifnot return 512Byte/sz or 1(sz > 512B); 
inline size_t __deque_buf_size(size_t n, size_t sz) {
	return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : 1);
}

template<class T>










HTL_NS_END

#endif // ! _HTL_DEQUE_H
