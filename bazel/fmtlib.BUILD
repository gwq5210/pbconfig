load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
  name = "fmtlib",
  hdrs = glob([
    "include/**/*.h",
  ]),
  defines = ["FMT_HEADER_ONLY"],
  includes = ["include"],
  visibility = ["//visibility:public"],
)
