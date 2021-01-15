load("@rules_cc//cc:defs.bzl", "cc_library")


cc_library(
    name = "example_api",
    hdrs = glob([
        "api/*.hpp",
    ]),
    includes = [
        "."
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
        "//:inversify",
    ],
    visibility = ["//visibility:public"]
)
