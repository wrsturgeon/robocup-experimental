#!/bin/bash

set -eu
shopt -s nullglob # Don't iterate with zero elements



# Make a folder to declutter all the executables & coverage files
mkdir -p ./coverage && cd coverage && rm -rf *



echo 'Checking leak detection...'
# Make sure we detect a genuine (test) memory leak
clang++ ../test/leak.cpp -o ./run-test ${SANITIZE} ${ALL_FLAGS}
if ./run-test >/dev/null 2>&1
then
  echo "Missed leak"
  exit 1
else
  echo "Successfully detected test leak"
fi



echo 'Checking compilation, coverage, and memory leaks...'
# Now make sure, knowing we can detect them, that there aren't any
# TODO: convert V4L2 to C++ and remove */legacy/* exemptions
for file in $(find ../src -type f ! -name README.md ! -path '*/legacy/*')
do
  FNAME=$(echo ${file} | rev | cut -d/ -f1 | cut -d. -f2- | rev)
  TEST=../test/${FNAME}.cpp
  echo "Running ${TEST}..."
  # TODO: link everything except this file WITHOUT testing, then only this one with testing (remove -Wno-error=unused-function)
  clang++ -o ./run-test ${TEST} ${ALL_FLAGS} ${SANITIZE} -Wno-error=unused-function
  ./run-test
  clang++ -o ./${FNAME} ${TEST} ${ALL_FLAGS} ${COVERAGE} -Wno-error=unused-function
  ./${FNAME} # Generate coverage at the same time
  llvm-profdata merge ./default.profraw -o ./${FNAME}.profdata
  rm ./default.profraw
  (llvm-cov report -instr-profile=./${FNAME}.profdata ${FNAME} ${file} | sed '3q;d' | xargs ../scripts/parse-coverage.sh) || \
  (llvm-cov show   -instr-profile=./${FNAME}.profdata ${FNAME} ${file}; exit 0) # 0 FOR NOW
done
rm -f ./run-test
echo 'All good!'
