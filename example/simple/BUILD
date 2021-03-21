load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")


cc_library(
    name = "simple_api",
    hdrs = glob([
        "api/*.hpp",
        "src/*.hpp",
    ]),
    includes = [
        ".",
    ],
    visibility = ["//visibility:public"]
)

cc_library(
    name = "simple_src_hdrs",
    hdrs = glob([
        "src/*.hpp",
    ]),
    includes = [
        ".",
    ],
    visibility = ["//visibility:public"]
)

cc_binary(
    name = "simple",
    srcs = glob([
        "src/*.cpp",
    ]),
    deps = [
        ":simple_api",
        ":simple_src_hdrs",
        "//:inversify",
    ],
    visibility = ["//visibility:public"]
)
