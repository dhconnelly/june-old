cc_library(
    name = "instr",
    srcs = ["instr.cc"],
    hdrs = ["instr.h"],
    deps = [
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "value",
    srcs = ["value.cc"],
    hdrs = ["value.h"],
    deps = [
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "ast",
    srcs = ["ast.cc"],
    hdrs = ["ast.h"],
    deps = [
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "token",
    srcs = ["token.cc"],
    hdrs = ["token.h"],
    deps = [
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "vm",
    srcs = ["vm.cc"],
    hdrs = ["vm.h"],
    deps = [
        ":instr",
        ":value",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "compiler",
    srcs = ["compiler.cc"],
    hdrs = ["compiler.h"],
    deps = [
        ":ast",
        ":instr",
        ":value",
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

cc_library(
    name = "evaluator",
    srcs = ["evaluator.cc"],
    hdrs = ["evaluator.h"],
    deps = [
        ":compiler",
        ":parser",
        ":scanner",
        ":vm",
    ],
)

cc_binary(
    name = "june",
    srcs = ["main.cc"],
    deps = [
        ":evaluator",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/status:statusor",
    ],
)
