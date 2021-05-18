#ifndef LIBTEST_HPP
#define LIBTEST_HPP

#include <boost/config.hpp>

#include <iosfwd>
#include <string>


#if defined(LIB_SOURCE)
#	define LIB_DECL BOOST_SYMBOL_EXPORT
#else
#	define LIB_DECL BOOST_SYMBOL_IMPORT
#endif




#endif // LIBTEST_HPP
