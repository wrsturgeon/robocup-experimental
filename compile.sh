#!/bin/bash

# Exit on first error
set -eu

syntaxerr() { echo "Syntax: ${0} <nao|release|debug|test> modules to enable ..."; exit 1; }

if [ ${#} -lt 1 ]
then
  syntaxerr
fi



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
    sudo apt-get dist-upgrade -y gcc-10 g++-10 libstdc++-10-dev llvm clang lld
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

# With immense fucking pleasure I banish the SDL2 install time from our CI builds 🙏



#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Now begin the compilation process

# Run code checker
./scripts/code-checker.sh

# http://events17.linuxfoundation.org/sites/events/files/slides/GCC%252FClang%20Optimizations%20for%20Embedded%20Linux.pdf
SRC=${PWD}/src
SDL="$(sdl2-config --cflags --libs | sed 's|-I|-iquote |g') -Wno-poison-system-directories" # -Wno b/c not a hard-coded path
FLAGS='-std=c++20 -flto' # -fuse-ld=lld'
INCLUDES="-include ${PWD}/src/options.hpp -include ${PWD}/src/specifiers.hpp -iquote ${PWD}/src -iquote ${PWD}/eigen -iquote ${PWD}/naoqi_driver/include -iquote ${PWD}/googletest/googletest/include"
MACROS="-D_BITS=${BITS} -D_DEBUG=${DEBUG} -D_GNU_SOURCE -DLLVM_ENABLE_THREADS=1"
WARNINGS='-Weverything -Werror -pedantic-errors -Wno-c++98-compat -Wno-c++98-compat-pedantic'
export ASAN_OPTIONS='detect_leaks=1:detect_stack_use_after_return=1:detect_invalid_pointer_pairs=1:strict_string_checks=1:check_initialization_order=1:strict_init_order=1:replace_str=1:replace_intrin=1:alloc_dealloc_mismatch=1:debug=1'
export LSAN_OPTIONS="suppressions=${PWD}/lsan.supp" # Apparently Objective-C has internal memory leaks (lol)

# Enable selected modules
for arg in "${@:2}"; do
  MACROS="${MACROS} -D_$(echo ${arg} | tr '[:lower:]' '[:upper:]')_ENABLED=1"
done

if [ "${DEBUG}" -eq 1 ]
then
  FLAGS="${FLAGS} -O1 -fPIC -fno-omit-frame-pointer -fno-optimize-sibling-calls -ffunction-sections -fdata-sections"
  MACROS="${MACROS} -DEIGEN_INITIALIZE_MATRICES_BY_NAN"
  SANITIZE="-fsanitize-ignorelist=${PWD}/ignorelist.txt -fsanitize=address,undefined -fsanitize-stats -fsanitize-address-use-after-scope -Wno-error=unused-command-line-argument"
  COVERAGE='-fprofile-instr-generate -fcoverage-mapping'
  for dir in ./src/*/
  do # Enable every module
    if [ ${dir} = './src/sdl/' ]
    then # GitHub Actions has no video driver
      continue
    fi
    DIRNAME=$(echo ${dir::${#dir}-1} | rev | cut -d/ -f1 | rev | tr '[:lower:]' '[:upper:]')
    FLAGS="${FLAGS} -D_${DIRNAME}_ENABLED=1"
  done
else
  FLAGS="${FLAGS} -Ofast -march=native -mtune=native -funit-at-a-time -fno-common -fomit-frame-pointer -mllvm -polly -mllvm -polly-vectorizer=stripmine -Rpass-analysis=loop-vectorize"
  WARNINGS="${WARNINGS} -Wno-error=pass-failed -Wno-keyword-macro"
fi

ALL_FLAGS="${FLAGS} ${MACROS} ${INCLUDES} ${WARNINGS}"

if [ "${TEST}" -eq 1 ]
then
  if source ./scripts/run-tests.sh
  then
    exit 0
  else
    exit 1
  fi
else
  INCLUDES="${INCLUDES} $(sdl2-config --cflags --libs | sed 's|-I|-iquote |g')"
fi

echo 'Compiling...'
clang++ -o ./run ./src/main.cpp ${ALL_FLAGS}
echo 'Running...'
./run
echo 'Done!'

# Cleanup executables
find . -maxdepth 1 -type f ! -name LICENSE ! -iname '*.*' | xargs rm
