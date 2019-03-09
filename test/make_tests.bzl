def itertools_tests(progs):
  for p in progs:
    native.cc_test(
        name = "test_{}".format(p),
        srcs = ["test_{}.cpp".format(p), "catch.hpp", "helpers.hpp"],
        deps = [
            "//:cppitertools",
            ":test_main",
            ],
        copts = ["-I.", "-std=c++17", "-Wall", "-Wextra", "-pedantic", "-g"],
    )
