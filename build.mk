# Automatically hard-linked into ./build/ in the main Makefile and called from inside.

.PHONY: eigen vcl highway naoqi-driver naoqi-sdk test check-leak-detection

OS := $(shell if [ $(shell uname -s) = Darwin ]; then echo mac; else echo linux; fi) # fuck Windows 💪🤝🚫🪟
CORES := $(shell if [ $(OS) = linux ]; then nproc --all; else sysctl -n hw.ncpu; fi)
BITS := $(shell getconf LONG_BIT)

CXX := clang++

DIR := $(shell cd .. && pwd)
SRC := $(DIR)/src
TPY := $(DIR)/third-party
TST := $(DIR)/test
SCT := $(TST)/scripts

FLAGS := -std=gnu++20 -ferror-limit=1 -ftemplate-backtrace-limit=0
INCLUDES := -iquote $(SRC) -isystem $(TPY)/eigen $(shell find $(SRC)/util -type f ! -name README.md | xargs -I{} echo '-include {}')
MACROS := -DBITS=$(BITS) -DOS=$(strip $(OS)) -DCORES=$(CORES)
WARNINGS := -Weverything -Werror -pedantic-errors -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-c++20-compat -Wno-keyword-macro -Wno-poison-system-directories -Wno-missing-prototypes
COMMON := $(strip $(FLAGS)) $(strip $(MACROS)) $(strip $(INCLUDES)) $(strip $(WARNINGS))

DEBUG_FLAGS   := -O0 -fno-omit-frame-pointer -g -fno-optimize-sibling-calls -fsanitize=address -fno-common -fsanitize-address-use-after-scope -fsanitize-address-use-after-return=always -DEIGEN_INITIALIZE_MATRICES_BY_NAN
RELEASE_FLAGS := -Ofast -fomit-frame-pointer -flto -march=native -mtune=native -mllvm -polly -mllvm -polly-vectorizer=stripmine -Rpass-analysis=loop-vectorize -DNDEBUG
COVERAGE := -fprofile-instr-generate -fcoverage-mapping
TEST_FLAGS := $(strip $(DEBUG_FLAGS)) $(strip $(COVERAGE)) -isystem $(TPY)/gtest/googletest/include -Wno-padded -Wno-weak-vtables



# Release: no debug symbols, no bullshit, just as fast as possible
release:
	echo 'TODO'

naoqi-sdk: $(TPY)
	echo '  naoqi-sdk'
	if [ ! -d $(<)/naoqi-sdk ]; then \
		wget -q -O naoqi-sdk.tar.gz https://community-static.aldebaran.com/resources/2.1.4.13/sdk-c%2B%2B/naoqi-sdk-2.1.4.13-$(strip $(OS))$(BITS).tar.gz && \
		tar -xzf naoqi-sdk.tar.gz && \
		rm naoqi-sdk.tar.gz && \
		find . -type d -maxdepth 1 -iname 'naoqi-sdk*' -print -quit | xargs -I{} mv {} $(<)/naoqi-sdk; \
	fi



#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Compilation

compile = echo "Compiling $(@)..." && $(CXX) -o ./$(@) $(<) $(strip $(COMMON))
TEST_CLANG_ARGS = $(strip $(COMMON)) gmain.o gtest.o $(strip $(TEST_FLAGS))

deps = $(SRC)/$(1).hpp



# Testing
gtest.o:
	echo 'Compiling GoogleTest libraries...'
	$(CXX) -o ./gtest.o -c -w -O0 -iquote $(TPY)/gtest/googletest -iquote $(TPY)/gtest/googletest/include $(TPY)/gtest/googletest/src/gtest-all.cc
gmain.o:
	echo 'Compiling GoogleTest main function...'
	$(CXX) -o ./gmain.o -c -w -O0 -iquote $(TPY)/gtest/googletest -iquote $(TPY)/gtest/googletest/include $(TPY)/gtest/googletest/src/gtest_main.cc

check-leak-detection: ../test/leak.cpp
	$(compile) $(strip $(TEST_FLAGS))
	echo '  Catching intentional leak...'
ifdef VERBOSE
	! $(SCT)/run-and-analyze.sh ./check-leak-detection
else
	! $(SCT)/run-and-analyze.sh ./check-leak-detection >/dev/null 2>&1
endif
	rm ./check-leak-detection
	echo '  Got it!'

test: check-leak-detection gmain.o gtest.o
	$(SCT)/test.sh $(TEST_CLANG_ARGS)



# TODO: use PGO (profiling-guided optimization)
