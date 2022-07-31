# Simply clears & recreates ./build/, then copies ./build.mk there and calls it.

ifndef VERBOSE
.SILENT: # This silences *every* Makefile here on down
endif

.PHONY: run check
ALL_SRC_INCLUDE := $(shell find ./src ./include)

OS := $(shell if [ $(shell uname -s) = Darwin ]; then echo mac; else echo linux; fi) # fuck Windows 💪🤝🚫🪟
CORES := $(shell if [ $(OS) = linux ]; then nproc --all; else sysctl -n hw.ncpu; fi)
MAKE := make OS=$(OS) CORES=$(CORES) #-j$(CORES)

run: check build/Makefile
	cd ./build && $(MAKE)

release test: check build/Makefile
	cd ./build && $(MAKE) $(@)

check:
	scripts/check.sh

build/Makefile: build
	echo 'Syncing ./build/Makefile with ./build.mk...'
	rm -f ./build/Makefile
	ln ./build.mk ./build/Makefile

build:
	echo 'Clearing ./build...'
	rm -rf ./build && mkdir ./build
