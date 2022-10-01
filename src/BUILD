cc_library(
    name = "ast",
    hdrs = ["ast.h"],
)

cc_library(
    name = "token",
    srcs = ["token.cc"],
    hdrs = ["token.h"],
)

cc_library(
    name = "vm",
    srcs = ["vm.cc"],
    hdrs = ["vm.h"],
    deps = [
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "compiler",
    srcs = ["compiler.cc"],
    hdrs = ["compiler.h"],
    deps = [
        ":ast",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "parser",
    srcs = ["parser.cc"],
    hdrs = ["parser.h"],
    deps = [
        ":ast",
        ":token",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "scanner",
    srcs = ["scanner.cc"],
    hdrs = ["scanner.h"],
    deps = [
        ":token",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_binary(
    name = "june",
    srcs = ["main.cc"],
    deps = [
        ":compiler",
        ":parser",
        ":scanner",
        ":vm",
        "@com_google_absl//absl/status:statusor",
    ],
)
