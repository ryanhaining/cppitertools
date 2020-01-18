from conans import ConanFile, CMake

import os


class CppIterTools(ConanFile):
    name = "cppitertools"
    version = "2.0"
    author = "Ryan Haining <haining.cpp@gmail.com>"
    homepage = "https://github.com/ryanhaining/cppitertools"
    url = homepage
    topics = ("conan", "itertools", "cppitertools")
    license = 'BSD 2-Clause "Simplified" License'
    description = "Range-based for loop add-ons inspired by the Python builtins and itertools library. " \
                  "Like itertools and the Python3 builtins, this library uses lazy evaluation wherever possible."
    settings = "build_type", "compiler", "os", "arch"
    generators = "cmake", "cmake_find_package", "cmake_paths"
    exports = "LICENSE.md"

    exports_sources = list()
    for file in os.listdir("."):
        if file.endswith(".hpp"):
            exports_sources.append(str(file))
    print("found files: " + str(exports_sources))
    exports_sources = tuple(exports_sources) + \
                      ("internal/*", "CMakeLists.txt", "cmake/dummy-config.cmake.in")
    no_copy_source = True

    def package(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.install()

    def package_id(self):
        self.info.header_only()
