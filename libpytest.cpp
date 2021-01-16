#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#include <boost/python.hpp>

#undef BOOST_BIND_GLOBAL_PLACEHOLDERS

#include <libtest.hpp>

#include <boost/python/module.hpp>

#include <iostream>

// https://stackoverflow.com/questions/8487986/file-macro-shows-full-path
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define DEBUG() std::cout << __FILENAME__ << ":" << __LINE__ << "(" << __func__ << "): "


namespace py = boost::python;

// https://stackoverflow.com/questions/27512585/print-boost-python-object
std::ostream &operator<<(std::ostream &os, const py::object &o) {
	return os << py::extract<std::string>(py::str(o))();
}

namespace {

PyObject *exportException(const std::string &name) {
	std::string scopeName = py::extract<std::string>(py::scope().attr("__name__"));
	std::string qualifiedName = scopeName + "." + name;
	DEBUG() << "name=" << name << std::endl;
	DEBUG() << "scopeName=" << scopeName << std::endl;
	DEBUG() << "qualifiedName=" << qualifiedName << std::endl;

	PyObject * exType = PyErr_NewException(qualifiedName.c_str(), PyExc_Exception, 0);
	if(!exType) {
		DEBUG() << "!exType" << std::endl;
		py::throw_error_already_set();
	}
	py::scope().attr(name.c_str()) = py::handle<>(py::borrowed(exType));
	return exType;
}

PyObject *exType = nullptr;

} // namespace

BOOST_PYTHON_MODULE (libpytest) {
	py::def("f", &f);

	//ExportException(LogicError);
	//py::class_<LogicError>("LogicError_", py::no_init)
	//		.def("getName", &LogicError::getName);
	exType = exportException("LogicError");
	DEBUG() << "chain before: " << py::detail::exception_handler::chain << std::endl;
	py::register_exception_translator<LogicError>([](const LogicError &ex) {
		DEBUG() << "ex=" << ex.getName() << "   " << ex.what() << std::endl;
//		py::object exPy(ex); /* wrap the C++ exception */
//		DEBUG() << "ex=" << exPy << std::endl;
		py::object exTypePy(py::handle<>(py::borrowed(exType)));
		DEBUG() << "exTypePy=" << exTypePy << std::endl;
		/* add the wrapped exception to the Python exception */
//		exTypePy.attr("cause") = exPy;
		PyErr_SetString(exType, ex.what());
	});
	DEBUG() << "chain after: " << py::detail::exception_handler::chain << std::endl;

//	py::register_exception_translator<std::exception>([](const std::exception &ex) {
//		DEBUG() << "wtf=" << ex.what() << std::endl;
//		DEBUG() << "typeid.name=" << typeid(ex).name() << std::endl;
//		throw ex;
//	});
}