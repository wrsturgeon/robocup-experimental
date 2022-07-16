#!/bin/bash

# https://developer.softbankrobotics.com/nao-naoqi-2-1/naoqi-developer-guide/getting-started/c-sdk-installation

set -ex

pip install -U qibuild --user
PIP_BIN=$(pip show qibuild | grep Location | head -n1 | cut -d' ' -f2)
PIP_BIN=${PIP_BIN%$*lib*}bin
rm -rf ./.qi ./upennalizers
${PIP_BIN}/qibuild config --wizard
${PIP_BIN}/qibuild init
${PIP_BIN}/qisrc create upennalizers
${PIP_BIN}/qibuild configure upennalizers
${PIP_BIN}/qibuild make upennalizers
