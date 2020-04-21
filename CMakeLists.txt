cmake_minimum_required(VERSION 3.12)
project(cppitertools VERSION 2.0)

if(NOT DEFINED ENV{cppitertools_INSTALL_CMAKE_DIR})
  message(WARNING [[
The default value of cppitertools_INSTALL_CMAKE_DIR changed recently, from
  "share/cppitertools/cmake"
to
  "share"
in order to behave better with existing CMake practice.

In order to get the previous behavior, pass
  -Dcppitertools_INSTALL_CMAKE_DIR=share/cppitertools/cmake
to the CMake invocation; in order to get the new behavior without the warning,
pass
  -Dcppitertools_INSTALL_CMAKE_DIR=share
explicitly.
]])
endif()

# installation directories
set(cppitertools_INSTALL_INCLUDE_DIR "include" CACHE STRING "The installation include directory")
set(cppitertools_INSTALL_CMAKE_DIR "share" CACHE STRING "The installation cmake directory")

# define a header-only library
add_library(cppitertools INTERFACE)
add_library(cppitertools::cppitertools ALIAS cppitertools)

target_include_directories(cppitertools INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${cppitertools_INSTALL_INCLUDE_DIR}/cppitertools>)

# require C++17
target_compile_features(cppitertools INTERFACE cxx_std_17)

# Make package findable
configure_file(cmake/dummy-config.cmake.in cppitertools-config.cmake @ONLY)

# Enable version checks in find_package
include(CMakePackageConfigHelpers)
write_basic_package_version_file(cppitertools-config-version.cmake COMPATIBILITY SameMajorVersion)

# install and export target
install(
  TARGETS cppitertools
  EXPORT cppitertools-targets)

install(
  DIRECTORY .
  DESTINATION ${cppitertools_INSTALL_INCLUDE_DIR}/cppitertools)

install(
  EXPORT cppitertools-targets
  FILE cppitertools-config.cmake
  NAMESPACE cppitertools::
  DESTINATION ${cppitertools_INSTALL_CMAKE_DIR}/cppitertools)
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/cppitertools-config-version.cmake
  DESTINATION ${cppitertools_INSTALL_CMAKE_DIR}/cppitertools)
