#!/bin/bash

set -e

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
  if [ "$(sed '4q;d' ${file})" != '#if DISPLAY' ]
  then
    echo -e "Please conditionally compile all ./src/display files with \"#if DISPLAY\"; should be disabled on Nao"
    EXIT_CODE=1
  fi
done

if [ ${EXIT_CODE} -eq 0 ]
then
  echo "All good!"
fi

exit ${EXIT_CODE}
