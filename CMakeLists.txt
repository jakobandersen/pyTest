cmake_minimum_required(VERSION 3.10)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

###########################################################################
# Package Properties
###########################################################################

project(mod VERSION 0.1)
set(CPACK_PACKAGE_CONTACT "Jakob Lykke Andersen <jlandersen@imada.sdu.dk>")

set(CMAKE_CXX_FLAGS_OPTDEBUG "-g -O3")
set(CMAKE_C_FLAGS_OPTDEBUG "-g -O3")
set(CMAKE_EXE_LINKER_FLAGS_OPTDEBUG "")
set(CMAKE_SHARED_LINKER_FLAGS_OPTDEBUG "")
mark_as_advanced(CMAKE_CXX_FLAGS_OPTDEBUG CMAKE_C_FLAGS_OPTDEBUG CMAKE_EXE_LINKER_FLAGS_OPTDEBUG CMAKE_SHARED_LINKER_FLAGS_OPTDEBUG)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
            "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel OptDebug."
            FORCE)
endif()


###########################################################################
# Options
###########################################################################
option(ENABLE_SYMBOL_HIDING "Hide internal symbols in the library." OFF)
option(ENABLE_DEP_SYMBOL_HIDING "Hide symbols provided by library dependencies." ON)
option(ENABLE_IPO "Using link-time optimization." ON)
include(CheckIPOSupported)

get_directory_property(hasParent PARENT_DIRECTORY)

enable_testing()  # should be included here to add the targets in the top-level folder

###########################################################################
# Individual checks
###########################################################################
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

# Boost
# -------------------------------------------------------------------------
set(v 1.73.0)
foreach(PY 3 34 35 36 37 38 39)
    set(lib "python${PY}")
    find_package(Boost ${v} QUIET COMPONENTS ${lib})
    if(Boost_FOUND)
        find_package(Boost ${v} COMPONENTS ${lib})
        set(PYTHON_TARGET ${lib})
        break()
    endif()
endforeach()
if(NOT Boost_FOUND)
    find_package(Boost ${v} REQUIRED COMPONENTS python3)
    message(FATAL_ERROR "Could not find Boost.Python for Python 3. Tried 'python' wih suffixes 3, 34, 35, 36, 37, 38, and 39.")
endif()
message(STATUS "(Boost include dir is ${Boost_INCLUDE_DIRS})")


# Python
# -------------------------------------------------------------------------
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)


# Threads
# -------------------------------------------------------------------------
find_package(Threads REQUIRED)


###########################################################################
# Targets and Artefacts
###########################################################################
include(GNUInstallDirs)

if(NOT hasParent)
    set(CMAKE_NO_BUILTIN_CHRPATH TRUE)                 #
    # https://gitlab.kitware.com/cmake/community/wikis/doc/cmake/RPATH-handling
    set(CMAKE_SKIP_BUILD_RPATH FALSE)                  # use RPATH in the build tree
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)          # but not the RPATH used in installation
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)        # in addition to those deduced automatically
    # use
    # set_target_properties(theTarget PROPERTIES INSTALL_RPATH "theRPATH")
    # on each target that needs it
endif()


# libtest
# -------------------------------------------------------------------------
add_library(libtest SHARED libtest.cpp)
target_include_directories(libtest
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/>)
if(ENABLE_DEP_SYMBOL_HIDING)
    target_link_libraries(libtest PRIVATE
            $<$<CXX_COMPILER_ID:GNU>:-Wl,--exclude-libs,ALL>
            $<$<CXX_COMPILER_ID:Clang>:-Wl,--exclude-libs,ALL>
            #$<$<CXX_COMPILER_ID:AppleClang>:-Wl,--exclude-libs,ALL> # TODO: is a an equivalent?
            )
endif()
target_link_libraries(libtest PRIVATE Boost::headers Threads::Threads)
target_link_libraries(libtest PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wl,--no-undefined>
        $<$<CXX_COMPILER_ID:Clang>:-Wl,--no-undefined>
        #$<$<CXX_COMPILER_ID:AppleClang>:-Wl,--no-undefined> # TODO: is there an equivalent?
        )
# https://stackoverflow.com/questions/52018092/how-to-set-rpath-and-runpath-with-gcc-ld
target_link_libraries(libtest PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wl,--disable-new-dtags>
        $<$<CXX_COMPILER_ID:Clang>:-Wl,--disable-new-dtags>
        )
# on each target that needs it
target_compile_options(libtest PRIVATE -Wall -Wextra
        -Wno-unused-parameter
        -Wno-comment
        -Wno-sign-compare
        -Wno-unused-local-typedefs
        # gives false positive for boost::optional
        $<$<CXX_COMPILER_ID:GNU>:-Wno-error=maybe-uninitialized>
        )
target_compile_options(libtest PUBLIC
        $<$<CXX_COMPILER_ID:Clang>:-Wno-mismatched-tags>
        $<$<CXX_COMPILER_ID:AppleClang>:-Wno-mismatched-tags>)
set_target_properties(libtest PROPERTIES OUTPUT_NAME "test")
if(ENABLE_SYMBOL_HIDING)
    set_target_properties(libtest PROPERTIES
            CXX_VISIBILITY_PRESET hidden
            $<$<CXX_COMPILER_ID:GNU>VISIBILITY_INLINES_HIDDEN ON>
            )
endif()
target_compile_definitions(libtest PRIVATE LIB_SOURCE)
if(ENABLE_IPO)
    check_ipo_supported(RESULT result OUTPUT output)
    if(result)
        set_property(TARGET libtest PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(FATAL_ERROR "Interprocedural optimization was requested ('ENABLE_IPO=on'), but it is not supported: ${output}")
    endif()
endif()

install(TARGETS libtest
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})


# libpytest
# -------------------------------------------------------------------------
add_library(pytest MODULE libpytest.cpp)
target_include_directories(pytest
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(pytest PUBLIC libtest Boost::${PYTHON_TARGET} Python3::Python)
target_link_libraries(pytest PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wl,--no-undefined>
        $<$<CXX_COMPILER_ID:Clang>:-Wl,--no-undefined>
        #$<$<CXX_COMPILER_ID:AppleClang>:-Wl,--no-undefined> # TODO: is there an equivalent?
        )
set_target_properties(pytest PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}")
target_compile_options(pytest PRIVATE -Wall -Wextra
        -Wno-unused-parameter
        -Wno-comment
        -Wno-unused-local-typedefs)
set_target_properties(pytest PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON)
target_compile_definitions(pytest PRIVATE PY_SOURCE)
if(ENABLE_IPO)
    check_ipo_supported(RESULT result OUTPUT output)
    if(result)
        set_property(TARGET pytest PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(FATAL_ERROR "Interprocedural optimization was requested ('ENABLE_IPO=on'), but it is not supported: ${output}")
    endif()
endif()

install(TARGETS pytest
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/pytest
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/pytest
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# module files
install(DIRECTORY lib/pytest
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        FILES_MATCHING PATTERN "*.py" PATTERN "*.pyi")