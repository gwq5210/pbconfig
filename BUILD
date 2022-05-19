package(
  default_visibility = ["//visibility:public"],
)

cc_library(
  name = "pbconfig",
  hdrs = glob(["src/*.h"]),
  srcs = glob(["src/*.cc"], ["src/*_test.cc"]),
  copts = [
    "-Wall -fPIC -pipe -g -O2 -std=c++11",
  ],
  includes = ["src"],
  visibility = ["//visibility:public"],
  deps = [
    "@spdlog//:spdlog",
    "@fmtlib//:fmtlib",
    "@rapidjson//:rapidjson",
  ],
)

cc_test(
  name = "pbconfig_test",
  srcs = glob([
    "src/*_test.cc",
    "proto/*.cfg.h",
    "proto/*.cfg.cc",
  ]),
  copts = [
    "-Wall -fPIC -pipe -g -O2 -std=c++11",
  ],
  visibility = [
    "//visibility:public",
  ],
  deps = [
    "@googletest//:gtest",
    "@googletest//:gtest_main",
    "@spdlog//:spdlog",
    "@fmtlib//:fmtlib",
    "@rapidjson//:rapidjson",
    ":pbconfig",
  ],
)
