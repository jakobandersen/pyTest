#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include <libtest.hpp>

#include <iostream>

// https://stackoverflow.com/questions/8487986/file-macro-shows-full-path
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define DEBUG() std::cout << __FILENAME__ << ":" << __LINE__ << "(" << __func__ << "): "

namespace py = pybind11;

namespace detail {

template<typename F>
struct FunctionWrapper {
};

template<typename R, typename ...Args>
struct FunctionWrapper<R(Args...)> : Function<R(Args...)> {
	std::shared_ptr<Function<R(Args...)>> clone() const override {
		PYBIND11_OVERRIDE_PURE(
				std::shared_ptr<Function<R(Args...)>>,
				Function<R(Args...)>,
				clone,
		);
	}

	void print(std::ostream &s) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function override = pybind11::get_override(static_cast<const Function<R(Args...)> *>(this), "__str__");
		if(override) {
			s << override();
			return;
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"Function<R(Args...)>::print\"");
	}

	R operator()(Args ...args) const override {
		PYBIND11_OVERRIDE_PURE_NAME(
				R,
				Function<R(Args...)>,
				"__call__",
				operator(),
				args...
		);
	}
};

} // namespace detail

template<typename Sig>
void exportFunc(py::module_ m, const char *name) {
	using Func = Function<Sig>;
	using Wrap = detail::FunctionWrapper<Sig>;
	py::class_<Func, Wrap>(m, name)
			.def(py::init())
			.def("clone", &Func::clone)
			.def("__str__", &Func::print)
			.def("__call__", &Func::operator());
}


PYBIND11_MODULE(libpytest, m) {
	std::cout << "PYBIND11_MODULE(libpytest, m)" << std::endl;

	py::class_<C>(m, "C")
	      .def_readwrite("data", &C::data);
	m.def("makeC", &C::make);

	exportFunc<int(std::shared_ptr<C>)>(m, "Function");

	m.def("useFunction", &useFunction);
}