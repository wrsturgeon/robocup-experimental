#!/bin/sh

set -eu

cd ..
DIR=$(pwd)
cd build

for CPP in $(find ${DIR}/src -type f)
do
  NOX=$(echo ${CPP} | rev | cut -d/ -f1 | cut -d. -f2- | rev) # No extension or path
  if [ -f ${DIR}/test/${NOX}.cpp ]
  then
    echo "Testing ${CPP}..."
    rm -f ./default.profraw
    # lldb ./test-${NOX} -b -o run --one-line-on-crash 'thread backtrace' # This is great but WON'T fail if it leaks or tests fail
    ${DIR}/test/scripts/run-and-analyze.sh ./test-${NOX}
    llvm-profdata merge ./default.profraw -o ./${NOX}.profdata || (echo 'No coverage' && exit 1)
    (llvm-cov report ./test-${NOX} --instr-profile=./${NOX}.profdata ${CPP} | sed '3q;d' | xargs ${DIR}/test/scripts/parse-coverage.sh) || \
    (llvm-cov show ./test-${NOX} --instr-profile=./${NOX}.profdata ${CPP} && exit 1)
  fi
done
