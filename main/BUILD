load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

cc_binary(
    name = "TestStage",
    srcs = ["testStage.cpp"],
)

cc_library(
    name = "SHITTestHelper",
    srcs =
        [
            "LoggingHW.cpp",
            "LoggingHW_config.cpp",
        ],
    hdrs = glob(
        ["*.h"],
    ),
    includes = ["src"],
    visibility = ["//visibility:public"],
    deps = [
        "@SHIT",
    ],
)

cc_test(
    name = "PrintUnitTests",
    srcs = ["SHIPrintUnitTests.cpp"],
    deps = [
        ":SHITTestHelper",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "FactoryUnitTests",
    srcs = ["SHIFactoryUnitTests.cpp"],
    deps = [
        ":SHITTestHelper",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "EventBusUnitTests",
    srcs = ["SHIEventBusUnitTests.cpp"],
    deps = [
        ":SHITTestHelper",
        "@googletest//:gtest_main",
    ],
)
