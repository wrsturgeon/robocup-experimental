#!/bin/bash

set -eu
shopt -s nullglob # Don't iterate with zero elements
echo 'Checking code style and safety...'

EXIT_CODE=0

# Assert only .cpp in ./test/
INVALID_FILES=$(find ./test -type f ! -iname '*.cpp' ! -name README.md ! -path '*/scripts/*')
# Unfortunately using grep -v legacy above causes the script to crash
if [ ! -z "$(echo ${INVALID_FILES} | grep -v legacy)" ]
then
  echo '  Please only use .cpp & README.md in ./test/'
  echo "    $(${INVALID_FILES} | grep -v legacy)"
  EXIT_CODE=1
fi

# Assert only .hpp in ./src/
INVALID_FILES=$(find ./src -type f ! -iname '*.hpp' ! -name README.md)
if [ ! -z "$(echo ${INVALID_FILES} | grep -v legacy)" ]
then
  echo '  Please use only .hpp & README.md in ./src/'
  echo "    "$(echo ${INVALID_FILES} | grep -v legacy)
  EXIT_CODE=1
fi

# Assert no _, only - in filenames
find . -name .DS_Store | xargs -I{} rm {} # Mac folder info
find . -name '*.icloud' | xargs -I{} rm {} # More dumb Mac shit
INVALID_FILES=$(find ./src ./test ./src -name '*_*')
if [ ! -z "${INVALID_FILES}" ]
then
  echo '  Please use hyphens instead of underscores in filenames'
  echo "    "${INVALID_FILES}
  EXIT_CODE=1
fi

# Assert for each directory in ./src/
for dir in ./src/*/
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
    if [ ${dir} != './src/util/' ]
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

  done
done

for file in $(find ./src -mindepth 2 -type f ! -name README.md ! -path '*/legacy/*' ! -path '*/util/*')
do
  FNAME=$(echo ${file} | rev | cut -d/ -f1 | cut -d. -f2- | rev)
  TEST_FILE="./test/${FNAME}.cpp"
  if [ ! -f ${TEST_FILE} ]
  then
    echo "  Please write ${TEST_FILE} to test ${file}"
    EXIT_CODE=1
  fi
  if [ -f ${TEST_FILE} ]
  then
    if ! (head -n1 ${TEST_FILE} | grep -q '#include ".*'${FNAME}'.hpp"')
    then
      echo "  Please #include the original .hpp on the first line of ${TEST_FILE}"
      EXIT_CODE=1
    fi
    if [ "$(sed '2q;d' ${TEST_FILE})" != '' ]
    then
      echo "  Please leave a blank line between your hpp and gtest in ${TEST_FILE}"
      EXIT_CODE=1
    else
      if ! (sed '3q;d' ${TEST_FILE} | grep -q '#include "gtest.hpp"')
      then
        echo '  Please #include "gtest.hpp" on the third line of '${TEST_FILE}
        EXIT_CODE=1
      fi
    fi
  fi
  for occurrence in $(grep -n 'class .* {' ${file} | cut -d: -f1)
  do
    occurrence=$((occurrence+1))
    line_contents=$(sed ${occurrence}'q;d' ${file})
    while [ "${line_contents}" != '};' ]
    do
      if ((echo "${line_contents}" | grep '^   ' > /dev/null) || (echo "${line_contents}" | grep '^  {' > /dev/null))
      then
        echo -e "  Please don't define multi-line items within a \`class\` block ( ${file}:${occurrence} )"
        EXIT_CODE=1
      fi
      if echo "${line_contents}" | grep '^$' > /dev/null
      then
        echo -e "  Please don't leave blank lines within a \`class\` block ( ${file}:${occurrence} )"
        EXIT_CODE=1
      fi
      occurrence=$((occurrence+1))
      line_contents=$(sed ${occurrence}'q;d' ${file})
    done
  done
done

if [ ${EXIT_CODE} -eq 0 ]
then
  echo "  All good!"
fi

exit ${EXIT_CODE}