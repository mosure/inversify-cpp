load("@rules_cc//cc:defs.bzl", "cc_library")


cc_library(
    name = "catch2",
    copts = [
        "-std=c++11",
    ],
    hdrs = [
        "third_party/catch2/catch.hpp",
    ],
    includes = [
        "third_party",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "inversify",
    copts = [
        "-std=c++17",
        "-pedantic-errors",
        "-Wall",
        "-Wextra",
        "-Wshadow",
        "-Wunused",
    ],
    hdrs = glob([
        "include/**/*.hpp",
    ]),
    includes = [
        "include",
    ],
    visibility = ["//visibility:public"]
)

cc_library(
    name = "test_mocks",
    hdrs = glob([
        "test/mock/*.hpp",
    ]),
    includes = [
        "test",
    ],
    visibility = ["//visibility:public"]
)

cc_binary(
    name = "test",
    copts = [
        "-DINVERSIFY_BINDING_INSPECTION",
    ],
    srcs = glob([
        "test/**/*.cpp",
    ]),
    deps = [
        ":catch2",
        ":inversify",
        ":test_mocks",
    ],
    visibility = ["//visibility:public"]
)
