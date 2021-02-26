load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")


cc_library(
    name = "example_api",
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
    name = "example_src_hdrs",
    hdrs = glob([
        "src/*.hpp",
    ]),
    includes = [
        ".",
    ],
    visibility = ["//visibility:public"]
)

cc_binary(
    name = "example",
    srcs = glob([
        "src/*.cpp",
    ]),
    deps = [
        ":example_api",
        ":example_src_hdrs",
        "//:inversify",
    ],
    visibility = ["//visibility:public"]
)
