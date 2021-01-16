#include "libtest.hpp"

#include <iostream>

void f() {
	std::cout << "f()" << std::endl;
	throw LogicError("TheError(what)");
}

const char *Exception::what() const noexcept {
	return text.c_str();
}