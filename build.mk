# Automatically hard-linked into ./build/ in the main Makefile and called from inside.

.PHONY: eigen vcl highway naoqi-driver naoqi-sdk test check-leak-detection

OS := $(shell if [ $(shell uname -s) = Darwin ]; then echo mac; else echo linux; fi) # fuck Windows 💪🤝🚫🪟
CORES := $(shell if [ $(OS) = linux ]; then nproc --all; else sysctl -n hw.ncpu; fi)
BITS := $(shell getconf LONG_BIT)

CXX := clang++ # $(shell if [ $(OS) = linux ]; then echo clang++; else echo /usr/local/opt/llvm/bin/clang++; fi)

DIR := $(shell cd .. && pwd)
SRC := $(DIR)/src
INC := $(DIR)/include
TPY := $(DIR)/third-party
TST := $(DIR)/test
SCT := $(DIR)/scripts

ALL_TESTS := $(foreach dir,$(shell find $(SRC) -type f -mindepth 2 -iname '*.cpp' | rev | cut -d/ -f1 | cut -d. -f2- | rev),test_$(dir))

FLAGS := -std=gnu++20 -flto -ferror-limit=1 -ftemplate-backtrace-limit=0
INCLUDES := -include $(INC)/options.hpp -iquote $(INC)
MACROS := -D_BITS=$(BITS) -D_OS=$(strip $(OS)) -D_CORES=$(CORES)
WARNINGS := -Weverything -Werror -pedantic-errors -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-keyword-macro -Wno-poison-system-directories
COMMON := $(strip $(FLAGS)) $(strip $(INCLUDES)) $(strip $(MACROS)) $(strip $(WARNINGS))

DEBUG_FLAGS   := -O0 -fno-omit-frame-pointer -g -fno-optimize-sibling-calls -DEIGEN_INITIALIZE_MATRICES_BY_NAN
RELEASE_FLAGS := -Ofast -fomit-frame-pointer -march=native -mtune=native -fno-common -mllvm -polly -mllvm -polly-vectorizer=stripmine -Rpass-analysis=loop-vectorize
SANITIZE := -fsanitize=leak
COVERAGE := -fprofile-instr-generate -fcoverage-mapping
TEST_FLAGS := $(strip $(DEBUG_FLAGS)) $(strip $(SANITIZE)) $(strip $(COVERAGE)) -Wno-padded -Wno-weak-vtables

INCLUDE_EIGEN=-iquote $(TPY)/eigen
INCLUDE_GTEST=-iquote $(TPY)/gtest/googletest/include
INCLUDE_VCL=-iquote $(TPY)/vcl
INCLUDE_NAOQI_DRIVER=-iquote $(TPY)/naoqi-driver
INCLUDE_NAOQI_SDK=-iquote $(TPY)/naoqi-sdk



# Release: no debug symbols, no bullshit, just as fast as possible
release: release-flags
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

compile = echo "Compiling $(@)..." && clang++ -o ./$(@) $(<) $(strip $(COMMON))
compile-bin = $(compile) $(call nth_prereqs,3) $(strip $(RELEASE_FLAGS))
compile-lib = $(compile-bin) -c
compile-tst = clang++ -o ./$(@) $(<) $(strip $(COMMON)) -include $(word 2,$(^)) gmain.o gtest.o $(call nth_prereqs,4) $(strip $(INCLUDE_GTEST)) $(strip $(TEST_FLAGS))

nth_prereqs = $(shell echo $(^) $(foreach library,$(|),-iquote $(TPY)/$(library)) | cut -d' ' -f$(1)-)

deps = $(SRC)/$(1).cpp $(INC)/$(1).hpp



# No dependencies
pxpos.o: $(call deps,vision/pxpos)
	$(compile-lib)
xoshiro.o: $(call deps,rnd/xoshiro)
	$(compile-lib)

# Only third-party libraries
units.o: $(call deps,measure/units) | eigen
	$(compile-lib)
image-api.o: $(call deps,vision/image-api) | eigen
	$(compile-lib)

# All others, in some dependency-based order
distortion.o: $(call deps,vision/distortion) | eigen
	$(compile-lib)



# Testing
gtest.o:
	echo 'Compiling GoogleTest libraries...'
	clang++ -o ./gtest.o -c -w -O0 $(COMMON) $(INCLUDE_GTEST) -iquote $(TPY)/gtest/googletest $(TPY)/gtest/googletest/src/gtest-all.cc
gmain.o:
	echo 'Compiling GoogleTest main function...'
	clang++ -o ./gmain.o -c -w -O0 $(COMMON) $(INCLUDE_GTEST) -iquote $(TPY)/gtest/googletest $(TPY)/gtest/googletest/src/gtest_main.cc

test_distortion: $(TST)/distortion.cpp $(call deps,vision/distortion) | eigen
	$(compile-tst)
test_field-lines: $(TST)/field-lines.cpp $(call deps,measure/field-lines) units.o xoshiro.o | eigen
	$(compile-tst)
test_image-api: $(TST)/image-api.cpp $(call deps,vision/image-api) distortion.o pxpos.o | eigen
	$(compile-tst)
test_pxpos: $(TST)/pxpos.cpp $(call deps,vision/pxpos)
	$(compile-tst)
test_pyramid: $(TST)/pyramid.cpp $(call deps,wasserstein/pyramid) xoshiro.o image-api.o | eigen
	$(compile-tst)
test_scrambler: $(TST)/scrambler.cpp $(call deps,training/scrambler)
	$(compile-tst)
test_types: $(TST)/types.cpp $(call deps,qbit/types) | vcl
	$(compile-tst)
test_units: $(TST)/units.cpp $(call deps,measure/units) | eigen
	$(compile-tst)
test_xoshiro: $(TST)/xoshiro.cpp $(call deps,rnd/xoshiro)
	$(compile-tst)

check-leak-detection: ../test/leak.cpp
	$(compile) $(strip $(TEST_FLAGS))
ifndef VERBOSE
	! ./check-leak-detection >/dev/null 2>&1
else
	! ./check-leak-detection 2>/dev/null
endif
	rm ./check-leak-detection
	echo '  Detected intentional leak'

srcify = $(subst include,src,$(subst .hpp,.cpp,$(1)))
noextn = $(shell echo $(1) | rev | cut -d/ -f1 | cut -d. -f2- | rev)

test: check-leak-detection gmain.o gtest.o $(ALL_TESTS)
	$(SCT)/test.sh



# TODO: use PGO (profiling-guided optimization)

# TODO: undo src/include split
