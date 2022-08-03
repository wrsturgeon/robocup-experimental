#!/bin/sh

set -eux

cd ..
DIR=$(pwd)
cd build

for CPP in $(find ${DIR}/src -type f -iname '*.cpp')
do
  NOX=$(echo ${CPP} | rev | cut -d/ -f1 | cut -d. -f2- | rev) # No extension or path
  if [ -f ${DIR}/test/${NOX}.cpp ]
  then
    echo "Testing ${CPP}..."
    rm -f ./default.profraw
    ./test_${NOX}
    llvm-profdata merge ./default.profraw -o ./${NOX}.profdata || (echo 'No coverage' && exit 1)
    (llvm-cov report ./test_${NOX} --instr-profile=./${NOX}.profdata ${CPP} | sed '3q;d' | xargs ${DIR}/scripts/parse-coverage.sh) || \
    (llvm-cov show ./test_${NOX} --instr-profile=./${NOX}.profdata ${CPP} && exit 1)
  fi
done
