# Automatically hard-linked into ./build/ in the main Makefile and called from inside.

.PHONY: eigen naoqi-driver naoqi-sdk test

CXX := clang++

OS := $(shell if [ $(shell uname -s) = Darwin ]; then echo mac; else echo linux; fi) # fuck Windows 💪🤝🚫🪟
CORES := $(shell if [ $(OS) = linux ]; then nproc --all; else sysctl -n hw.ncpu; fi)
BITS := $(shell getconf LONG_BIT)

DIR := $(shell cd .. && pwd)
SRC := $(DIR)/src
INC := $(DIR)/include
TPY := $(DIR)/third-party
TST := $(DIR)/test

ALL_TESTS := $(foreach dir,$(shell find $(SRC) -type f -mindepth 2 -iname '*.cpp' | rev | cut -d/ -f1 | cut -d. -f2- | rev),test_$(dir))

FLAGS := -std=gnu++20 -flto
INCLUDES := -include $(INC)/options.hpp -iquote $(INC)
MACROS := -D_BITS=$(BITS) -D_OS=$(strip $(OS)) -D_CORES=$(CORES) -imacros $(INC)/macros.hpp
WARNINGS := -Weverything -Werror -pedantic-errors -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-keyword-macro
COMMON := $(strip $(FLAGS)) $(strip $(INCLUDES)) $(strip $(MACROS)) $(strip $(WARNINGS))

# TODO: remove stupid enable/disable macros

DEBUG_FLAGS   := -g -O1 -fno-omit-frame-pointer -fno-optimize-sibling-calls -DEIGEN_INITIALIZE_MATRICES_BY_NAN
RELEASE_FLAGS :=    -Ofast -march=native -mtune=native -funit-at-a-time -fno-common -fomit-frame-pointer -mllvm -polly -mllvm -polly-vectorizer=stripmine -Rpass-analysis=loop-vectorize
TEST_FLAGS := $(strip $(DEBUG_FLAGS)) $(strip $(SANITIZE)) $(strip $(COVERAGE))
SANITIZE := -fsanitize=address,undefined,cfi -fsanitize-stats -fsanitize-address-use-after-scope -fsanitize-memory-track-origins -fsanitize-memory-use-after-dtor -Wno-error=unused-command-line-argument
COVERAGE := -fprofile-instr-generate -fcoverage-mapping

INCLUDE_EIGEN=-iquote $(TPY)/eigen
INCLUDE_NAOQI_DRIVER=-iquote $(TPY)/naoqi-driver
INCLUDE_NAOQI_SDK=-iquote $(TPY)/naoqi-sdk

ASAN_OPTIONS=detect_leaks=1:detect_stack_use_after_return=1:detect_invalid_pointer_pairs=1:strict_string_checks=1:check_initialization_order=1:strict_init_order=1:replace_str=1:replace_intrin=1:alloc_dealloc_mismatch=1:debug=1
LSAN_OPTIONS=suppressions=$(DIR)/lsan.supp # Apparently Objective-C has internal memory leaks (lol)



# Release: no debug symbols, no bullshit, just as fast as possible
release: release-flags
	echo TODO



# Dependencies
pull = \
echo "Pulling $(@)..."; \
cd $(<); \
(cd $(@) && git pull -q) || \
(echo "  Downloading into $(TPY)/$(@)..." && git clone -q $(1) $(@) && echo '  Done!')

$(TPY):
	mkdir -p $(TPY)
eigen: $(TPY)
	$(call pull,https://gitlab.com/libeigen/eigen.git)
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
compile-tst = $(compile) $(call nth_prereqs,4) $(strip $(TEST_FLAGS))
compile-lib = $(compile-bin) -c $(call nth_prereqs,3)

nth_prereqs = $(subst eigen,$(INCLUDE_EIGEN),$(shell cut -d' ' -f$(1)- <<< "$(^)"))

deps = $(SRC)/$(1).cpp $(INC)/$(1).hpp



# No dependencies
distortion: $(call deps,vision/distortion)
	$(compile-lib)
pxpos: $(call deps,vision/pxpos)
	$(compile-lib)
xoshiro: $(call deps,rnd/xoshiro)
	$(compile-lib)

# Only Eigen
units: $(call deps,measure/units) eigen
	$(compile-lib)

# Dependencies, in some dependency-based order
image-api: $(call deps,vision/image-api) eigen distortion pxpos
	$(compile-lib)



# Testing
test_distortion: $(TST)/distortion.cpp $(call deps,vision/distortion) eigen
	$(compile-tst)
test_field-lines: $(TST)/field-lines.cpp $(call deps,measure/field-lines) eigen units
	$(compile-tst)
test_image-api: $(TST)/image-api.cpp $(call deps,vision/image-api) eigen
	$(compile-tst)
test_pxpos: $(TST)/pxpos.cpp $(call deps,vision/pxpos)
	$(compile-tst)
test_pyramid: $(TST)/pyramid.cpp $(call deps,wasserstein/pyramid) eigen
	$(compile-tst)
test_scrambler: $(TST)/scrambler.cpp $(call deps,training/scrambler)
	$(compile-tst)
test_units: $(TST)/units.cpp $(call deps,measure/units) eigen
	$(compile-tst)
test_xoshiro: $(TST)/xoshiro.cpp $(call deps,rnd/xoshiro)
	$(compile-tst)

test: $(ALL_TESTS)
	echo 'TODO: run all tests'
