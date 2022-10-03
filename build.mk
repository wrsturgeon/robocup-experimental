# Automatically hard-linked into ./build/ in the main Makefile and called from inside.

.PHONY: eigen check-leak-detection

OS := $(shell if [ $(shell uname -s) = Darwin ]; then echo mac; else echo linux; fi) # fuck Windows 💪🤝🚫🪟
CORES := $(shell if [ $(OS) = linux ]; then nproc --all; else sysctl -n hw.ncpu; fi)
BITS := 32 #$(shell getconf LONG_BIT)

CXX := clang++

DIR := $(shell cd .. && pwd)
SRC := $(DIR)/cpp
EXT := $(DIR)/ext
SCT := $(DIR)/sh

FLAGS := -std=gnu++20 -ftemplate-backtrace-limit=0
INCLUDES := -iquote $(SRC) -isystem $(EXT)/eigen -include $(SRC)/options.hpp
MACROS := -DBITS=$(BITS) -DOS=$(strip $(OS)) -DCORES=$(CORES)
WARNINGS := -Weverything -Werror -pedantic-errors -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-unused-function
COMMON := $(strip $(FLAGS)) $(strip $(MACROS)) $(strip $(INCLUDES)) $(strip $(WARNINGS))

DEBUG_FLAGS   := -O3 -fno-omit-frame-pointer -g -fno-optimize-sibling-calls -fsanitize=address -fno-common -fsanitize-address-use-after-scope -fsanitize-address-use-after-return=always -DEIGEN_INITIALIZE_MATRICES_BY_NAN
RELEASE_FLAGS := -Ofast -fomit-frame-pointer -march=native -mtune=native -mllvm -polly -mllvm -polly-vectorizer=stripmine -Rpass-analysis=loop-vectorize -DNDEBUG



# Release: no debug symbols, no bullshit, just as fast as possible
release:
	echo 'TODO'

tidy:
	echo 'Running clang-tidy on the project (takes a minute)...'
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
	find $(SRC) -type f -name '*\.*pp' -maxdepth 2 ! -name 'leak.cpp' | sed 's/^/#include "/' | sed 's/$$/"/' >> ./all.cpp
	echo '// NOLINTEND(bugprone-suspicious-include,llvm-include-order)' >> ./all.cpp
	clang-tidy ./all.cpp --quiet -- $(strip $(COMMON)) $(strip $(DEBUG_FLAGS))

naoqi-sdk: $(EXT)
	echo '  naoqi-sdk'
	if [ ! -d $(<)/naoqi-sdk ]; then \
		wget -q -O naoqi-sdk.tar.gz https://community-static.aldebaran.com/resources/2.1.4.13/sdk-c%2B%2B/naoqi-sdk-2.1.4.13-$(strip $(OS))$(BITS).tar.gz && \
		tar -xzf naoqi-sdk.tar.gz && \
		rm naoqi-sdk.tar.gz && \
		find . -type d -maxdepth 1 -iname 'naoqi-sdk*' -print -quit | xargs -I{} mv {} $(<)/naoqi-sdk; \
	fi



#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Compilation

compile = $(CXX) -o ./$(@) $(<) $(strip $(COMMON))

deps = $(SRC)/$(1).hpp



check-leak-detection: ../cpp/leak.cpp
	echo 'Checking leak detection...'
	$(compile) $(strip $(DEBUG_FLAGS))
ifdef VERBOSE
	! sh -x $(SCT)/run-and-analyze ./check-leak-detection
else
	! $(SCT)/run-and-analyze ./check-leak-detection >/dev/null 2>&1
endif
	rm ./check-leak-detection

debug: ../cpp/main.cpp
	echo 'Running...'
	$(compile) $(strip $(DEBUG_FLAGS))
ifdef VERBOSE
	sh -x $(SCT)/run-and-analyze ./debug
else
	$(SCT)/run-and-analyze ./debug
endif
	rm ./debug

# TODO: use PGO (profiling-guided optimization)
