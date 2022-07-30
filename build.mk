# Automatically hard-linked into ./build/ in the main Makefile and called from inside.

.PHONY: eigen naoqi-driver naoqi-sdk test check-leak-detection

OS := $(shell if [ $(shell uname -s) = Darwin ]; then echo mac; else echo linux; fi) # fuck Windows 💪🤝🚫🪟
CORES := $(shell if [ $(OS) = linux ]; then nproc --all; else sysctl -n hw.ncpu; fi)
BITS := $(shell getconf LONG_BIT)

CXX := clang++ # $(shell if [ $(OS) = linux ]; then echo clang++; else echo /usr/local/opt/llvm/bin/clang++; fi)

DIR := $(shell cd .. && pwd)
SRC := $(DIR)/src
INC := $(DIR)/include
TPY := $(DIR)/third-party
TST := $(DIR)/test

ALL_TESTS := $(foreach dir,$(shell find $(SRC) -type f -mindepth 2 -iname '*.cpp' | rev | cut -d/ -f1 | cut -d. -f2- | rev),test_$(dir))

FLAGS := -std=gnu++20 -flto -ferror-limit=1 -ftemplate-backtrace-limit=0
INCLUDES := -include $(INC)/options.hpp -iquote $(INC)
MACROS := -D_BITS=$(BITS) -D_OS=$(strip $(OS)) -D_CORES=$(CORES)
WARNINGS := -Weverything -Werror -pedantic-errors -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-keyword-macro -Wno-poison-system-directories
COMMON := $(strip $(FLAGS)) $(strip $(INCLUDES)) $(strip $(MACROS)) $(strip $(WARNINGS))

DEBUG_FLAGS   := -O1 -fno-omit-frame-pointer -fno-optimize-sibling-calls -DEIGEN_INITIALIZE_MATRICES_BY_NAN
RELEASE_FLAGS := -Ofast -fomit-frame-pointer -march=native -mtune=native -funit-at-a-time -fno-common -mllvm -polly -mllvm -polly-vectorizer=stripmine -Rpass-analysis=loop-vectorize
SANITIZE := -fsanitize=leak
COVERAGE := -fprofile-instr-generate -fcoverage-mapping
TEST_FLAGS := $(strip $(DEBUG_FLAGS)) $(strip $(SANITIZE)) $(strip $(COVERAGE))

INCLUDE_EIGEN=-iquote $(TPY)/eigen
INCLUDE_GTEST=-iquote $(TPY)/gtest/googletest/include
INCLUDE_NAOQI_DRIVER=-iquote $(TPY)/naoqi-driver
INCLUDE_NAOQI_SDK=-iquote $(TPY)/naoqi-sdk

LSAN_OPTIONS=suppressions=$(DIR)/lsan.supp # Apparently Objective-C has internal memory leaks (lol)



# Release: no debug symbols, no bullshit, just as fast as possible
release: release-flags
	echo 'TODO'



# Dependencies
pull = \
echo "Pulling $(@)..."; \
cd $(<); \
(cd $(@) 2>/dev/null && git pull -q) || \
(echo "  Downloading into $(TPY)/$(@)..." && git clone -q $(1) $(@) && echo '  Done!')

$(TPY):
	mkdir -p $(TPY)
eigen: $(TPY)
	$(call pull,https://gitlab.com/libeigen/eigen.git)
gtest: $(TPY)
	$(call pull,https://github.com/google/googletest.git)
naoqi-driver: $(TPY)
	$(call pull,https://github.com/ros-naoqi/naoqi_driver)
naoqi-sdk: $(TPY)
	echo '  naoqi-sdk'
	if [ ! -d $(<)/naoqi-sdk ]; then \
		wget -q -O naoqi-sdk.tar.gz https://community-static.aldebaran.com/resources/2.1.4.13/sdk-c%2B%2B/naoqi-sdk-2.1.4.13-$(strip $(OS))$(BITS).tar.gz && \
		tar -xzf naoqi-sdk.tar.gz && \
		rm naoqi-sdk.tar.gz && \
		find . -type d -maxdepth 1 -iname 'naoqi-sdk*' -print -quit | xargs -I{} mv {} $(<)/naoqi-sdk; \
	fi



#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Compilation

compile = echo "Compiling $(@)..." && $(CXX) -o ./$(@) $(<) $(COMMON)
compile-bin = $(compile) $(call nth_prereqs,3) $(strip $(RELEASE_FLAGS))
compile-tst = $(compile) $(call nth_prereqs,3) gmain.o gtest.o $(TST)/$(subst test_,,$(@)).cpp $(strip $(TEST_FLAGS)) $(INCLUDE_GTEST)
compile-lib = $(compile-bin) -c $(call nth_prereqs,3)

nth_prereqs = $(subst eigen,$(INCLUDE_EIGEN),$(shell echo $(^) | cut -d' ' -f$(1)-))

deps = $(SRC)/$(1).cpp $(INC)/$(1).hpp



# No dependencies
distortion.o: $(call deps,vision/distortion)
	$(compile-lib)
pxpos.o: $(call deps,vision/pxpos)
	$(compile-lib)
xoshiro.o: $(call deps,rnd/xoshiro)
	$(compile-lib)

# Only third-party libraries
units.o: $(call deps,measure/units) eigen
	$(compile-lib)

# Dependencies, in some dependency-based order
image-api.o: $(call deps,vision/image-api) eigen distortion pxpos
	$(compile-lib)



# Testing
gtest.o: gtest
	echo 'Compiling GoogleTest libraries...'
	$(CXX) -o ./gtest.o -c -w -O1 $(COMMON) $(INCLUDE_GTEST) -iquote $(TPY)/gtest/googletest $(TPY)/gtest/googletest/src/gtest-all.cc
gmain.o: gtest
	echo 'Compiling GoogleTest main function...'
	$(CXX) -o ./gmain.o -c -w -O1 $(COMMON) $(INCLUDE_GTEST) -iquote $(TPY)/gtest/googletest $(TPY)/gtest/googletest/src/gtest_main.cc

test_distortion: $(call deps,vision/distortion) eigen
	$(compile-tst)
test_field-lines: $(call deps,measure/field-lines) eigen units.o xoshiro.o
	$(compile-tst)
test_image-api: $(call deps,vision/image-api) eigen
	$(compile-tst)
test_pxpos: $(call deps,vision/pxpos)
	$(compile-tst)
test_pyramid: $(call deps,wasserstein/pyramid) eigen
	$(compile-tst)
test_scrambler: $(call deps,training/scrambler)
	$(compile-tst)
test_units: $(call deps,measure/units) eigen
	$(compile-tst)
test_xoshiro: $(call deps,rnd/xoshiro)
	$(compile-tst)

../coverage:
	mkdir -p ../coverage && cd ../coverage && rm -rf *

check-leak-detection: ../test/leak.cpp ../coverage
	$(compile) $(strip $(TEST_FLAGS))
ifndef VERBOSE
	! ./check-leak-detection >/dev/null 2>&1
else
	! ./check-leak-detection 2>/dev/null
endif
	rm ./check-leak-detection
	echo '  Detected intentional leak'

verify = \
echo "Testing $(1)..."; \
rm -f ./default.profraw; \
LSAN_OPTIONS=$(strip $(LSAN_OPTIONS)) ./test_$(1); \
llvm-profdata merge ./default.profraw -o ./$(1).profdata; \
llvm-cov report ./test_$(1) --instr-profile=./$(1).profdata | grep -w "src/.*$(1).cpp";

test: check-leak-detection gmain.o gtest.o $(ALL_TESTS)
	$(foreach test,$(ALL_TESTS),$(call verify,$(subst test_,,$(test))))



# TODO: use PGO (profiling-guided optimization)
