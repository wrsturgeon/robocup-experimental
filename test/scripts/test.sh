#!/bin/sh

set -eu

cd ..
DIR=$(pwd)
cd build

# Make a synthetic .cpp file that just #includes all files in the project
echo '// NOLINTBEGIN(bugprone-suspicious-include)' > ./all.cpp
find ${DIR}/src  -type f -name '*\.*pp' -maxdepth 2      | sed 's/^/#include "/' | sed 's/$/"/' >> ./all.cpp
find ${DIR}/test -type f -name '*\.*pp' ! -name leak.cpp | sed 's/^/#include "/' | sed 's/$/"/' >> ./all.cpp
echo '// NOLINTEND(bugprone-suspicious-include)' >> ./all.cpp

clang-tidy ./all.cpp --quiet -- ${@}

for HPP in $(find ${DIR}/src -type f -mindepth 2 ! -name README.md ! -path '*/legacy/*' ! -path '*/util/*')
do
  NOEXTN=$(echo ${HPP} | rev | cut -d/ -f1 | cut -d. -f2- | rev) # No extension or path
  echo "Testing ${NOEXTN}..."
  rm -f ./default.profraw
  clang++ -o ./test-${NOEXTN} ${DIR}/test/${NOEXTN}.cpp ${@}
  ${DIR}/test/scripts/run-and-analyze.sh ./test-${NOEXTN}
  llvm-profdata merge ./default.profraw -o ./${NOEXTN}.profdata || (echo 'No coverage' && exit 1)
  (llvm-cov report ./test-${NOEXTN} --instr-profile=./${NOEXTN}.profdata ${HPP} | sed '3q;d' | xargs ${DIR}/test/scripts/parse-coverage.sh) || \
  (llvm-cov show ./test-${NOEXTN} --instr-profile=./${NOEXTN}.profdata ${HPP} --show-branches=count --show-expansions --show-instantiations --show-line-counts-or-regions && exit 1)
done
