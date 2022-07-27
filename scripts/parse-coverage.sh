#!/bin/bash

set -eu

if [ ${3} -ne 0 ]
then
  echo "Missing ${3} region(s) in ${1} (${4} covered)"
  exit 1
fi

if [ ${6} -ne 0 ]
then
  echo "Missing ${6} function(s) in ${1} (${7} covered)"
  exit 1
fi

if [ ${9} -ne 0 ]
then
  echo "Missing ${9} line(s) in ${1} (${10} covered)"
  exit 1
fi

if [ ${12} -ne 0 ]
then
  echo "Missing ${12} branch(es) in ${1} (${13} covered)"
  exit 1
fi
