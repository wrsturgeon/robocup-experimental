#!/bin/bash

set -eu
shopt -s nullglob # Don't iterate with zero elements
echo 'Checking code style and safety...'

EXIT_CODE=0

# Assert only .cpp in ./src/ & ./test/
INVALID_FILES=$(find ./src ./test -type f ! -iname '*.cpp' ! -name README.md)
# Unfortunately using grep -v legacy above causes the script to crash
if [ ! -z "$(echo ${INVALID_FILES} | grep -v legacy)" ]
then
  echo '  Please only use .cpp & README.md in ./src/ & ./test/'
  echo "    $(${INVALID_FILES} | grep -v legacy)"
  EXIT_CODE=1
fi

# Assert only .hpp in ./include/
INVALID_FILES=$(find ./include -type f ! -iname '*.hpp' ! -name README.md)
if [ ! -z "$(echo ${INVALID_FILES} | grep -v legacy)" ]
then
  echo '  Please use only .hpp & README.md in ./include/'
  echo "    "$(echo ${INVALID_FILES} | grep -v legacy)
  EXIT_CODE=1
fi

# Assert no _, only - in filenames
find . -name .DS_Store | xargs -I{} rm {} # Mac folder info
find . -name '*.icloud' | xargs -I{} rm {} # More dumb Mac shit
INVALID_FILES=$(find ./src ./test ./include -name '*_*')
if [ ! -z "${INVALID_FILES}" ]
then
  echo '  Please use hyphens instead of underscores in filenames'
  echo "    "${INVALID_FILES}
  EXIT_CODE=1
fi

# Assert all folders in ./src/ and ./include/ are mutual
for dir in $(find ./src -type d -mindepth 1)
do
  if [ ! -d "./include/${dir:6}" ]
  then
    echo "  Missing ./include/${dir:6}"
    EXIT_CODE=1
  fi
done
for dir in $(find ./include -type d -mindepth 1 ! -name util)
do
  if [ ! -d "./src/${dir:10}" ]
  then
    echo "  Missing ./src/${dir:10}"
    EXIT_CODE=1
  fi
done

# Assert all existing cpp-hpp pairs are properly fused
# Assert no one-liners in cpp files (with great sorrow, but for coverage <3)
for file in $(find ./src -type f ! -name README.md)
do
  HEADER=$(echo ${file:6} | rev | cut -d. -f2- | rev).hpp
  DIRNAME=$(echo ${file:6} | rev | cut -d/ -f2 | rev)
  if ([ -f ./include/${HEADER} ] && [ "$(head -n4 ${file} | tr -d '\n')" != '#include "'${HEADER}'"namespace '${DIRNAME}' {' ])
  then
    echo "  Please #include \"${HEADER}\" on the first line of ${file} and open the ${DIRNAME} namespace"
    EXIT_CODE=1
  fi
  if grep -nq '{.*;.*}' ${file}
  then
    echo '  Please define braced blocks on a separate line for coverage'
    grep -n '{.*;.*}' ${file} | cut -d: -f1 | xargs -I{} echo '    '${file}:{}
    EXIT_CODE=1
  fi
done

# Assert "eigen.h" and not any of Eigen's headers
if grep -Rn ./src ./include -e '#include' --exclude=eigen.hpp 2>/dev/null | grep Eigen
then
  echo "  Please #include \"eigen.hpp\" instead of Eigen's internal headers"
  EXIT_CODE=1
fi

# Assert no manual "options.hpp"
if grep -Rn ./src ./include -e 'options.hpp' --exclude=options.hpp 2>/dev/null
then
  echo "  Please don't manually #include "options.hpp"; it's included automatically\n"
  EXIT_CODE=1
fi

# Assert no manual "gtest/gtest.h"
if grep -Rn ./src ./test -e 'gtest/gtest.h' --exclude=gtest.hpp
then
  echo "Please #include "gtest.hpp" instead of "gtest/gtest.h" to avoid 3rd-party errors showing up as ours\n"
  EXIT_CODE=1
fi

# Assert no manual "eigen-matrix-plugin.hpp"
if grep -Rn ./src ./include -e 'eigen-matrix-plugin.hpp' --exclude=eigen.hpp 2>/dev/null
then
  echo "  Please don't manually #include \"eigen-matrix-plugin.hpp\"; it's included in Eigen::Matrix"
  EXIT_CODE=1
fi

# Assert #include guards & namespaces
for dir in ./include/*/
do
  dirname=$(echo ${dir::${#dir}-1} | rev | cut -d/ -f1 | rev)
  DIRUPPER=$(echo ${dirname} | tr '[:lower:]' '[:upper:]')

  # Make sure folder name is lowercase
  if [ "${dirname}" != "$(echo ${dirname} | tr '[:upper:]' '[:lower:]')" ]
  then
    echo "  Please lowercase folder ${dir}"
    EXIT_CODE=1
  fi

  # Make sure the folder has its own README
  if [ ! -f ${dir}/README.md ] && [ ! -L ${dir}/README.md ]
  then
    echo "  Symlinking README.md from ${PWD}/src/${dirname}/README.md to ${PWD}/${dir}README.md..."
    ln -s ${PWD}/src/${dirname}/README.md ${PWD}/${dir:2}README.md
  fi

  for file in ${dir}*.hpp
  do
    filename=$(echo ${file} | rev | cut -d/ -f1 | rev)
    FILEUPPER=$(echo ${filename} | tr '[:lower:]' '[:upper:]' | tr './-' '_')

    # No #includes inside namespace
    if [ ${dir} != './include/util/' ]
    then
      LAST_NAMESPACE=$(grep -n '^namespace' ${file} | tail -n1 | cut -d: -f1)
      if [ -z "${LAST_NAMESPACE}" ]
      then
        echo "  Please use \`namespace ${dirname}\` in ${file}"
        EXIT_CODE=1
      else
        LAST_INCLUDE="$(grep -n '#include' ${file} | tail -n1 | cut -d: -f1)"
        if ([ ! -z "${LAST_INCLUDE}" ] && [ ${LAST_INCLUDE} -gt "${LAST_NAMESPACE}" ])
        then
          echo "  Please make sure all headers are #include'd before opening a namespace in ${file}"
          echo "    Last #include on line ${LAST_INCLUDE}; last namespace on line ${LAST_NAMESPACE}"
          echo
          EXIT_CODE=1
        fi
      fi
    fi

    # Specific-to-general #includes
    LAST_LOCAL=$(grep -n '#include "' ${file} | tail -n1 | cut -d: -f1)
    FIRST_SYSTEM=$(grep -n '#include <' ${file} | head -n1 | cut -d: -f1)
    if [ ! -z "${LAST_LOCAL}" ] && [ ! -z "${FIRST_SYSTEM}" ] && [ "${LAST_LOCAL}" -gt "${FIRST_SYSTEM}" ]
    then
      echo "  Please #include in reverse-dependency order (specific to general) so we don't omit necessary #includes in other files"
      echo "    In ${file}: last local include (\"...\") on line ${LAST_LOCAL}; first system include (<...>) on line ${FIRST_SYSTEM}"
      echo
      EXIT_CODE=1
    fi

    for hpp in $(find ./include -type f ! -path '*/util/*' ! -name eigen-matrix-plugin.hpp)
    do
      if grep -n ${hpp} -e '{' | grep -v namespace | grep -v class | grep -v struct | grep -v global
      then
        echo "  Please don't define anything in .hpp files (just declare)"
        EXIT_CODE=1
      fi
    done

  done
done

for file in $(find ./src -mindepth 2 -type f ! -name README.md ! -path '*/legacy/*')
do
  FNAME=$(echo ${file} | rev | cut -d/ -f1 | cut -d. -f2- | rev)
  TEST_FILE="./test/${FNAME}.cpp"
  if [ ! -f ${TEST_FILE} ]
  then
    echo "  Please write ${TEST_FILE} to test ${file}"
    EXIT_CODE=1
  fi
  if ! (head -n1 ${TEST_FILE} | grep -q '#include ".*'${FNAME}'.hpp"')
  then
    echo "  Please #include the original .hpp on the first line of ${TEST_FILE}"
    EXIT_CODE=1
  fi
  if ! (sed '2q;d' ${TEST_FILE} | grep -q '#include "gtest.hpp"')
  then
    echo '  Please #include "gtest.hpp" on the second line of '${TEST_FILE}
    EXIT_CODE=1
  fi
done

if [ ${EXIT_CODE} -eq 0 ]
then
  echo "  All clear!"
fi

exit ${EXIT_CODE}
