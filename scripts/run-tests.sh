#!/bin/bash

set -eu

echo 'Checking leak detection...'
# Make sure we detect a genuine (test) memory leak
clang++ ./test/leak.cpp -o ./leak ${ALL_FLAGS}
if ./leak >/dev/null 2>&1
then
  echo "Missed leak"
  exit 1
else
  echo "Successfully detected test leak"
fi

echo 'Checking compilation...'
# Now make sure, knowing we can detect them, that there aren't any (TODO: we don't actually run these yet--implement unit testing)
find ./test -name '_*' | xargs -I{} clang++ -o ./run_test {} ${ALL_FLAGS} && ./run_test
find ./test -type f ! -name '_*' ! -name 'README.md' ! -name 'leak.cpp' | xargs -I{} clang++ -o ./run_test {} ${ALL_FLAGS} && ./run_test
rm -f ./tmp_compiled
echo 'All good!'
