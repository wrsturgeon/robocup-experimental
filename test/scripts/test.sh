#!/bin/sh

set -eu

cd ..
DIR=$(pwd)
cd build

echo 'Running tests...'
for HPP in $(find ${DIR}/src -type f -mindepth 2 ! -name README.md ! -path '*/legacy/*' ! -path '*/util/*')
do
  NOEXTN=$(echo ${HPP} | rev | cut -d/ -f1 | cut -d. -f2- | rev) # No extension or path
  echo "  ${NOEXTN}"
  rm -f ./default.profraw
  clang++ -o ./test-${NOEXTN} ${DIR}/test/${NOEXTN}.cpp ${@}
  ${DIR}/test/scripts/run-and-analyze.sh ./test-${NOEXTN}
  llvm-cov report ./test-${NOEXTN} --instr-profile=./${NOEXTN}.profdata ${HPP} --verify-region-info | grep -m1 ${HPP} | xargs ${DIR}/test/scripts/parse-coverage.sh ${NOEXTN}
done
