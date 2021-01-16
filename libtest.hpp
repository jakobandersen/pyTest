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

LIB_DECL void f();


struct LIB_DECL Exception : public std::exception {
protected:
	Exception(std::string &&text) : text(text) {}
public:
	virtual std::string getName() const = 0;
	virtual const char *what() const noexcept;
protected:
	std::string text;
	mutable std::string whatString; // is set by the what function
};

struct LIB_DECL LogicError : public Exception {
	LogicError(std::string &&text) : Exception(std::move(text)) {}

	std::string getName() const {
		return "LogicError";
	}
};

#endif // LIBTEST_HPP