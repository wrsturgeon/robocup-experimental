# Automatically hard-linked into ./build/ in the main Makefile and called from inside.

.PHONY: eigen vcl highway naoqi-driver naoqi-sdk test check-leak-detection

OS := $(shell if [ $(shell uname -s) = Darwin ]; then echo mac; else echo linux; fi) # fuck Windows 💪🤝🚫🪟
CORES := $(shell if [ $(OS) = linux ]; then nproc --all; else sysctl -n hw.ncpu; fi)
BITS := 32 #$(shell getconf LONG_BIT)

CXX := clang++

DIR := $(shell cd .. && pwd)
SRC := $(DIR)/src
TPY := $(DIR)/third-party
TST := $(DIR)/test
SCT := $(TST)/scripts

FLAGS := -std=gnu++20 -ftemplate-backtrace-limit=0
INCLUDES := -iquote $(SRC) -isystem $(TPY)/eigen -include $(SRC)/options.hpp
MACROS := -DBITS=$(BITS) -DOS=$(strip $(OS)) -DCORES=$(CORES)
WARNINGS := -Weverything -Werror -pedantic-errors -Wno-c++98-compat -Wno-c++98-compat-pedantic
COMMON := $(strip $(FLAGS)) $(strip $(MACROS)) $(strip $(INCLUDES)) $(strip $(WARNINGS))

DEBUG_FLAGS   := -O0 -fno-omit-frame-pointer -g -fno-optimize-sibling-calls -fsanitize=address -fno-common -fsanitize-address-use-after-scope -fsanitize-address-use-after-return=always -DEIGEN_INITIALIZE_MATRICES_BY_NAN
RELEASE_FLAGS := -Ofast -fomit-frame-pointer -march=native -mtune=native -mllvm -polly -mllvm -polly-vectorizer=stripmine -Rpass-analysis=loop-vectorize -DNDEBUG
COVERAGE := -fprofile-instr-generate -fcoverage-mapping
TEST_FLAGS := $(strip $(DEBUG_FLAGS)) $(strip $(COVERAGE)) -isystem $(TPY)/gtest/googletest/include



# Release: no debug symbols, no bullshit, just as fast as possible
release:
	echo 'TODO'

tidy:
	echo 'Tidying the entire project...'
	echo '#pragma clang diagnostic push' > ./all.cpp
	echo '#pragma clang diagnostic ignored "-Wreserved-identifier"' >> ./all.cpp
	echo '#pragma clang diagnostic ignored "-Wunused-function"' >> ./all.cpp
	echo '// NOLINTNEXTLINE(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)' >> ./all.cpp
	echo 'INLINE auto __asan_default_options() -> char const* {' >> ./all.cpp
	echo '  return "allocator_frees_and_returns_null_on_realloc_zero=true"' >> ./all.cpp
	echo '         "detect_stack_use_after_return=true:"' >> ./all.cpp
	echo '         "detect_invalid_pointer_pairs=255:"' >> ./all.cpp
	echo '         "check_initialization_order=true:"' >> ./all.cpp
	echo '         "alloc_dealloc_mismatch=true:"' >> ./all.cpp
	echo '         "detect_odr_violation=255:"' >> ./all.cpp
	echo '         "strict_string_checks=true:"' >> ./all.cpp
	echo '         "strict_init_order=true:"' >> ./all.cpp
	echo '         "use_odr_indicator=true:"' >> ./all.cpp
	echo '         "detect_leaks=1:"' >> ./all.cpp
	echo '         "debug=true";' >> ./all.cpp
	echo '}' >> ./all.cpp
	echo '#pragma clang diagnostic pop' >> ./all.cpp
	echo '// NOLINTBEGIN(bugprone-suspicious-include,llvm-include-order)' >> ./all.cpp
	find $(SRC) -type f -name '*\.*pp' -maxdepth 2      | sed 's/^/#include "/' | sed 's/$$/"/' >> ./all.cpp
	find $(TST) -type f -name '*\.*pp' ! -name leak.cpp | sed 's/^/#include "/' | sed 's/$$/"/' >> ./all.cpp
	echo '// NOLINTEND(bugprone-suspicious-include,llvm-include-order)' >> ./all.cpp
	clang-tidy ./all.cpp --quiet -- $(TEST_CLANG_ARGS)

naoqi-sdk: $(TPY)
	echo '  naoqi-sdk'
	if [ ! -d $(<)/naoqi-sdk ]; then \
		wget -q -O naoqi-sdk.tar.gz https://community-static.aldebaran.com/resources/2.1.4.13/sdk-c%2B%2B/naoqi-sdk-2.1.4.13-$(strip $(OS))$(BITS).tar.gz && \
		tar -xzf naoqi-sdk.tar.gz && \
		rm naoqi-sdk.tar.gz && \
		find . -type d -maxdepth 1 -iname 'naoqi-sdk*' -print -quit | xargs -I{} mv {} $(<)/naoqi-sdk; \
	fi



#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Compilation

compile = $(CXX) -o ./$(@) $(<) $(strip $(COMMON))
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
	echo 'Checking leak detection...'
	$(compile) $(strip $(TEST_FLAGS))
ifdef VERBOSE
	! $(SCT)/run-and-analyze.sh ./check-leak-detection
else
	! $(SCT)/run-and-analyze.sh ./check-leak-detection >/dev/null 2>&1
endif
	rm ./check-leak-detection

test: check-leak-detection gmain.o gtest.o
	$(SCT)/test.sh $(TEST_CLANG_ARGS)

# TODO: use PGO (profiling-guided optimization)
