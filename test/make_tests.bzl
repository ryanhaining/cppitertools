def itertools_tests(progs):
  for p in progs:
    native.cc_test(
        name = "test_{}".format(p),
        srcs = ["test_{}.cpp".format(p), "test_main.cpp", "catch.hpp", "helpers.hpp"],
        deps = ["//:cppitertools",],
        copts = ["-I.",  "-std=c++17", "-Wall", "-Wextra", "-pedantic", "-g"],
    )
