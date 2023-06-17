package(default_visibility = ["//visibility:public"])
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "expat",
    srcs = [
        "lib/xmlparse.c",
        "lib/xmlrole.c",
        "lib/xmltok.c",
    ],
    hdrs = glob(["lib/*.h"]) + [
        "lib/xmltok_impl.c",
        "lib/xmltok_ns.c",
        "expat_config.h"
    ],
    copts = ["-I$(GENDIR)/external/expat", "-Iexternal/expat"],
)

filegroup(
    name = "expat.h",
    srcs = [
        "lib/expat.h"
    ]
)