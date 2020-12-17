load("@rules_cc//cc:defs.bzl", "cc_library")


cc_library(
    name = "InversifyCpp",
    copts = [
        "-pedantic-errors",
        "-Wall",
        "-Wextra",
        "-Wshadow",
    ],
    hdrs = glob([
        "include/**/*.hpp",
    ]),
    includes = [
        "include",
    ],
    srcs = [

    ],
    visibility = ["//visibility:public"]
)

cc_binary(
    name = "test",
    srcs = [
        "test/main.cpp",
    ],
    deps = [
        ":InversifyCpp",
    ],
    visibility = ["//visibility:public"]
)
