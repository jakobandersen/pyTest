#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#include <pybind11/pybind11.h>

#include <libtest.hpp>

#include <iostream>

// https://stackoverflow.com/questions/8487986/file-macro-shows-full-path
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define DEBUG() std::cout << __FILENAME__ << ":" << __LINE__ << "(" << __func__ << "): "


PYBIND11_MODULE(libpytest, m) {

}