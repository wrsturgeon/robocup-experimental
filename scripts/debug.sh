#!/bin/bash

if [ ${#} -lt 1 ]
then
  echo 'Syntax: '${0}' <arguments to clang++>'
  exit 1
fi

if [ $(PWD | rev | cut -d/ -f1) != 'pucobor' ]
then
  echo 'Please run from the `robocup` folder as `./scripts/debug.sh <arguments to clang++>`'
  exit 1
fi

if [ ! -f ${1} ]
then
  echo 'Source file '${1}' not found.'
  exit 1
fi

clang++ -g -o debug_x -D_BITS=$(getconf LONG_BIT) -include ./src/options.hpp -iquote ./src -O1 -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Werror ${@}
lldb ./debug_x -b -o run --one-line-on-crash 'thread backtrace'
rm debug_x
