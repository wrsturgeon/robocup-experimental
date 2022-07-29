# Simply clears & recreates ./build/, then copies ./build.mk there and calls it.

ifndef VERBOSE
.SILENT:
endif

.PHONY: run check
ALL_SRC_INCLUDE := $(shell find ./src ./include)

run: check build/Makefile
	cd ./build && make

check:
	scripts/code-checker.sh

build/Makefile: build
	echo 'Syncing ./build/Makefile with ./build.mk...'
	rm -f ./build/Makefile
	ln ./build.mk ./build/Makefile

build:
	echo 'Clearing ./build...'
	rm -rf ./build && mkdir ./build
