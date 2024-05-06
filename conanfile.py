from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps


class CppIterTools(ConanFile):
    name = 'cppitertools'
    version = '3.0'
    author = 'Ryan Haining <haining.cpp@gmail.com>'
    homepage = 'https://github.com/ryanhaining/cppitertools'
    url = homepage
    topics = ('itertools', 'cppitertools')
    license = "BSD 2-Clause 'Simplified' License"
    description = 'Range-based for loop add-ons inspired by the Python builtins and itertools library. ' \
                  'Like itertools and the Python3 builtins, this library uses lazy evaluation wherever possible.'
    settings = 'build_type', 'compiler', 'os', 'arch'
    exports = 'LICENSE.md'

    exports_sources = (
            'cppitertools/*',
            'cppitertools/internal/*',
            'CMakeLists.txt',
            'cmake/dummy-config.cmake.in')

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []
