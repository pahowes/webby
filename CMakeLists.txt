cmake_minimum_required(VERSION 2.8.12)
project(webby)

#
# Configures the compiler, regardless of the build type.
#
if("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
  add_definitions(-std=c++11
                  -stdlib=libc++
                  -Weverything
                  -Wno-c++98-compat
                  -Wno-documentation-unknown-command
                  -Wno-missing-prototypes
                  -Wno-missing-noreturn
                  -Wno-padded
                  -Wno-unreachable-code
                  -Wno-weak-vtables)
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
  add_definitions(-std=c++11
                  -Wall
                  -Wextra
                  -Wpedantic)
endif()

#
# Dependency on Boost 1.55 due to ASF
#
find_package(Boost 1.55 REQUIRED)
include_directories(Boost_INCLUDE_DIRS)

#
# Use `make doc` to generate API documentation.
#
find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM)
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/html
          DESTINATION doc)
endif(DOXYGEN_FOUND)

#
# Adds the include directory to the compiler's search path.
#
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/externals/asf/include
  ${CMAKE_CURRENT_SOURCE_DIR}/externals/qlog/include
  )

#
# Source files to compile for the library.
#
#set(
#  LIBRARY_SOURCE
#  ${CMAKE_CURRENT_SOURCE_DIR}/source/server.cpp
#  ${CMAKE_CURRENT_SOURCE_DIR}/source/server_config.cpp
#  ${CMAKE_CURRENT_SOURCE_DIR}/source/server_error.cpp
#)

#
# Header files installed with the library.
#
#set(
#  LIBRARY_HEADERS
#  ${CMAKE_CURRENT_SOURCE_DIR}/include/server.hpp
#  ${CMAKE_CURRENT_SOURCE_DIR}/include/server_config.hpp
#  ${CMAKE_CURRENT_SOURCE_DIR}/include/server_error.hpp
#  ${CMAKE_CURRENT_SOURCE_DIR}/include/webby.hpp
#)

#
# Builds "libwebby.so", "libwebby.a", "webby.dll" or "webby.lib" depending on the compiler.
#
#add_library(webby_s STATIC ${LIBRARY_SOURCE})
#add_library(webby   SHARED ${LIBRARY_SOURCE})

#
# Builds the test driver. This can be launched with either `make test` or `ctest`
#
enable_testing()
link_directories(${CMAKE_BINARY_DIR})
add_executable(webbyd ${CMAKE_CURRENT_SOURCE_DIR}/test/main.cpp)
