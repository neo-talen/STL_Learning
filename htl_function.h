#pragma once

#ifndef _HTL_MYFUNCS_H
#define _HTL_MYFUNCS_H

#include"htl_micros.h"

HTL_NS_BEGIN

template <class T>
struct less : public binary_function<T, T, bool> {
	bool operator()(const T& x, const T& y) const { return x < y; }
};





HTL_NS_END

#endif