#!/bin/bash

set -eu
shopt -s nullglob # Don't iterate with zero elements
echo -e 'Checking style & safety...\n'

EXIT_CODE=0

# Assert only .cpp & .hpp in ./src/
INVALID_FILES=$(find ./src -type f -not -iname '*.hpp' -not -iname '*.cpp' -not -name README.md)
# Unfortunately using grep -v legacy above causes the script to crash
if [ ! -z "$(echo ${INVALID_FILES} | grep -v legacy)" ]
then
  echo ${INVALID_FILES} | grep -v legacy | tr ' ' '\n'
  echo -e 'Please only use .cpp, .hpp, & README.md in ./src/\n'
  EXIT_CODE=1
fi

# Assert only '#include <...>' and not '#include "..."'
if grep -Rn ./src -e '#include "\.\.'
then
  echo -e "Please always use #include starting from ./src (manually included) to preserve links if we move files\n"
  EXIT_CODE=1
fi

# Assert <eigen.h> and not any of Eigen's headers
if grep -Rn ./src -e '#include' --exclude=eigen.hpp | grep Eigen
then
  echo -e "Please #include <eigen.hpp> instead of Eigen's internal headers\n"
  EXIT_CODE=1
fi

# Assert no manual <options.hpp>
if grep -Rn ./src -e 'options.hpp' --exclude=options.hpp
then
  echo -e "Please don't manually #include <options.hpp>; it's included automatically\n"
  EXIT_CODE=1
fi

# Assert no manual <specifiers.hpp>
if grep -Rn ./src -e 'specifiers.hpp' --exclude=specifiers.hpp
then
  echo -e "Please don't manually #include <specifiers.hpp>; it's included automatically\n"
  EXIT_CODE=1
fi

# Assert #include guards & namespaces
for dir in ./src/*/
do
  dirname=$(echo ${dir::${#dir}-1} | rev | cut -d/ -f1 | rev)
  DIRUPPER=$(echo ${dirname} | tr '[:lower:]' '[:upper:]')

  # Make sure folder name is lowercase
  if [ "${dirname}" != "$(echo ${dirname} | tr '[:upper:]' '[:lower:]')" ]
  then
    echo -e "Please lowercase folder ${dir}\n"
    EXIT_CODE=1
  fi

  # Make sure the folder has its own README
  if [ ! -f "${dir}/README.md" ]
  then
    echo -e "Please create a README.md in ${dir}\n"
    EXIT_CODE=1
  fi

  # Make sure this folder is known to src/options.hpp
  if ! grep -qw "_${DIRUPPER}_ENABLED" ./src/options.hpp
  then
    echo -e "Please add _${DIRUPPER}_ENABLED support to ./src/options.hpp\n"
    EXIT_CODE=1
  fi

  for file in ${dir}*.hpp
  do
    filename=$(echo ${file} | rev | cut -d/ -f1 | rev)
    FILEUPPER=$(echo ${filename} | tr '[:lower:]' '[:upper:]' | tr '.' '_')

    # Include guards at the top
    if [ "$(head -n4 ${file} | tr -d '\n')" != "#if ${DIRUPPER}_ENABLED#ifndef ${DIRUPPER}_${FILEUPPER}_#define ${DIRUPPER}_${FILEUPPER}_" ]
    then
      echo -e "Missing or unsafe #include guards at the top of ${file}; please use the following:\n\n#if ${DIRUPPER}_ENABLED\n#ifndef ${DIRUPPER}_${FILEUPPER}_\n#define ${DIRUPPER}_${FILEUPPER}_\n\n[your #includes]\n\nnamespace ${dirname} {\n"
      EXIT_CODE=1
    fi

    # Ending those guards & namespace
    if [ "$(tail -n8 ${file} | tr -d '\n')" != "} // namespace ${dirname}#endif // ${DIRUPPER}_${FILEUPPER}_#else // ${DIRUPPER}_ENABLED#pragma message(\"Skipping ${filename}; ${dirname} module disabled\")#endif // ${DIRUPPER}_ENABLED" ]
    then
      echo -e "Missing or unsafe #include guards and namespace at the bottom of ${file}; please use the following:\n\n} // namespace ${dirname}\n\n#endif // ${DIRUPPER}_${FILEUPPER}_\n\n#else // ${DIRUPPER}_ENABLED\n#pragma message(\"Skipping ${filename}; ${dirname} module disabled\")\n#endif // ${DIRUPPER}_ENABLED\n"
      EXIT_CODE=1
    fi

    # Ending newline
    if [ ! -z "$(tail -c1 ${file})" ]
    then
      echo -e "Missing newline at the end of ${file}\n"
      EXIT_CODE=1
    fi

    # No #includes inside namespace
    LAST_NAMESPACE=$(grep -n '^namespace' ${file} | tail -n1 | cut -d: -f1)
    if [ -z "${LAST_NAMESPACE}" ]
    then
      echo -e "Please use \`namespace ${dirname}\` in ${file}\n"
    else
      LAST_INCLUDE="$(grep -n '#include' ${file} | tail -n1 | cut -d: -f1)"
      if [ ! -z "${LAST_INCLUDE}" ] && [ ${LAST_INCLUDE} -gt "${LAST_NAMESPACE}" ]
      then
        echo -e "Please make sure all headers are #include'd before opening a namespace in ${file}\n"
        EXIT_CODE=1
      fi
    fi

  done
done

if [ ${EXIT_CODE} -eq 0 ]
then
  echo -e "All good!\n"
fi

exit ${EXIT_CODE}
