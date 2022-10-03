#!/bin/bash

set -eu

if [ ${#} -lt 1 ]
then
  echo 'Usage: run-and-analyze.sh <executable>'
  exit 1
fi

ASAN_OPTIONS=detect_leaks=1:strict_string_checks=1:detect_stack_use_after_return=1:check_initialization_order=1:strict_init_order=1 \
ASAN_SYMBOLIZER=$(which llvm-symbolizer) \
LSAN_OPTIONS=suppressions=../lsan.supp:use_unaligned=1 \
${1} > /dev/null 2>&1 || ( \
  lldb ${1} -b -o run --one-line-on-crash bt --one-line-on-crash q; \
  exit 1
)
