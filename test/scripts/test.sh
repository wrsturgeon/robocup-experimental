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
    ${DIR}/test/scripts/run-and-analyze.sh ./test-${NOX}
    llvm-profdata merge ./default.profraw -o ./${NOX}.profdata || (echo 'No coverage' && exit 1)
    (llvm-cov report ./test-${NOX} --instr-profile=./${NOX}.profdata ${CPP} | sed '3q;d' | xargs ${DIR}/test/scripts/parse-coverage.sh) || \
    (llvm-cov show ./test-${NOX} --instr-profile=./${NOX}.profdata ${CPP} --show-branches=count --show-expansions --show-instantiations --show-line-counts-or-regions && exit 1)
  fi
done
