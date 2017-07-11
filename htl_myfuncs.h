#pragma once

#ifndef _HTL_MYFUNCS_H
#define _HTL_MYFUNCS_H

#include<iostream>
using std::ostream;
using std::cout;
template<class InputIterator>
void Print(InputIterator first, InputIterator last, ostream& out, const char* seq = " ") {
	while (first != last) {
		out << *first++ << seq;
	}
	out << "\n";
}

template<class Container>
inline void Print(const Container& c) {
	Print(c.begin(), c.end(), cout);
}



#endif