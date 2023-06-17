load("@rules_cc//cc:defs.bzl", "cc_test", "cc_library")

cc_library(
    name = "gokai-core",
    srcs = glob([
      "lib/**/*.cc",
      "lib/**/*-priv.h"
    ]),
    hdrs = glob([
      "include/gokai/**/*.h"
    ]),
    copts = ["-Icore/include/"],
    visibility = ["//visibility:public"]
)

cc_test(
  name = "test-gokai-core",
  size = "small",
  srcs = glob(["test/*.cc"]),
  copts = ["-Icore/include/"],
  deps = [
    "@com_google_googletest//:gtest_main",
    ":gokai-core"
  ],
)