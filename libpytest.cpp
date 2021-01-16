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
	DEBUG() << "name=" << name << std::endl;
	std::string scopeName = py::extract<std::string>(py::scope().attr("__name__"));
	DEBUG() << "scopeName=" << scopeName << std::endl;
	std::string qualifiedName = scopeName + "." + name;
	DEBUG() << "qualifiedName=" << qualifiedName << std::endl;

	PyObject * exType = PyErr_NewException(qualifiedName.c_str(), PyExc_Exception, 0);
	if(!exType) {
		DEBUG() << "!exType" << std::endl;
		py::throw_error_already_set();
	}
	py::scope().attr(name.c_str()) = py::handle<>(py::borrowed(exType));
	return exType;
}

} // namespace


BOOST_PYTHON_MODULE (libpytest) {
	py::def("f", &f);

	//ExportException(LogicError);
	py::class_<LogicError>("LogicError_", py::no_init);
	PyObject * exType = exportException("LogicError");
	py::register_exception_translator<LogicError>([exType](const LogicError &ex) {
		DEBUG() << "ex=" << ex.getName() << "   " << ex.what() << std::endl;
		py::object exPy(ex); /* wrap the C++ exception */
		DEBUG() << "ex=" << exPy << std::endl;
		py::object exTypePy(py::handle<>(py::borrowed(exType)));
		DEBUG() << "exTypePy=" << exTypePy << std::endl;
		/* add the wrapped exception to the Python exception */
		exTypePy.attr("cause") = exPy;
		PyErr_SetString(exType, ex.what());
	});
}