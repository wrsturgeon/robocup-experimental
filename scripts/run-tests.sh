#!/bin/bash

set -eu
shopt -s nullglob # Don't iterate with zero elements



# Make a folder to declutter all the executables & coverage files
mkdir -p ./coverage && cd coverage && rm -rf *



echo 'Checking leak detection...'
# Make sure we detect a genuine (test) memory leak
clang++ ../test/leak.cpp -o ./run_test ${SANITIZE} ${ALL_FLAGS}
if ./run_test >/dev/null 2>&1
then
  echo "Missed leak"
  exit 1
else
  echo "Successfully detected test leak"
fi



echo 'Compiling GoogleTest...'
clang++ -c -o ./gtest.o ../googletest/googletest/src/gtest-all.cc -iquote ../googletest/googletest -iquote ../googletest/googletest/include



echo 'Checking compilation, coverage, and memory leaks...'
EXIT_CODE=0
# Now make sure, knowing we can detect them, that there aren't any (TODO: we don't actually run these yet--implement unit testing)
for file in $(find ../test/src -type f)
do
  echo "Analyzing ${file}..."
  FNAME=$(echo ${file::${#file}-4} | rev | cut -d/ -f1 | rev)
  echo '  Compiling...'
  clang++ -o ./${FNAME} ${file} ./gtest.o ../googletest/googletest/src/gtest_main.cc ${ALL_FLAGS} ${SANITIZE} ${COVERAGE}
  echo '  Running...'
  if ! ./${FNAME} # Generate coverage at the same time
  then
    echo "./${FNAME} failed at runtime"
    EXIT_CODE=1
  fi
  echo '  Fetching coverage data...'
  llvm-profdata merge ./default.profraw -o ./${FNAME}.profdata
  rm ./default.profraw
  HPP=${SRC}/$(echo ${file} | rev | cut -d/ -f2 | rev)/${FNAME}.hpp
  if ! (llvm-cov report -instr-profile=./${FNAME}.profdata ${FNAME} ${HPP} | sed '3q;d' | xargs ../scripts/parse-coverage.sh)
  then
    llvm-cov show -instr-profile=./${FNAME}.profdata ${FNAME} ${HPP}
    EXIT_CODE=1
  fi
done
rm -f ./run_test
if [ ${EXIT_CODE} -eq 0 ]
then
  echo 'All good!'
fi
exit ${EXIT_CODE}
