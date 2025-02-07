load(":make_tests.bzl", "itertools_tests")

progs = [
    "accumulate",
    "batched",
    "chain",
    "chunked",
    "combinations",
    "combinations_with_replacement",
    "compress",
    "count",
    "cycle",
    "dropwhile",
    "enumerate",
    "filter",
    "filterfalse",
    "groupby",
    "imap",
    "permutations",
    "powerset",
    "product",
    "range",
    "repeat",
    "reversed",
    "slice",
    "sliding_window",
    "starmap",
    "sorted",
    "takewhile",
    "unique_everseen",
    "unique_justseen",
    "zip",
    "iteratoriterator",
    "iterator_wrapper",
    "iterbase",
    "mixed",
    "helpers",
]

SANITIZE = "-fsanitize=address,undefined"

cc_library(
  name = "test_main",
  srcs = ["test_main.cpp", "catch.hpp"],
  copts = [SANITIZE, "-Wall", "-Wextra", "-std=c++17", "-g"],
  linkopts = [SANITIZE],
)

itertools_tests(progs)
