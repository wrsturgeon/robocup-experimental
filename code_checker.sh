#!/bin/bash

set -e

if grep -rnw ./src -e '#include "'; then
  echo "Please always use #include <...> starting from ./src (manually included) to preserve links if we move files";
  exit 1
fi

echo "All good!"
