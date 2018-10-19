# Note that some examples currently use boost.optional which we do no not search for in this file.
# You might have to use the "keep going" option to continue building on errors if boost.optional is not in the include path.
# For example, building with MSVC (from an examples/buildMsvc directory):
#   set CXX=cl.exe
#   cmake .. -G Ninja
#   cmake --build . -- -k99

cmake_minimum_required(VERSION 3.8)
project(cppitertools_examples CXX)
set (CMAKE_CXX_STANDARD 17)

include_directories(
	..
)

file(GLOB _examples_files "*_examples.cpp")

foreach(_file_cpp ${_examples_files})
	get_filename_component(_name_cpp "${_file_cpp}" NAME)
	get_filename_component(_name_without_extension "${_name_cpp}" NAME_WE)
	add_executable(${_name_without_extension} ${_file_cpp})
endforeach()
