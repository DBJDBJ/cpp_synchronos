#pragma once

#include <iostream>
#include <iomanip>
#include <utility>
#include <tuple>        // std::tuple, std::get, std::tie, std::ignore

namespace dbj {

	//-------------------------------------------------------------------//
	// predefined macros report
	//-------------------------------------------------------------------//
	namespace {
		template<class T, class V >
		inline std::ostream& operator<<(std::ostream& os, std::tuple<T, V> & e)
		{
			return os << std::setw(4) << "{ " << std::setw(18) << std::get<0>(e) << " : " << std::setw(18) << std::get<1>(e) << "}" << std::endl;
		}
	}

#define SHOW(X) std::make_tuple(#X,X)
#define NOT(X) std::make_tuple(#X,"undefined")

	inline void predefined_macros_report(std::ostream & OS = std::cout) {

		OS << std::endl;
#if defined( __cplusplus_cli )
		OS << SHOW(__cplusplus_cli);
#else
		OS << NOT(__cplusplus_cli);
#endif  
#ifdef __FILE__
		OS << SHOW(__FILE__);
#endif
#ifdef __LINE__
		OS << SHOW(__LINE__);
#endif
#ifdef __STDC__
		OS << SHOW(__STDC__);
#else
		OS << NOT(__STDC__);
#endif
#ifdef __TIME__
		OS << SHOW(__TIME__);
#endif
#ifdef __DATE__
		OS << SHOW(__DATE__);
#endif
#ifdef __TIMESTAMP__
		OS << SHOW(__TIMESTAMP__);
#else
		OS << NOT(__TIMESTAMP__);
#endif
		OS << std::endl;
	}


	// Variadic templates ?

	void f() { }     // do nothing

	template<typename T, typename ... Tail>
	void f(T head, Tail ... tail)
	{
		g(head);    // do something to head
		f(tail...); // try again with tail
	}

} // dbj space