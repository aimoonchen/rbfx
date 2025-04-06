cmake_minimum_required(VERSION 3.13.5)

set(PYTHON_VERSION "3.13.2" CACHE STRING "The version of Python to build.")

string(REPLACE "." ";" VERSION_LIST ${PYTHON_VERSION})
list(GET VERSION_LIST 0 PY_VERSION_MAJOR)
list(GET VERSION_LIST 1 PY_VERSION_MINOR)
list(GET VERSION_LIST 2 PY_VERSION_PATCH)
set(PY_VERSION "${PY_VERSION_MAJOR}.${PY_VERSION_MINOR}.${PY_VERSION_PATCH}")
# set(CMAKE_DEBUG_POSTFIX "_d" CACHE STRING "Debug library postfix")
set(CMAKE_SUPPRESS_REGENERATION ON)
set(CMAKE_VS_GLOBALS "LinkIncremental=false")
# if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
#     set(CMAKE_CONFIGURATION_TYPES Release CACHE STRING "Release configuration" FORCE)
#     set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
#     # Set the possible values of build type for cmake-gui
#     set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Release")
# endif()

if(APPLE)
  set(_msg "Checking which MACOSX_DEPLOYMENT_TARGET to use")
  message(STATUS "${_msg}")
  include(cmake/PythonAutoDetectOSX.cmake)
  set(MACOSX_DEPLOYMENT_TARGET ${CMAKE_OSX_DEPLOYMENT_TARGET})
  message(STATUS "${_msg} - ${MACOSX_DEPLOYMENT_TARGET}")
endif()

project(Python C ASM)

if(NOT DEFINED CMAKE_MACOSX_RPATH)
  set(CMAKE_MACOSX_RPATH "")
endif()
if(NOT DEFINED CMAKE_BUILD_WITH_INSTALL_NAME_DIR)
  set(CMAKE_BUILD_WITH_INSTALL_NAME_DIR ON)
endif()

set(FreeBSD )
if(CMAKE_SYSTEM MATCHES FreeBSD)
  set(FreeBSD 1)
endif()

set(Solaris )
if(CMAKE_HOST_SOLARIS)
  set(Solaris 1)
endif()

include(CheckSymbolExists)
check_symbol_exists(__COSMOPOLITAN__ "stdio.h" Cosmopolitan)
if(Cosmopolitan)
  message(STATUS "Cosmopolitan compiler detected")
endif()

# Include helper functions
include(cmake/Extensions.cmake)
include(CMakeDependentOption)
include(CMakeParseArguments)
# include(CTest)

# Options
# option(DOWNLOAD_SOURCES "Automatically download the Python sources" OFF)
set(DOWNLOAD_SOURCES OFF)
set(_build_libpython_shared_default  0)
set(_build_libpython_shared_desc "Build libpython as a shared library")

if(DEFINED BUILD_SHARED OR DEFINED BUILD_STATIC)
  message(AUTHOR_WARNING "Deprecated options `BUILD_SHARED` or `BUILD_STATIC` are set, "
                         "please change configure options to use `BUILD_LIBPYTHON_SHARED` instead.")
  if(BUILD_SHARED AND BUILD_STATIC)
    message(AUTHOR_WARNING "Both BUILD_SHARED and BUILD_STATIC options are set to ON. Forcing BUILD_SHARED to OFF")
  endif()
  if(BUILD_STATIC)
    set(_build_libpython_shared_default  0)
    set(_build_libpython_shared_desc "Intialized from `BUILD_STATIC` (deprecated)")
  elseif(BUILD_SHARED)
    set(_build_libpython_shared_default  1)
    set(_build_libpython_shared_desc "Intialized from `BUILD_SHARED` (deprecated)")
  endif()
  message(AUTHOR_WARNING "Unsetting cache variables BUILD_SHARED and BUILD_STATIC")
  unset(BUILD_SHARED CACHE)
  unset(BUILD_STATIC CACHE)
endif()

# option(BUILD_LIBPYTHON_SHARED ${_build_libpython_shared_desc} ${_build_libpython_shared_default})
# option(BUILD_EXTENSIONS_AS_BUILTIN "Default all modules as builtin to libpython" ON)
# option(USE_LIB64 "Search for dependencies and install to prefix/lib64 instead of prefix/lib" OFF)
set(BUILD_LIBPYTHON_SHARED OFF)
set(BUILD_EXTENSIONS_AS_BUILTIN ON)
set(USE_LIB64 OFF)
if(WIN32)
#   option(INSTALL_WINDOWS_TRADITIONAL "Install the executable and extensions in the traditional location" OFF)
  set(INSTALL_WINDOWS_TRADITIONAL OFF)
  set(_build_wininst_default OFF)
  if(CMAKE_CROSSCOMPILING)
    set(_build_wininst_default OFF)
  endif()
#   option(BUILD_WININST "Build 'Windows Installer' program for distutils if not already provided in the source tree." ${_build_wininst_default})
#   cmake_dependent_option(BUILD_WININST_ALWAYS "Always build 'Windows installer' program even if provided in the source tree." OFF "BUILD_WININST" OFF)
  set(BUILD_WININST OFF)
  set(BUILD_WININST_ALWAYS OFF)
else()
  set(INSTALL_WINDOWS_TRADITIONAL OFF)
  set(BUILD_WININST OFF)
  set(BUILD_WININST_ALWAYS OFF)
endif()
# option(INSTALL_DEVELOPMENT "Install files required to develop C extensions" OFF)
# option(INSTALL_MANUAL "Install man files" OFF)
# option(INSTALL_TEST "Install test files" OFF)
set(INSTALL_DEVELOPMENT OFF)
set(INSTALL_MANUAL OFF)
set(INSTALL_TEST OFF)
# Append an item to a string if it does not already contain it.  Any additional
# arguments are treated as extra "set" arguments
macro(append_if_absent VARNAME VALUE)
    if(NOT ${VARNAME} MATCHES "(^| )${VALUE}($| )")
        if(${VARNAME})
            set(${VARNAME} "${${VARNAME}} ${VALUE}" ${ARGN})
        else()
            set(${VARNAME} "${VALUE}" ${ARGN})
        endif()
    endif()
endmacro()

# option(USE_SYSTEM_LIBRARIES "Use system libraries" OFF)
set(USE_SYSTEM_LIBRARIES OFF)
# Set platform dependent defaults
set(_use_builtin_zlib_default OFF)
set(_use_system_zlib_default ON)

cmake_dependent_option(USE_SYSTEM_BZip2 "Use system BZip2" ON "USE_SYSTEM_LIBRARIES" OFF)
cmake_dependent_option(USE_SYSTEM_Curses "Use system Curses" ON "USE_SYSTEM_LIBRARIES" OFF)
cmake_dependent_option(USE_SYSTEM_EXPAT "Use system EXPAT" ON "USE_SYSTEM_LIBRARIES" OFF)
cmake_dependent_option(USE_SYSTEM_LibFFI "Use system LibFFI" ON "USE_SYSTEM_LIBRARIES" ON)
cmake_dependent_option(USE_SYSTEM_OpenSSL "Use system OpenSSL" ON "USE_SYSTEM_LIBRARIES" OFF)
cmake_dependent_option(USE_SYSTEM_TCL "Use system TCL" ON "USE_SYSTEM_LIBRARIES" OFF)
cmake_dependent_option(USE_SYSTEM_ZLIB "Use system ZLIB" ${_use_system_zlib_default} "USE_SYSTEM_LIBRARIES" OFF)
cmake_dependent_option(USE_SYSTEM_GDBM "Use system GDBM" ON "USE_SYSTEM_LIBRARIES" OFF)
cmake_dependent_option(USE_SYSTEM_READLINE "Use system READLINE" ON "USE_SYSTEM_LIBRARIES" OFF)
cmake_dependent_option(USE_SYSTEM_SQLite3 "Use system SQLITE3" ON "USE_SYSTEM_LIBRARIES" OFF)
cmake_dependent_option(USE_SYSTEM_LIBMPDEC "Use system LIBMPDEC" ON "USE_SYSTEM_LIBRARIES" OFF)

# cmake_dependent_option(USE_BUILTIN_ZLIB "Use builtin ZLIB" ${_use_builtin_zlib_default} "NOT USE_SYSTEM_ZLIB" OFF)
set(USE_BUILTIN_ZLIB OFF)
# option(USE_LIBEDIT "Use libedit instead of readline" OFF)
# option(WITH_TSC "profile with the Pentium timestamp counter" OFF)
# option(ENABLE_IPV6 "Enable IP v6" ON)
# option(WITH_DOC_STRINGS "Enable if you want documentation strings in extension modules" ON)
set(USE_LIBEDIT OFF)
set(WITH_TSC OFF)
set(ENABLE_IPV6 ON)
set(WITH_DOC_STRINGS ON)
set(Py_DEBUG OFF) # TODO - Build debugging interpreter
# option(WITH_PYMALLOC "Define if you want to compile in Python-specific mallocs" ON)
set(WITH_PYMALLOC ON)
if(UNIX)
    # option(WITH_STATIC_DEPENDENCIES "Restrict extensions and external dependencies to static libraries" OFF)
    set(WITH_STATIC_DEPENDENCIES OFF)
    if(WITH_STATIC_DEPENDENCIES)
            set(_reason " because of WITH_STATIC_DEPENDENCIES=ON")

            set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_STATIC_LIBRARY_SUFFIX})
            message(STATUS "Setting CMAKE_FIND_LIBRARY_SUFFIXES to \"${CMAKE_STATIC_LIBRARY_SUFFIX}\"${_reason}")

            set(BUILD_EXTENSIONS_AS_BUILTIN ON CACHE BOOL "Forced to ON${_reason}" FORCE)
            message(STATUS "Setting BUILD_EXTENSIONS_AS_BUILTIN to ON${_reason}")

            set(BUILD_LIBPYTHON_SHARED OFF CACHE BOOL "Forced to OFF${_reason}" FORCE)
            message(STATUS "Setting BUILD_LIBPYTHON_SHARED to OFF${_reason}")

            append_if_absent(CMAKE_EXE_LINKER_FLAGS "-static")
            message(STATUS "Appending -static to CMAKE_EXE_LINKER_FLAGS${_reason}")
    endif()
else()
    set(WITH_STATIC_DEPENDENCIES 0)
endif()
# option(WITH_FREE_THREADING "Enable experimental free-threaded build and disables the GIL" OFF)
set(WITH_FREE_THREADING OFF)
if(WITH_FREE_THREADING OR WIN32)
    set(WITH_MIMALLOC ON)
else()
    # option(WITH_MIMALLOC "Enable mimalloc allocator" OFF)
    set(WITH_MIMALLOC OFF)
endif()
if(WITH_FREE_THREADING)
    set(Py_GIL_DISABLED ON)
else()
    set(Py_GIL_DISABLED OFF)
endif()

# Detect source directory
set(_landmark "pyconfig.h.in") # CMake will look for this file.
if(NOT (SRC_DIR AND EXISTS ${SRC_DIR}/${_landmark}))
    foreach(dirname
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/cpython-${PY_VERSION}
        ${CMAKE_CURRENT_SOURCE_DIR}/Python-${PY_VERSION}
        ${CMAKE_CURRENT_BINARY_DIR}/../cpython-${PY_VERSION}
        ${CMAKE_CURRENT_BINARY_DIR}/../Python-${PY_VERSION})
        set(SRC_DIR ${dirname})
        if(EXISTS ${SRC_DIR}/${_landmark})
            break()
        endif()
    endforeach()
endif()

# Download sources
get_filename_component(_parent_dir ${CMAKE_CURRENT_BINARY_DIR} PATH)
string(REGEX REPLACE "[a-z]+[1-9]$" "" _py_version_patch_no_rc ${PY_VERSION_PATCH})
set(_py_version_no_rc "${PY_VERSION_MAJOR}.${PY_VERSION_MINOR}.${_py_version_patch_no_rc}")
set(_download_link "https://www.python.org/ftp/python/${_py_version_no_rc}/Python-${PY_VERSION}.tgz")
# Variable below represent the set of available python version.
# 3.13.x
set(_download_3.13.0_md5 "c29f37220520ec6075fc37d4c62e178b")
set(_download_3.13.1_md5 "6820ac52d77af870f795dabc64583234")
set(_download_3.13.2_md5 "6192ce4725d9c9fc0e8a1cd38410b417")

set(_extracted_dir "Python-${PY_VERSION}")

if(NOT EXISTS ${SRC_DIR}/${_landmark} AND DOWNLOAD_SOURCES)
    get_filename_component(_filename ${_download_link} NAME)
    set(_archive_filepath ${CMAKE_CURRENT_BINARY_DIR}/../${_filename})
    if(EXISTS "${_archive_filepath}")
        message(STATUS "${_filename} already downloaded")
    else()
        message(STATUS "Downloading ${_download_link}")
        if(NOT DEFINED _download_${PY_VERSION}_md5)
            message(FATAL_ERROR "Selected PY_VERSION [${PY_VERSION}] is not associated with any checksum. Consider updating this CMakeLists.txt setting _download_${PY_VERSION}_md5 variable")
        endif()
        file(
          DOWNLOAD ${_download_link} ${_archive_filepath}
          EXPECTED_MD5 ${_download_${PY_VERSION}_md5}
          SHOW_PROGRESS
          )
    endif()

    message(STATUS "Extracting ${_filename}")
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xfz ${_archive_filepath}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/..
        RESULT_VARIABLE rv)
    if(NOT rv EQUAL 0)
        message(FATAL_ERROR "error: extraction of '${_filename}' failed")
    endif()
    set(SRC_DIR ${CMAKE_CURRENT_BINARY_DIR}/../${_extracted_dir})
endif()

get_filename_component(SRC_DIR "${SRC_DIR}" ABSOLUTE)
if(NOT EXISTS ${SRC_DIR}/${_landmark})
    message(FATAL_ERROR "Failed to locate python source.
The searched locations were:
   <CMAKE_CURRENT_SOURCE_DIR>
   <CMAKE_CURRENT_SOURCE_DIR>/cpython-${PY_VERSION}
   <CMAKE_CURRENT_SOURCE_DIR>/Python-${PY_VERSION}
   <CMAKE_CURRENT_BINARY_DIR>/../cpython-${PY_VERSION}
   <CMAKE_CURRENT_BINARY_DIR>/../Python-${PY_VERSION}
   <SRC_DIR>
You could try to:
  1) download ${_download_link}
  2) extract the archive in folder: ${_parent_dir}
  3) Check that file \"${_parent_dir}/${_extracted_dir}/${_landmark}\" exists.
  4) re-configure.
If you already downloaded the source, you could try to re-configure this project passing -DSRC_DIR:PATH=/path/to/Python-{PY_VERSION} using cmake or adding an PATH entry named SRC_DIR from cmake-gui.")
endif()
message(STATUS "SRC_DIR: ${SRC_DIR}")

# Extract version string from python source (Copied from FindPythonLibs.cmake)
file(STRINGS "${SRC_DIR}/Include/patchlevel.h" python_version_str
    REGEX "^#define[ \t]+PY_VERSION[ \t]+\"[^\"]+\"")
string(REGEX REPLACE "^#define[ \t]+PY_VERSION[ \t]+\"([^\"]+)\".*" "\\1"
      PY_VERSION_LONG "${python_version_str}")

# Extract version info and set variables PY_VERSION, PY_VERSION_(MAJOR|MINOR|PATCH), PY_RELEASE_(LEVEL|SERIAL)
include(cmake/PythonExtractVersionInfo.cmake)
python_extract_version_info(
  VERSION_STRING "${PY_VERSION_LONG}"
  )
message(STATUS "PY_VERSION     : ${PY_VERSION}")
message(STATUS "PY_VERSION_LONG: ${PY_VERSION_LONG}")

# Check version
if(NOT DEFINED _download_${PY_VERSION_LONG}_md5)
    message(WARNING "warning: selected python version '${PY_VERSION_LONG}' is not tested. Tested versions match '2.7.[3-14]' or '3.5.[1-5]' or `3.6.[0-4]`")
endif()
if(NOT "${PYTHON_VERSION}" STREQUAL "${PY_VERSION}")
  message(STATUS "Overwriting PYTHON_VERSION with value read from 'patchlevel.h': ${PYTHON_VERSION} -> ${PY_VERSION}")
  set(PYTHON_VERSION "${PY_VERSION}" CACHE STRING "The version of Python to build." FORCE)
endif()

if(NOT DEFINED PORTABLE_PYTHON_BUILD)
  set(PORTABLE_PYTHON_BUILD OFF)
endif()

# Apply patches
# option(PYTHON_APPLY_PATCHES "Apply patches" ON)
set(PYTHON_APPLY_PATCHES ON)
if(PYTHON_APPLY_PATCHES)
  include(cmake/PythonApplyPatches.cmake)
endif()

# Enable CXX language to support building of _distutils_findvs extension
if(WIN32)
  enable_language(CXX)
endif()

find_package(Python3 COMPONENTS Interpreter)

# Options depending of the python version
if(DEFINED USE_SYSTEM_DB)
    message(AUTHOR_WARNING "USE_SYSTEM_DB option is *NOT* supported with Python 3. Current version is ${PY_VERSION}")
endif()

if(DEFINED Py_USING_UNICODE)
    message(AUTHOR_WARNING "Py_USING_UNICODE option is *NOT* supported with Python 3. Current version is ${PY_VERSION}")
endif()

# option(WITH_C_LOCALE_COERCION "Enable C locale coercion to a UTF-8 based locale" ON)
set(WITH_C_LOCALE_COERCION ON)
# See "Improving performance in Python 2.7" - http://lwn.net/Articles/646888/
# option(WITH_COMPUTED_GOTOS "Improve performance enabling the computed goto based dispatch" OFF)
set(WITH_COMPUTED_GOTOS OFF)
set(USE_COMPUTED_GOTOS ${WITH_COMPUTED_GOTOS})

set(WITH_HASH_ALGORITHM "default" CACHE STRING "Define hash algorithm for str, bytes and memoryview.")
set_property(CACHE WITH_HASH_ALGORITHM PROPERTY STRINGS "default" "siphash24" "fnv")

if(DEFINED WITH_THREAD)
    message(AUTHOR_WARNING "WITH_THREAD option is *NOT* supported with Python < 3.7. Current version is ${PY_VERSION}")
endif()

set(WITH_SSL_DEFAULT_SUITES "python" CACHE STRING "Override default cipher suites string: python, openssl or a custom string")
if("${WITH_SSL_DEFAULT_SUITES}" STREQUAL "python")
    set(PY_SSL_DEFAULT_CIPHERS 1)
elseif("${WITH_SSL_DEFAULT_SUITES}" STREQUAL "openssl")
    set(PY_SSL_DEFAULT_CIPHERS 2)
else()
    set(PY_SSL_DEFAULT_CIPHERS 0)
    set(PY_SSL_DEFAULT_CIPHER_STRING "${WITH_SSL_DEFAULT_SUITES}")
endif()
# message(STATUS "Setting PY_SSL_DEFAULT_CIPHER_STRING to '${WITH_SSL_DEFAULT_SUITES}'")

# option(WITH_DECIMAL_CONTEXTVAR "Build _decimal module using a coroutine-local rather than a thread-local context" ON)
# option(WITH_TRACE_REFS "Enable tracing references for debugging purpose" OFF)
set(WITH_DECIMAL_CONTEXTVAR ON)
set(WITH_TRACE_REFS OFF)

# This is the major version number of Python
if(WIN32)
    set(LIBPYTHON_VERSION ${PY_VERSION_MAJOR}${PY_VERSION_MINOR})
    if(WITH_FREE_THREADING)
        set(LIBPYTHON_VERSION ${LIBPYTHON_VERSION}t)
    endif()
    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(LIBPYTHON python${LIBPYTHON_VERSION}_d)
    else()
        set(LIBPYTHON python${LIBPYTHON_VERSION})
    endif()
else()
    set(LIBPYTHON_VERSION ${PY_VERSION_MAJOR}.${PY_VERSION_MINOR})
    set(LIBPYTHON python${LIBPYTHON_VERSION})
endif()

# Proceed to the configure checks
include(cmake/ConfigureChecks.cmake)

# Set PYTHONHOME
set(LIBDIR "Lib") # See Lib/distutils/sysconfig.py - function 'get_python_lib'
if(UNIX)
    set(LIBDIR "lib")
endif()
if(USE_LIB64)
    set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS ON)
    set(LIBDIR "lib64")
endif()
set(PYTHONHOME "${LIBDIR}")
if(UNIX)
    set(PYTHONHOME "${PYTHONHOME}/${LIBPYTHON}")
    if(WITH_FREE_THREADING)
        set(PYTHONHOME "${PYTHONHOME}t")
    endif()
endif()

# Install tree directory
set(BIN_INSTALL_DIR bin)                         # Contains the python executable
if(INSTALL_WINDOWS_TRADITIONAL)
    set(BIN_INSTALL_DIR .)                       # Contains the python executable
endif()
set(LD_VERSION ${LIBPYTHON_VERSION}${ABIFLAGS})
set(CONFIG_INSTALL_DIR share/${LIBPYTHON})
set(EXTENSION_INSTALL_DIR ${PYTHONHOME}/lib-dynload)

if (${LIBPYTHON_VERSION} GREATER 3.1)
  set(LIB_CONFIG_INSTALL_DIR ${PYTHONHOME}/config-${LD_VERSION})
else()
  set(LIB_CONFIG_INSTALL_DIR ${PYTHONHOME}/config)
endif()

if(INSTALL_WINDOWS_TRADITIONAL)
    set(EXTENSION_INSTALL_DIR DLLs)
endif()
# set(INCLUDE_INSTALL_DIR include/python${LD_VERSION})
# if(MSVC)
    set(INCLUDE_INSTALL_DIR include)
# endif()
# Build tree directory
set(BIN_BUILD_DIR ${PROJECT_BINARY_DIR}/bin)
set(CONFIG_BUILD_DIR ${PROJECT_BINARY_DIR}/${CONFIG_INSTALL_DIR})
set(EXTENSION_BUILD_DIR ${PROJECT_BINARY_DIR}/${PYTHONHOME}/lib-dynload)
set(INCLUDE_BUILD_DIR ${SRC_DIR}/Include)

set(ARCHIVEDIR "libs")      # Contains the static (*.a) and import libraries (*.lib)

# Directories specific to 'libpython'
set(LIBPYTHON_LIBDIR ${LIBDIR})
set(LIBPYTHON_ARCHIVEDIR ${LIBDIR})
set(LIBPYTHON_STATIC_ARCHIVEDIR ${LIBDIR})
if(WIN32)
    set(LIBPYTHON_LIBDIR ${BIN_INSTALL_DIR})
    set(LIBPYTHON_ARCHIVEDIR ${ARCHIVEDIR})
    set(LIBPYTHON_STATIC_ARCHIVEDIR static-${ARCHIVEDIR})
endif()

set(EXTRA_PYTHONPATH "" CACHE STRING
    "A colon (:) separated list of extra paths to add to the PYTHONPATH")

# Configure 'pyconfig.h'
if(UNIX)
    set(PYCONFIG_BUILD_DIR ${BIN_BUILD_DIR})
    configure_file(cmake/config-unix/pyconfig.h.in
                   ${PYCONFIG_BUILD_DIR}/pyconfig.h)
elseif(WIN32)
    file(READ ${SRC_DIR}/PC/pyconfig.h.in PYCONFIG_H_CONTENTS)
    if(WITH_FREE_THREADING)
        string(REPLACE "/* #define Py_GIL_DISABLED 1 */" "#define Py_GIL_DISABLED 1" PYCONFIG_H_CONTENTS "${PYCONFIG_H_CONTENTS}")
    endif()
    file(WRITE ${SRC_DIR}/PC/pyconfig.h "${PYCONFIG_H_CONTENTS}")

    set(PYCONFIG_BUILD_DIR ${SRC_DIR}/PC) # In a windows build tree, 'pyconfig.h' is NOT required to
                                          # live along side the python executable.
                                          # See function '_init_posix()' and '_init_non_posix()'
                                          # in 'Lib/sysconfig.py'
    if(MINGW)
      set(PYCONFIG_BUILD_DIR cmake/config-mingw)
    endif()
endif()

# Install 'pyconfig.h'
# if(INSTALL_DEVELOPMENT)
#     install(FILES ${PYCONFIG_BUILD_DIR}/pyconfig.h
#             DESTINATION ${INCLUDE_INSTALL_DIR}/
#             COMPONENT Development)
# endif()
file(COPY ${PYCONFIG_BUILD_DIR}/pyconfig.h DESTINATION ${INCLUDE_BUILD_DIR})

# Set include directories
include_directories(${INCLUDE_BUILD_DIR})
include_directories(${INCLUDE_BUILD_DIR}/internal)
include_directories(${PYCONFIG_BUILD_DIR})
include_directories(${SRC_DIR}/Python)

# Set cflags used by all components
if(CMAKE_C_COMPILER_ID MATCHES GNU)
    append_if_absent(CMAKE_C_FLAGS "-std=c11")
    append_if_absent(CMAKE_C_FLAGS "-Wall")
    append_if_absent(CMAKE_C_FLAGS "-Wstrict-prototypes")
    append_if_absent(CMAKE_C_FLAGS "-fno-strict-aliasing")
    append_if_absent(CMAKE_C_FLAGS "-fwrapv")
    append_if_absent(CMAKE_C_FLAGS "-g")
    append_if_absent(CMAKE_C_FLAGS "-D_DEFAULT_SOURCE")
elseif(CMAKE_C_COMPILER_ID MATCHES Clang)
    append_if_absent(CMAKE_C_FLAGS "-Wall")
    append_if_absent(CMAKE_C_FLAGS "-g")
elseif(CMAKE_C_COMPILER_ID MATCHES Intel)
    append_if_absent(CMAKE_C_FLAGS "-Wall")
    append_if_absent(CMAKE_C_FLAGS "-no-ansi-alias")
elseif(CMAKE_C_COMPILER_ID MATCHES PGI)
    append_if_absent(CMAKE_C_FLAGS "-alias=traditional")
endif()

if(MSVC)
    append_if_absent(CMAKE_C_FLAGS "/Zm200")
    string(REPLACE "/Ob2" "/Ob1" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE}) # InlineFunctionExpansion=1
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}" CACHE STRING "Flags used by the compiler during release builds" FORCE)
endif()

# Useful additional variables that extensions can use.
if(CMAKE_SYSTEM MATCHES Linux)
    set(LINUX ON)
else()
    set(LINUX OFF)
endif()

# Clear PythonTargets.cmake
file(WRITE ${CONFIG_BUILD_DIR}/PythonTargets.cmake "")

# Define python executable wrapper command to ensure
# python executable resolves the expected python library.
set(PYTHON_WRAPPER_COMMAND )
if(BUILD_LIBPYTHON_SHARED AND UNIX)
    set(_envvar LD_LIBRARY_PATH)
    if(APPLE)
        set(_envvar DYLD_LIBRARY_PATH)
    endif()
    set(PYTHON_WRAPPER_COMMAND env ${_envvar}=${PROJECT_BINARY_DIR}/${LIBPYTHON_LIBDIR})
endif()

# Add extension modules
set(extensions_enabled "" CACHE INTERNAL "" FORCE)
set(extensions_disabled "" CACHE INTERNAL "" FORCE)
add_subdirectory(cmake/extensions CMakeBuild/extensions)

# Add the other subdirectories
add_subdirectory(cmake/libpython CMakeBuild/libpython)
# add_subdirectory(cmake/python CMakeBuild/python)
# add_subdirectory(cmake/include CMakeBuild/include)
# add_subdirectory(cmake/lib CMakeBuild/lib)
# add_subdirectory(cmake/tools CMakeBuild/tools)
# if(BUILD_WININST)
#     add_subdirectory(cmake/PC/bdist_wininst CMakeBuild/bdist_wininst)
# endif()
#[[
# Add target to run "Argument Clinic" over all source files
add_custom_target(clinic
    COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:python> ${SRC_DIR}/Tools/clinic/clinic.py --make
    DEPENDS python
    WORKING_DIRECTORY ${SRC_DIR}
    COMMENT "Running 'Argument Clinic' over all source files"
    VERBATIM
)

# Add target to generate 'Include/graminit.h' and 'Python/graminit.c'
set(generate_graminit_command $<TARGET_FILE:python> -m Parser.pgen)
add_custom_target(generate_graminit
    COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} ${generate_graminit_command}
        ${SRC_DIR}/Grammar/Grammar
        $<$<VERSION_GREATER_EQUAL:${PY_VERSION},3.8>:${SRC_DIR}/Grammar/Tokens>
        ${PROJECT_BINARY_DIR}/CMakeFiles/graminit.h
        ${PROJECT_BINARY_DIR}/CMakeFiles/graminit.c
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${PROJECT_BINARY_DIR}/CMakeFiles/graminit.h
        ${SRC_DIR}/Include/graminit.h
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${PROJECT_BINARY_DIR}/CMakeFiles/graminit.c
        ${SRC_DIR}/Python/graminit.c
    DEPENDS $<IF:$<VERSION_GREATER_EQUAL:${PY_VERSION},3.8>,python,pgen>
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Generating '${SRC_DIR}/Include/graminit.h' and '${SRC_DIR}/Python/graminit.c'."
    VERBATIM
)

# Add target to generate `Tools/peg_generator/pegen/grammar_parser.py`
add_custom_target(generate_pegen_metaparser
    COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH=${SRC_DIR}/Tools/peg_generator
        ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:python> -m pegen
            -q python
            ${SRC_DIR}/Tools/peg_generator/pegen/metagrammar.gram
            -o ${PROJECT_BINARY_DIR}/CMakeFiles/grammar_parser.py
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${PROJECT_BINARY_DIR}/CMakeFiles/grammar_parser.py
        ${SRC_DIR}/Tools/peg_generator/pegen/grammar_parser.py
    DEPENDS python
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Generating '${SRC_DIR}/Tools/peg_generator/pegen/grammar_parser.py'"
    VERBATIM
)

# Add target to generate `Parser/pegen/parse.c`
add_custom_target(generate_pegen_parse
    COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH=${SRC_DIR}/Tools/peg_generator
        ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:python> -m pegen
            -q c
            ${SRC_DIR}/Grammar/python.gram
            ${SRC_DIR}/Grammar/Tokens
            -o ${PROJECT_BINARY_DIR}/CMakeFiles/parse.c
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${PROJECT_BINARY_DIR}/CMakeFiles/parse.c
        ${SRC_DIR}/Parser/pegen/parse.c
    DEPENDS python
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Generating '${SRC_DIR}/Parser/pegen/parse.c'"
    VERBATIM
)

# Add target to generate 'opcode.h' header file
add_custom_target(generate_opcode_h
    COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:python> ${SRC_DIR}/Tools/scripts/generate_opcode_h.py
        ${SRC_DIR}/Lib/opcode.py
        ${PROJECT_BINARY_DIR}/CMakeFiles/opcode.h
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${PROJECT_BINARY_DIR}/CMakeFiles/opcode.h
        ${SRC_DIR}/Include/opcode.h
    DEPENDS python
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Generating '${SRC_DIR}/Include/opcode.h'."
    VERBATIM
)

# Add target to generate 'Include/Python-ast.h' from 'Python.asdl'
add_custom_target(generate_python_ast_h
    COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:python> ${SRC_DIR}/Parser/asdl_c.py
        -h ${SRC_DIR}/Include
        ${SRC_DIR}/Parser/Python.asdl
    DEPENDS python
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Generating '${SRC_DIR}/Include/Python-ast.h'"
    VERBATIM
)

# Add target to generate 'Python/Python-ast.c' from 'Python.asdl'
add_custom_target(generate_python_ast_c
    COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:python> ${SRC_DIR}/Parser/asdl_c.py
        -c ${SRC_DIR}/Python
        ${SRC_DIR}/Parser/Python.asdl
    DEPENDS python
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Generating '${SRC_DIR}/Python/Python-ast.c'"
    VERBATIM
)

# If needed, copy 'asdl.py' and 'Python.asdl' into build directory so that 'test_asdl_parser' passes.
set(_asdl_subdir ${LIBDIR})
if(WIN32)
  set(_asdl_subdir )
endif()
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/${LIBDIR}/Parser)
configure_file(
    ${SRC_DIR}/Parser/Python.asdl
    ${PROJECT_BINARY_DIR}/${_asdl_subdir}/Parser/Python.asdl
    COPYONLY
)
configure_file(
    ${SRC_DIR}/Parser/asdl.py
    ${PROJECT_BINARY_DIR}/${_asdl_subdir}/Parser/asdl.py
    COPYONLY
)
]]
# show_extension_summary()

if(UNIX)

    # python.pc
    configure_file(cmake/python.pc.in
                  ${PROJECT_BINARY_DIR}/Misc/python-${LIBPYTHON_VERSION}.pc @ONLY)
    configure_file(cmake/python.pc.in
                  ${PROJECT_BINARY_DIR}/Misc/python-${PY_VERSION_MAJOR}.pc @ONLY)
    configure_file(cmake/python.pc.in
                  ${PROJECT_BINARY_DIR}/Misc/python.pc @ONLY)
    if(INSTALL_DEVELOPMENT)
        install(FILES
                    ${PROJECT_BINARY_DIR}/Misc/python-${LIBPYTHON_VERSION}.pc
                    ${PROJECT_BINARY_DIR}/Misc/python-${PY_VERSION_MAJOR}.pc
                    ${PROJECT_BINARY_DIR}/Misc/python.pc
                DESTINATION lib/pkgconfig
                COMPONENT Development)
    endif()

    # Makefile
    set(MAKEFILE_LDSHARED_FLAGS "-shared")
    if(APPLE)
        set(MAKEFILE_LDSHARED_FLAGS "-dynamiclib -headerpad_max_install_names -undefined dynamic_lookup")
    endif()
    configure_file(cmake/makefile-variables.in
                   ${BIN_BUILD_DIR}/Makefile @ONLY)
    if(INSTALL_DEVELOPMENT)
        install(FILES ${BIN_BUILD_DIR}/Makefile
                DESTINATION ${LIB_CONFIG_INSTALL_DIR}
                RENAME Makefile
                COMPONENT Development)
    endif()

    # Utility scripts
    if(INSTALL_DEVELOPMENT)
        install(FILES ${SRC_DIR}/install-sh ${SRC_DIR}/Modules/makesetup
                DESTINATION ${LIB_CONFIG_INSTALL_DIR}
                COMPONENT Development)
    endif()

    # Install manual
    if(INSTALL_MANUAL)
        set(_install_man FILES ${SRC_DIR}/Misc/python.man DESTINATION share/man/man1 COMPONENT Runtime)
        install(${_install_man} RENAME python${LIBPYTHON_VERSION}.1)
        install(${_install_man} RENAME python${PY_VERSION_MAJOR}.1)
    endif()

endif()

# Create an empty lib-dynload folder, even if we don't have any extensions
# to go in there.  bin/python uses this to auto-determine the exec_prefix
# and properly generate the _sysconfigdata.py
# file(MAKE_DIRECTORY "${EXTENSION_BUILD_DIR}")
# install(DIRECTORY DESTINATION ${EXTENSION_INSTALL_DIR})

if(BUILD_TESTING)
    set(TESTOPTS -l)
    set(TESTPROG ${PROJECT_BINARY_DIR}/${PYTHONHOME}/test/regrtest.py)
    set(TESTPYTHONOPTS )
    set(TESTPYTHON $<TARGET_FILE:python> ${TESTPYTHONOPTS})
    set(TESTPYTHON ${CMAKE_CROSSCOMPILING_EMULATOR} ${TESTPYTHON})
    include(cmake/UnitTests.cmake)
    foreach(unittest ${unittests})
      set(EXTRATESTOPTS -v)
      # XXX Running 'test_doctest' in verbose mode always return 1
      if(unittest STREQUAL "test_doctest")
        set(EXTRATESTOPTS )
      endif()
      add_test(NAME ${unittest} COMMAND ${PYTHON_WRAPPER_COMMAND} ${TESTPYTHON} ${TESTPROG} ${TESTOPTS} ${EXTRATESTOPTS} ${unittest})
    endforeach()

    function(add_cmakescript_test testname script)
      add_test(cmake_${testname}_test ${CMAKE_COMMAND}
        -DTEST_${testname}:BOOL=ON
        -P ${PROJECT_SOURCE_DIR}/${script})
      set_tests_properties(cmake_${testname}_test PROPERTIES
        LABELS CMake
        PASS_REGULAR_EXPRESSION "SUCCESS")
    endfunction()
    add_cmakescript_test(
        python_extract_version_info
        cmake/PythonExtractVersionInfo.cmake
        )
endif()

include(CMakePackageConfigHelpers)

# Configure 'PythonConfig.cmake' for a build tree
set(CONFIG_DIR_CONFIG ${CONFIG_BUILD_DIR})
set(INCLUDE_DIR_CONFIG ${INCLUDE_BUILD_DIR})
set(PYTHON_CONFIG_CODE "####### Expanded from \@PYTHON_CONFIG_CODE\@ #######\n")
set(PYTHON_CONFIG_CODE "${PYTHON_CONFIG_CODE}list(APPEND PYTHON_INCLUDE_DIR \"${PYCONFIG_BUILD_DIR}\")\n")
set(PYTHON_CONFIG_CODE "${PYTHON_CONFIG_CODE}##################################################")
set(python_config ${CONFIG_BUILD_DIR}/PythonConfig.cmake)
configure_package_config_file(
    cmake/PythonConfig.cmake.in
    ${python_config}
    INSTALL_DESTINATION ${PROJECT_BINARY_DIR}
    PATH_VARS CONFIG_DIR_CONFIG INCLUDE_DIR_CONFIG
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

# Configure 'PythonConfig.cmake' for an install tree
set(CONFIG_DIR_CONFIG ${CONFIG_INSTALL_DIR})
set(INCLUDE_DIR_CONFIG ${INCLUDE_INSTALL_DIR})
set(PYTHON_CONFIG_CODE "")
set(python_install_config ${PROJECT_BINARY_DIR}/CMakeFiles/PythonConfig.cmake)
configure_package_config_file(
    cmake/PythonConfig.cmake.in
    ${python_install_config}
    INSTALL_DESTINATION ${CMAKE_INSTALL_PREFIX}/${CONFIG_INSTALL_DIR}
    PATH_VARS CONFIG_DIR_CONFIG INCLUDE_DIR_CONFIG
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

# Configure 'PythonTargets.cmake' and 'PythonConfigVersion.cmake
get_property(PYTHON_TARGETS GLOBAL PROPERTY PYTHON_TARGETS)
export(TARGETS ${PYTHON_TARGETS} APPEND FILE ${CONFIG_BUILD_DIR}/PythonTargets.cmake)

set(python_config_version ${CONFIG_BUILD_DIR}/PythonConfigVersion.cmake)
write_basic_package_version_file(
    ${python_config_version}
    VERSION ${PY_VERSION}
    COMPATIBILITY SameMajorVersion
)

if(INSTALL_DEVELOPMENT)
    # Install 'PythonTargets.cmake', 'PythonConfig.cmake' and 'PythonConfigVersion.cmake
    install(EXPORT PythonTargets
        FILE PythonTargets.cmake
        DESTINATION ${CONFIG_INSTALL_DIR} COMPONENT Development)

    install(
        FILES ${python_install_config} ${python_config_version}
        DESTINATION ${CONFIG_INSTALL_DIR} COMPONENT Development
    )
endif()

# Install License
# set(license_destination ${PYTHONHOME})
# if(INSTALL_WINDOWS_TRADITIONAL)
#   set(license_destination .)
# endif()
# install(FILES ${SRC_DIR}/LICENSE DESTINATION ${license_destination} COMPONENT Runtime RENAME LICENSE.txt)
