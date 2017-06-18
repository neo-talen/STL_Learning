#pragma once

#ifndef _HTL_MYFUNCS_H
#define _HTL_MYFUNCS_H

#include<ostream>
using std::ostream;

template<class InputIterator>
void Print(InputIterator first, InputIterator last, ostream& out, const char* seq = " ") {
	while (first != last) {
		out << *first++ << seq;
	}
	out << "\n";
}



#endif