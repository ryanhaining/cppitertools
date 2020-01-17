from conans import ConanFile, CMake

import os


class Tentris_Parser(ConanFile):
    name = "cppitertools"
    version = "1.0"
    author = "Ryan Haining"
    description = "Implementation of python itertools and builtin iteration functions for C++17 "
    topics = ("please add topics here")
    settings = "build_type", "compiler", "os", "arch"
    generators = "cmake", "cmake_find_package", "cmake_paths"

    exports_sources = list()
    for file in os.listdir("."):
        if file.endswith(".hpp"):
            exports_sources.append(str(file))
    print("found files: " + str(exports_sources))

    exports_sources = tuple(exports_sources) + \
                      ("internal/*", "CMakeLists.txt", "cmake/dummy-config.cmake.in", "LICENSE.md")
    no_copy_source = True

    def package(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.install()

    def package_id(self):
        self.info.header_only()
