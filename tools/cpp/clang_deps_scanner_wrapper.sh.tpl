#!/bin/bash
#
# Ship the environment to the C++ action
#
set -eu

# Set-up the environment
%{env}

# Call the C++ compiler
%{deps_scanner} -format=p1689 -- %{cc} "$@" > $DEPS_SCANNER_OUTPUT_FILE

# workaround for clang-scan-deps BUG
# the .d file generated by clang-scan-deps is inconsistent with the one generated by clang.
# the source file path in .d file is absolute path, and Bazel regards the source file as extra dependency.
# see: https://github.com/llvm/llvm-project/issues/69439
# use two functions handle Linux and MacOS
# the difference is as follows
# 1. how to get string length
# 2. how to use sed to modify .d file
fix_bash() {
output_file_path=$(readlink -f $DEPS_SCANNER_OUTPUT_FILE)
output_file_length=$(expr length $output_file_path)
output_file_short_length=$(expr length $DEPS_SCANNER_OUTPUT_FILE)
current_path_length=$(expr $output_file_length - $output_file_short_length)
current_path=${output_file_path:0:$current_path_length}
dotd_file_length=$(expr $output_file_short_length - 2)
dotd_file_path=${DEPS_SCANNER_OUTPUT_FILE:0:$dotd_file_length}
sed_arg="s|$current_path||g"
sed -i $sed_arg $dotd_file_path
}
fix_zsh() {
output_file_path=$(readlink -f $DEPS_SCANNER_OUTPUT_FILE)
output_file_length=${#output_file_path}
output_file_short_length=${#DEPS_SCANNER_OUTPUT_FILE}
current_path_length=$(expr $output_file_length - $output_file_short_length)
current_path=${output_file_path:0:$current_path_length}
dotd_file_length=$(expr $output_file_short_length - 2)
dotd_file_path=${DEPS_SCANNER_OUTPUT_FILE:0:$dotd_file_length}
sed_arg="s|$current_path||g"
sed -i '' $sed_arg $dotd_file_path
}

system_type=$(uname -s)
if [ "$system_type" = "Linux" ]; then
    fix_bash
elif [ "$system_type" = "Darwin" ]; then
    fix_zsh
else
    echo "The system is not supported now: $system_type"
    exit 1
fi
