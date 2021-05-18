#ifndef LIBTEST_HPP
#define LIBTEST_HPP

#include <boost/config.hpp>

#include <iosfwd>
#include <memory>
#include <string>


#if defined(LIB_SOURCE)
#	define LIB_DECL BOOST_SYMBOL_EXPORT
#else
#	define LIB_DECL BOOST_SYMBOL_IMPORT
#endif

class LIB_DECL C {
	C(int data);
public:
	int data;
public:
	static std::shared_ptr<C> make(int data);
};

template<typename Sig>
struct LIB_DECL Function {};

template<typename R, typename ...Args>
struct LIB_DECL Function<R(Args...)> {
	virtual ~Function() {};
	virtual std::shared_ptr<Function> clone() const = 0;
	virtual void print(std::ostream &s) const = 0;
	virtual int operator()(std::shared_ptr<C> arg) const = 0;
};

LIB_DECL void useFunction(std::shared_ptr<Function<int(std::shared_ptr<C>)>> f);

#endif // LIBTEST_HPP
