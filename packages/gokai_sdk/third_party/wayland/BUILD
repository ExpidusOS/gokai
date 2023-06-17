package(default_visibility = ["//visibility:public"])
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

filegroup(
    name = "wayland-core-protocol",
    srcs = [
        "protocol/wayland.xml"
    ]
)

genrule(
    name = "wayland-version",
    outs = ["wayland-version.h"],
    cmd = "echo '#define WAYLAND_VERSION \"1.22.0\"' > $(OUTS)"
)

cc_library(
    name = "wayland-util",
    linkstatic = True,
    srcs = [
        "src/wayland-util.c"
    ],
    hdrs = [
        "src/wayland-private.h",
        "src/wayland-util.h",
        "wayland-version.h"
    ],
)

cc_binary(
    name = "wayland-scanner",
    srcs = ["src/scanner.c"],
    copts = ["-Iexternal/expat/lib"],
    deps = [
        "//:wayland-util",
        "@expat//:expat"
    ],
)

genrule(
    name = "wayland-core-protocol-sources-static",
    outs = ["wayland-protocol-private.c"],
    srcs = [":wayland-core-protocol"],
    cmd = "$(location wayland-scanner) -s private-code < $(SRCS) > $(OUTS)",
    tools = ["wayland-scanner"],
)

genrule(
    name = "wayland-core-protocol-sources-shared",
    outs = ["wayland-protocol-public.c"],
    srcs = [":wayland-core-protocol"],
    cmd = "$(location wayland-scanner) -s public-code < $(SRCS) > $(OUTS)",
    tools = ["wayland-scanner"],
)

genrule(
    name = "wayland-core-client-protocol-headers",
    outs = ["wayland-client-protocol.h"],
    srcs = [":wayland-core-protocol"],
    cmd = "$(location wayland-scanner) -s client-header < $(SRCS) > $(OUTS)",
    tools = ["wayland-scanner"],
)

genrule(
    name = "wayland-core-server-protocol-headers",
    outs = ["wayland-server-protocol.h"],
    srcs = [":wayland-core-protocol"],
    cmd = "$(location wayland-scanner) -s server-header < $(SRCS) > $(OUTS)",
    tools = ["wayland-scanner"],
)

cc_library(
    name = "wayland-client",
    srcs = [
        "src/connection.c",
        "src/wayland-client.c",
        "src/wayland-os.c",
    ],
    hdrs = glob(["src/*.h"]) + ["wayland-client-protocol.h"],
    copts = ["-I$(GENDIR)/external/libffi", "-Iexternal/wayland/src"] + select({
      "@platforms//cpu:arm": [
        "-Iexternal/libffi/src/arm",
      ],
      "@platforms//cpu:aarch64": [
        "-Iexternal/libffi/src/aarch64",
      ],
      "@platforms//cpu:x86_32": [
        "-Iexternal/libffi/src/x86",
      ],
      "@platforms//cpu:x86_64": [
        "-Iexternal/libffi/src/x86",
      ],
      "@platforms//cpu:riscv64": [
        "-Iexternal/libffi/src/riscv",
      ],
    }),
    deps = [
        "//:wayland-util",
        "@libffi//:ffi"
    ],
)

cc_library(
    name = "wayland-server",
    srcs = [
        "src/connection.c",
        "src/wayland-server.c",
        "src/wayland-shm.c",
        "src/event-loop.c",
    ],
    hdrs = glob(["src/*.h"]) + ["wayland-server-protocol.h"],
    copts = ["-I$(GENDIR)/external/libffi", "-Iexternal/wayland/src"] + select({
      "@platforms//cpu:arm": [
        "-Iexternal/libffi/src/arm",
      ],
      "@platforms//cpu:aarch64": [
        "-Iexternal/libffi/src/aarch64",
      ],
      "@platforms//cpu:x86_32": [
        "-Iexternal/libffi/src/x86",
      ],
      "@platforms//cpu:x86_64": [
        "-Iexternal/libffi/src/x86",
      ],
      "@platforms//cpu:riscv64": [
        "-Iexternal/libffi/src/riscv",
      ],
    }),
    deps = [
        "//:wayland-util",
        "@libffi//:ffi"
    ],
)