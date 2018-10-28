# Note that some examples currently use boost.optional which we do no not search for in this file.
# You might have to use the "keep going" option to continue building on errors if boost.optional is not in the include path.
# For example, building with MSVC (from a test/buildMsvc directory):
#   set CXX=cl.exe
#   cmake .. -G Ninja
#   cmake --build . -- -k99

cmake_minimum_required(VERSION 3.8)
project(cppitertools_tests CXX)
set (CMAKE_CXX_STANDARD 17)

find_package(Boost 1.60.0 REQUIRED)
include_directories(
	..
        ${Boost_INCLUDE_DIRS}
)


include(CheckIncludeFileCXX)
set(CMAKE_REQUIRED_INCLUDES ${PROJECT_SOURCE_DIR})
CHECK_INCLUDE_FILE_CXX(catch.hpp _has_catch)
if(NOT "${_has_catch}")
	message("WARNING: catch.hpp not found, run ./download_catch.sh from test/ directory first")
endif()

file(GLOB test_sources RELATIVE ${PROJECT_SOURCE_DIR} "test_*.cpp")
list(REMOVE_ITEM test_sources test_main.cpp)
add_library(test_main OBJECT test_main.cpp)

foreach(_source_cpp ${test_sources})
	get_filename_component(_name_without_extension "${_source_cpp}" NAME_WE)
	add_executable(${_name_without_extension} ${_source_cpp} $<TARGET_OBJECTS:test_main>)
endforeach()

add_executable(test_all ${test_sources} $<TARGET_OBJECTS:test_main>)
