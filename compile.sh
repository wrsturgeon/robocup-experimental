#!/bin/bash

# Exit on first error
set -eu

syntaxerr() { echo "Syntax: ${0} <nao|release|debug|test> modules to enable ..."; exit 1; }

if [ ${#} -lt 1 ]
then
  syntaxerr
fi

# Run code checker
./scripts/code-checker.sh



#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Collecting data on OS & intended build

# Read command-line argument for build type
DEBUG=0
TEST=0
NAO=0
case "${1}" in
  release)
    ;; # for now
  debug)
    DEBUG=1
    ;;
  test)
    DEBUG=1
    TEST=1
    ;;
  nao)
    NAO=1
    ;;
  *)
    syntaxerr
    ;;
esac

# Detect operating system
case "$(uname -s)" in
  Darwin)
    OS=mac
    PATH=/usr/local/opt/llvm/bin:${PATH}
    brew upgrade
    brew install llvm 2>/dev/null
    set +e # Ignore error if command-line tools are already installed
    xcode-select --install 2>/dev/null
    set -e # Start caring about errors again
    ;;
  Linux)
    OS=linux
    sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
    sudo apt-get update -y
    sudo apt-get dist-upgrade -y gcc-10 g++-10 libstdc++-10-dev
    sudo apt-get install -y lld
    ;;
  *)
    echo 'Unsupported OS'
    exit 1
    ;;
esac

# Detect architecture
BITS=$(getconf LONG_BIT)
if [ "${OS}" = 'mac' ]
then
  CORES=$(sysctl -n hw.ncpu)
else
  CORES=$(nproc --all)
fi



#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Ensuring we have everything we need

# Download updated submodules (Eigen, etc.)
git submodule update --init --recursive

# Download NaoQI SDK if not already present
if [ ! -d ./naoqi-sdk ]
then
  # V5 SDKS ARE ALL HERE, THEY'RE JUST SO OLD THERE ARE NO LINKS TO THIS PAGE ANYWHERE <3
  # https://www.softbankrobotics.com/emea/en/support/nao-6/downloads-softwares/former-versions

  # Download
  wget -q -O naoqi-sdk.tar.gz "https://community-static.aldebaran.com/resources/2.1.4.13/sdk-c%2B%2B/naoqi-sdk-2.1.4.13-${OS}${BITS}.tar.gz"
  
  # Unpack
  # This is going to make Git go berserk
  tar -xzf naoqi-sdk.tar.gz
  
  # Delete the original compressed file
  rm naoqi-sdk.tar.gz
  
  # Remove the horrific filename extension
  # The resulting folder (naoqi-sdk) is .gitignore'd
  find . -type d -maxdepth 1 -iname 'naoqi-sdk*' -print -quit | xargs -I{} mv {} ./naoqi-sdk
fi

if [ "${DEBUG}" -eq 1 ]
then
  # Download SDL2 if not already present
  sdl2-config --version 2>/dev/null > /dev/null || (\
    git clone https://github.com/libsdl-org/SDL && \
    cd ./SDL && \
    mkdir build && cd build && \
    ../autogen.sh && \
    ../configure && \
    make -j${CORES} && \
    sudo make install && \
    cd ../.. && sudo rm -r ./SDL)
fi



#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Now begin the compilation process

# http://events17.linuxfoundation.org/sites/events/files/slides/GCC%252FClang%20Optimizations%20for%20Embedded%20Linux.pdf
FLAGS='-std=c++20 -ferror-limit=1'
INCLUDES="-iquote ./src -include ./src/options.hpp -include ./src/specifiers.hpp -isystem ./eigen -isystem ./naoqi_driver/include $(sdl2-config --cflags --libs | sed 's|-I|-isystem |g')"
MACROS="-D_BITS=${BITS} -D_DEBUG=${DEBUG} -D_GNU_SOURCE -DLLVM_ENABLE_THREADS=1"
WARNINGS='-Wall -Wextra -Werror -Wno-builtin-macro-redefined -Wstrict-aliasing -Wthread-safety -Wself-assign -Wno-missing-field-initializers -pedantic-errors -Wno-keyword-macro -Wno-zero-length-array'

# Enable selected modules
for arg in "${@:2}"; do
  MACROS="${MACROS} -D_$(echo ${arg} | tr '[:lower:]' '[:upper:]')_ENABLED=1"
done

if [ "${DEBUG}" -eq 1 ]
then
  FLAGS="${FLAGS} -g -O1 -fno-omit-frame-pointer -fno-optimize-sibling-calls -fprofile-instr-generate -fcoverage-mapping -U_FORTIFY_SOURCE -fsanitize=address,undefined,cfi"
  MACROS="${MACROS} -DEIGEN_INITIALIZE_MATRICES_BY_NAN -DG"
else
  FLAGS="${FLAGS} -Ofast -march=native -mtune=native -funit-at-a-time -flto -Ofast -fvisibility=hidden -fno-common -fomit-frame-pointer -mllvm -polly -mllvm -polly-vectorizer=stripmine -Rpass-analysis=loop-vectorize"
fi

# Enable every module if we're testing
if [ "${TEST}" -eq 1 ]
then
  for dir in ./src/*/
  do # Enable every module
    DIRNAME=$(echo ${dir::${#dir}-1} | rev | cut -d/ -f1 | rev | tr '[:lower:]' '[:upper:]')
    FLAGS="${FLAGS} -D_${DIRNAME}_ENABLED=1"
  done
fi

ALL_FLAGS="${FLAGS} ${MACROS} ${INCLUDES} ${WARNINGS}"

if [ "${TEST}" -eq 1 ]
then
  echo 'Checking compilation...'
  find ./src -type f -name '*.*pp' | xargs -I{} clang++ -c -std=c++20 -o ./tmp_compiled {} ${ALL_FLAGS} -Wno-unused-command-line-argument # Unused linker okay
	rm -f ./tmp_compiled
  echo 'All good!'
fi

# https://github.com/google/sanitizers/wiki/AddressSanitizerFlags
export ASAN_OPTIONS='detect_leaks=1:detect_stack_use_after_return=1:detect_invalid_pointer_pairs=1:strict_string_checks=1:check_initialization_order=1:strict_init_order=1:replace_str=1:replace_intrin=1:alloc_dealloc_mismatch=1:debug=1'
export LSAN_OPTIONS='suppressions=lsan.supp' # Apparently Objective-C has internal memory leaks (lol)
echo 'Compiling...'
clang++ -o run ./src/main.cpp ${ALL_FLAGS}
echo 'Running...'
./run
echo 'Done!'
