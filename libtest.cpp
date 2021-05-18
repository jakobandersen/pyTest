#include "libtest.hpp"

#include <iostream>

C::C(int data) : data(data) {}

std::shared_ptr<C> C::make(int data) {
	return std::shared_ptr<C>(new C(data));
}

void useFunction(std::shared_ptr<Function<int(std::shared_ptr<C>)>> f) {
	std::cout << "useFunction: begin" << std::endl;
	f->print(std::cout);
	std::cout << std::endl;
	auto c = C::make(60);
	(*f)(c);
	std::cout << "useFunction: end" << std::endl;
}