package(default_visibility = ["//visibility:public"])
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
load("@bazel_skylib//rules:write_file.bzl", "write_file")

write_file(
  name = "gen_ffi_header",
  out = "gen_ffi_header.sh",
  content = [
      "#!/usr/bin/env bash",
      "LIBFFI_TEMPLATE_SUBST=(",
      "  \"-e s/@VERSION@/3.2.1/\" \\",
      "  \"-e s/@TARGET@/FAKE_TARGET_/\" \\",
      "  \"-e s/@HAVE_LONG_DOUBLE@/CONF_HAVE_LONG_DOUBLE/\" \\",
      "  \"-e s/@HAVE_LONG_DOUBLE_VARIANT@/CONF_HAVE_LONG_DOUBLE_VARIANT/\" \\",
      "  \"-e s/@FFI_EXEC_TRAMPOLINE_TABLE@/CONF_FFI_EXEC_TRAMPOLINE_TABLE/\"",
      ")",
      "sed \"${LIBFFI_TEMPLATE_SUBST[@]}\"",
  ],
)

write_file(
  name = "fficonfig",
  out = "fficonfig.h",
  content = ["#pragma once"] + select({
      "@platforms//cpu:x86_64": [
          "#undef CRAY_STACKSEG_END",
          "#undef C_ALLOCA",
          "#define EH_FRAME_FLAGS \"a\"",
          "#undef FFI_DEBUG",
          "#undef FFI_NO_RAW_API",
          "#undef FFI_NO_STRUCTS",
          "#define HAVE_ALLOCA 1",
          "#define HAVE_ALLOCA_H 1",
          "#undef HAVE_AS_CFI_PSEUDO_OP",
          "#undef HAVE_AS_REGISTER_PSEUDO_OP",
          "#undef HAVE_AS_SPARC_UA_PCREL",
          "#define HAVE_DLFCN_H 1",
          "#define HAVE_HIDDEN_VISIBILITY_ATTRIBUTE 1",
          "#define HAVE_INTTYPES_H 1",
          "#define HAVE_LONG_DOUBLE 1",
          "#define HAVE_MEMCPY 1",
          "#define HAVE_MEMORY_H 1",
          "#define HAVE_MMAP 1",
          "#define HAVE_MMAP_ANON 1",
          "#define HAVE_MMAP_DEV_ZERO 1",
          "#define HAVE_MMAP_FILE 1",
          "#define HAVE_RO_EH_FRAME 1",
          "#define HAVE_STDINT_H 1",
          "#define HAVE_STDLIB_H 1",
          "#undef HAVE_STRINGS_H",
          "#define HAVE_STRING_H 1",
          "#define HAVE_SYS_MMAN_H 1",
          "#define HAVE_SYS_STAT_H 1",
          "#define HAVE_SYS_TYPES_H 1",
          "#define HAVE_UNISTD_H 1",
          "#undef NO_MINUS_C_MINUS_O",
          "#define PACKAGE \"libffi\"",
          "#undef PACKAGE_BUGREPORT",
          "#undef PACKAGE_NAME",
          "#undef PACKAGE_STRING",
          "#undef PACKAGE_TARNAME",
          "#undef PACKAGE_VERSION",
          "#define SIZEOF_DOUBLE 8",
          "#define SIZEOF_LONG_DOUBLE 12",
          "#undef STACK_DIRECTION",
          "#define STDC_HEADERS 1",
          "#undef USING_PURIFY",
          "#undef VERSION",
          "#undef WORDS_BIGENDIAN",
          "#ifdef HAVE_HIDDEN_VISIBILITY_ATTRIBUTE",
          "#ifdef LIBFFI_ASM",
          "#ifdef __APPLE__",
          "#define FFI_HIDDEN(name) .private_extern name",
          "#else",
          "#define FFI_HIDDEN(name) .hidden name",
          "#endif",
          "#else",
          "#define FFI_HIDDEN __attribute__ ((visibility (\"hidden\")))",
          "#endif",
          "#else",
          "#ifdef LIBFFI_ASM",
          "#define FFI_HIDDEN(name)",
          "#else",
          "#define FFI_HIDDEN",
          "#endif",
          "#endif"
      ]
  }),
)

genrule(
  name = "ffi_header",
  cmd = "sh $(location gen_ffi_header.sh) < $(SRCS) > $(OUTS)",
  srcs = ["include/ffi.h.in"],
  outs = ["ffi.h"],
  tools = ["gen_ffi_header.sh"],
)

cc_library(
  name = "ffi",
  srcs = select({
      "@platforms//cpu:arm": [
          "src/arm/sysv.S",
          "src/arm/ffi.c",
      ],
      "@platforms//cpu:aarch64": [
          "src/aarch64/sysv.S",
          "src/aarch64/ffi.c",
      ],
      "@platforms//cpu:x86_32": [
          "src/x86/sysv.S",
          "src/x86/ffi.c",
      ],
      "@platforms//cpu:x86_64": [
          "src/x86/ffi64.c",
          "src/x86/ffiw64.c",
          "src/x86/unix64.S",
          "src/x86/win64.S",
      ],
      "@platforms//cpu:riscv64": [
          "src/riscv/sysv.S",
          "src/riscv/ffi.c",
      ]
  }),
  hdrs = [
    "ffi.h",
    "fficonfig.h",
    "include/ffi_cfi.h",
    "include/ffi_common.h",
    "include/tramp.h",
  ] + select({
    "@platforms//cpu:arm": [
      "src/arm/ffitarget.h",
    ],
    "@platforms//cpu:aarch64": [
      "src/aarch64/ffitarget.h",
    ],
    "@platforms//cpu:x86_32": [
      "src/x86/ffitarget.h",
      "src/x86/internal.h",
    ],
    "@platforms//cpu:x86_64": [
      "src/x86/asmnames.h",
      "src/x86/ffitarget.h",
      "src/x86/internal64.h",
    ],
    "@platforms//cpu:riscv64": [
      "src/riscv/ffitarget.h",
    ],
  }),
  copts = select({
    "@platforms//cpu:arm": [
      "-Iexternal/libffi/src/arm",
    ],
    "@platforms//cpu:aarch64": [
      "-Iexternal/libffi/src/aarch64",
    ],
    "@platforms//cpu:x86_32": [
      "-DHAVE_AS_X86_PCREL",
      "-DHAVE_AS_ASCII_PSEUDO_OP",
      "-Iexternal/libffi/src/x86",
    ],
    "@platforms//cpu:x86_64": [
      "-DHAVE_AS_X86_PCREL",
      "-DHAVE_AS_ASCII_PSEUDO_OP",
      "-Iexternal/libffi/src/x86",
      "-DCONF_HAVE_LONG_DOUBLE=1",
      "-DX86_64=1",
    ],
    "@platforms//cpu:riscv64": [
      "-Iexternal/libffi/src/riscv",
    ],
  }) + [
    "-Iexternal/libffi/include",
    "-I$(GENDIR)/external/libffi"
  ],
)