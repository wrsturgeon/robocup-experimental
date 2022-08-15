# Automatically hard-linked into ./build/ in the main Makefile and called from inside.

.PHONY: eigen vcl highway naoqi-driver naoqi-sdk test check-leak-detection

OS := $(shell if [ $(shell uname -s) = Darwin ]; then echo mac; else echo linux; fi) # fuck Windows 💪🤝🚫🪟
CORES := $(shell if [ $(OS) = linux ]; then nproc --all; else sysctl -n hw.ncpu; fi)
BITS := $(shell getconf LONG_BIT)

CXX := clang++ # $(shell if [ $(OS) = linux ]; then echo clang++; else echo /usr/local/opt/llvm/bin/clang++; fi)

DIR := $(shell cd .. && pwd)
SRC := $(DIR)/src
TPY := $(DIR)/third-party
TST := $(DIR)/test
SCT := $(TST)/scripts

ALL_TESTS := $(foreach dir,$(shell find $(SRC) -type f -mindepth 2 ! -name README.md ! -path '*/legacy/*' ! -path '*/util/*' | rev | cut -d/ -f1 | cut -d. -f2- | rev),test-$(dir))

FLAGS := -std=gnu++20 -ferror-limit=1 -ftemplate-backtrace-limit=0
INCLUDES := -include $(SRC)/options.hpp -iquote $(SRC) #$(shell find $(SRC)/util -type f ! -name README.md | xargs -I{} echo '-include {}')
MACROS := -D_BITS=$(BITS) -D_OS=$(strip $(OS)) -D_CORES=$(CORES)
WARNINGS := -Weverything -Werror -pedantic-errors -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-c++20-compat -Wno-keyword-macro -Wno-poison-system-directories -Wno-missing-prototypes
COMMON := $(strip $(FLAGS)) $(strip $(MACROS)) $(strip $(INCLUDES)) $(strip $(WARNINGS))

DEBUG_FLAGS   := -O0 -fno-omit-frame-pointer -g -fno-optimize-sibling-calls -fsanitize=address -fno-common -fsanitize-address-use-after-scope -fsanitize-address-use-after-return=always -DEIGEN_INITIALIZE_MATRICES_BY_NAN -D_DEBUG
RELEASE_FLAGS := -Ofast -fomit-frame-pointer -flto -march=native -mtune=native -mllvm -polly -mllvm -polly-vectorizer=stripmine -Rpass-analysis=loop-vectorize
COVERAGE := -fprofile-instr-generate -fcoverage-mapping
TEST_FLAGS := $(strip $(DEBUG_FLAGS)) $(strip $(COVERAGE)) -Wno-padded -Wno-weak-vtables

INCLUDE_EIGEN=-iquote $(TPY)/eigen
INCLUDE_GTEST=-iquote $(TPY)/gtest/googletest/include
INCLUDE_VCL=-iquote $(TPY)/vcl
INCLUDE_NAOQI_DRIVER=-iquote $(TPY)/naoqi-driver
INCLUDE_NAOQI_SDK=-iquote $(TPY)/naoqi-sdk



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

prereqs = $(foreach library,$(|),-iquote $(TPY)/$(library))
compile = echo "Compiling $(@)..." && clang++ -o ./$(@) $(<) $(strip $(COMMON))
compile-bin = $(compile) $(prereqs) $(strip $(RELEASE_FLAGS))
compile-lib = $(compile-bin) -c
TEST_CLANG_ARGS = $(strip $(COMMON)) gmain.o gtest.o $(prereqs) $(strip $(INCLUDE_GTEST)) $(strip $(TEST_FLAGS))
compile-tst = echo "Tidying $(@)..." && \
clang-tidy $(word 2,$(^)) $(word 3,$(^)) --quiet -- $(subst iquote,isystem,$(TEST_CLANG_ARGS)) && \
echo '  All good; compiling...' && clang++ -o ./$(@) $(<) $(TEST_CLANG_ARGS) -include $(word 2,$(^))

deps = $(SRC)/$(1).hpp



# Testing
gtest.o:
	echo 'Compiling GoogleTest libraries...'
	clang++ -o ./gtest.o -c -w -O0 $(COMMON) $(INCLUDE_GTEST) -iquote $(TPY)/gtest/googletest $(TPY)/gtest/googletest/src/gtest-all.cc
gmain.o:
	echo 'Compiling GoogleTest main function...'
	clang++ -o ./gmain.o -c -w -O0 $(COMMON) $(INCLUDE_GTEST) -iquote $(TPY)/gtest/googletest $(TPY)/gtest/googletest/src/gtest_main.cc

test-distortion: $(TST)/distortion.cpp $(call deps,vision/distortion) | eigen
	$(compile-tst)
test-field-lines: $(TST)/field-lines.cpp $(call deps,measure/field-lines) | eigen
	$(compile-tst)
test-image-api: $(TST)/image-api.cpp $(call deps,vision/image-api) | eigen
	$(compile-tst)
test-pxpos: $(TST)/pxpos.cpp $(call deps,vision/pxpos)
	$(compile-tst)
test-pyramid: $(TST)/pyramid.cpp $(call deps,wasserstein/pyramid) | eigen
	$(compile-tst)
test-rshift: $(TST)/rshift.cpp $(call deps,util/rshift)
	$(compile-tst)
test-scrambler: $(TST)/scrambler.cpp $(call deps,training/scrambler)
	$(compile-tst)
test-units: $(TST)/units.cpp $(call deps,measure/units) | eigen
	$(compile-tst)
test-xoshiro: $(TST)/xoshiro.cpp $(call deps,rnd/xoshiro)
	$(compile-tst)

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

noextn = $(shell echo $(1) | rev | cut -d/ -f1 | cut -d. -f2- | rev)

test: check-leak-detection gmain.o gtest.o $(ALL_TESTS)
	$(SCT)/test.sh



# TODO: use PGO (profiling-guided optimization)
