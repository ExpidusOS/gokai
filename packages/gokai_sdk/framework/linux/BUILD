load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "gokai-framework",
    srcs = glob([
        "lib/**/*.cc",
        "lib/**/*-priv.h"
    ]),
    hdrs = glob(["include/gokai/**/*.h"]),
    copts = ["-Icore/include/", "-Iframework/linux/include"],
    deps = [
        "//core:gokai-core",
        "@wayland//:wayland-server",
    ],
    visibility = ["//visibility:public"],
    target_compatible_with = [
        "@platforms//os:linux"
    ]
)