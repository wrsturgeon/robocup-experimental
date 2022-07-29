# Automatically hard-linked into ./build/ in the main Makefile and called from inside.

.PHONY: eigen naoqi_driver naoqi-sdk wasserstein_pyramid.so

CXX := clang++

OS := $(shell if [ $(shell uname -s) = Darwin ]; then echo mac; else echo linux; fi) # fuck Windows 💪🤝🚫🪟
CORES := $(shell if [ $(OS) = linux ]; then nproc --all; else sysctl -n hw.ncpu; fi)
BITS := $(shell getconf LONG_BIT)

DIR := $(shell cd .. && pwd)
SRC := $(DIR)/src
INC := $(DIR)/include
TPY := $(DIR)/third-party

FLAGS := -std=gnu++20 -flto
INCLUDES := -include $(INC)/options.hpp -iquote $(INC)
MACROS := -D_BITS=$(BITS) -D_OS=$(strip $(OS)) -D_CORES=$(CORES) -imacros $(INC)/macros.hpp
WARNINGS := -Weverything -Werror -pedantic-errors -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-keyword-macro
COMMON := $(strip $(FLAGS)) $(strip $(INCLUDES)) $(strip $(MACROS)) $(strip $(WARNINGS))

# TODO: remove stupid enable/disable macros

DEBUG_FLAGS   := -g -O1 -fno-omit-frame-pointer -fno-optimize-sibling-calls -DEIGEN_INITIALIZE_MATRICES_BY_NAN
RELEASE_FLAGS :=    -Ofast -fomit-frame-pointer
SANITIZE := -fsanitize=address,undefined,cfi -fsanitize-stats -fsanitize-address-use-after-scope -fsanitize-memory-track-origins -fsanitize-memory-use-after-dtor -Wno-error=unused-command-line-argument
COVERAGE := -fprofile-instr-generate -fcoverage-mapping

INCLUDE_EIGEN=-iquote $(TPY)/eigen
INCLUDE_NAOQI_DRIVER=-iquote $(TPY)/naoqi_driver
INCLUDE_NAOQI_SDK=-iquote $(TPY)/naoqi-sdk

ASAN_OPTIONS=detect_leaks=1:detect_stack_use_after_return=1:detect_invalid_pointer_pairs=1:strict_string_checks=1:check_initialization_order=1:strict_init_order=1:replace_str=1:replace_intrin=1:alloc_dealloc_mismatch=1:debug=1
LSAN_OPTIONS=suppressions=$(DIR)/lsan.supp # Apparently Objective-C has internal memory leaks (lol)



# Release: no debug symbols, no bullshit, just as fast as possible
release: wasserstein_pyramid.so

# Dependencies
define pull
echo "Pulling $(@)..."
cd $(<); \
cd $(@) 2>/dev/null && git pull -q || git clone -q $(1)
endef
$(TPY):
	mkdir -p $(TPY)
eigen: $(TPY)
	$(call pull,https://gitlab.com/libeigen/eigen.git)
naoqi_driver: $(TPY)
	$(call pull,https://github.com/ros-naoqi/naoqi_driver)
naoqi-sdk: $(TPY)
	echo '  naoqi-sdk'
	if [ ! -d $(<)/naoqi-sdk ]; then \
		wget -q -O naoqi-sdk.tar.gz https://community-static.aldebaran.com/resources/2.1.4.13/sdk-c%2B%2B/naoqi-sdk-2.1.4.13-$(strip $(OS))$(BITS).tar.gz && \
		tar -xzf naoqi-sdk.tar.gz && \
		rm naoqi-sdk.tar.gz && \
		find . -type d -maxdepth 1 -iname 'naoqi-sdk*' -print -quit | xargs -I{} mv {} $(<)/naoqi-sdk; \
	fi

test-all:
	echo $(INCLUDE)

soname = $(shell echo $(subst /,_,$(1)) | rev | cut -d. -f2- | rev).so
nopath = $(subst $(SRC)/,,$(<))
makelib = $(call _makelib,$(call nopath,$(<)))
_makelib = echo "Compiling $(1)..."; $(call _compilelib,$(call soname,$(1)))
define _compilelib
echo "$(CXX) -c -o ./$(1) $(<) $(COMMON)$()"
$(CXX) -c -o ./$(1) $(<) $(COMMON)$()
endef

wasserstein_pyramid.so: $(SRC)/wasserstein/pyramid.cpp eigen #rnd_xoshiro.so vision_image-api.so
	$(makelib) $(INCLUDE_EIGEN)
