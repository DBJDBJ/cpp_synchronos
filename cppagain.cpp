// cppagain.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <string>
#include "dbj11.h"

template<typename T > class IS {
public:
	enum { here = true } ;
} ;

template<> class IS<nullptr_t> {
	enum { here = false };
};

//-------------------------------------------------------------------//
// from this: 
// typedef wchar_t     _TCHAR;
// to this :
using _TCHAR = wchar_t     ;
// that is freedom ...
//-------------------------------------------------------------------//
int _tmain(int argc, _TCHAR* argv[])
{
	dbj::predefined_macros_report();
	return 0;
}

