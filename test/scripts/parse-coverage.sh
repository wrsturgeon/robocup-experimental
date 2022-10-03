#!/bin/bash

set -eux

SHOW="llvm-cov show ./test-${1} --instr-profile=./${1}.profdata ${2} -Xdemangler c++filt -Xdemangler -n --verify-region-info --show-instantiation-summary" # --show-line-counts-or-regions --show-branches=percent --show-expansions"

if [ ${7} -ne 0 ]
then
  echo "Missing ${7} function(s) in ${2} (${8} covered)"
  ${SHOW} --line-coverage-lt=100
  exit 1
fi

if [ ${10} -ne 0 ]
then
  echo "Missing ${10} line(s) in ${2} (${11} covered)"
  ${SHOW} --line-coverage-lt=100
  exit 1
fi

# if [ ${4} -ne 0 ]
# then
#   echo "Missing ${4} region(s) in ${2} (${5} covered)"
#   ${SHOW} --region-coverage-lt=100
#   exit 1
# fi

# if [ ${14} -ne 0 ]
# then
#   echo "Missing ${14} branch(es) in ${2} (${15} covered)"
#   ${SHOW} --region-coverage-lt=100
#   exit 1
# fi
