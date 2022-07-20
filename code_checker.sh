#!/bin/bash

set -ex

EXIT_CODE=0

if grep -Rn ./src -e '#include "'
then
  echo "Please always use #include <...> starting from ./src (manually included) to preserve links if we move files"
  EXIT_CODE=1
fi

for file in $(find ./src -iname '*.hpp')
do
  if [[ "$(head -n1 ${file})" != '#ifndef'* ]]
  then
    echo "Please add #include guards to ${file}"
    EXIT_CODE=1
  fi
done

for file in $(find ./src/display -type f)
do
  if [ "$(sed '3q;d' ${file})" != '#include <util/options.hpp>' ] || [ "$(sed '4q;d' ${file})" != '#if DISPLAY_ON' ]
  then
    echo -e "Please conditionally compile all ./src/DISPLAY_ON files with \"#if DISPLAY_ON\"; should be disabled on Nao"
    EXIT_CODE=1
  fi
done

if [ ${EXIT_CODE} -eq 0 ]
then
  echo "All good!"
fi

exit ${EXIT_CODE}
