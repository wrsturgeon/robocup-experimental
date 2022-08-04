# Simply clears & recreates ./build/, then copies ./build.mk there and calls it.

ifndef VERBOSE
.SILENT: # This silences *every* Makefile here on down
endif

.PHONY: run check
ALL_SRC_INCLUDE := $(shell find ./src ./include)

run: check build/Makefile
	cd ./build && make

release test: check build/Makefile
	cd ./build && make $(@)

format:
	find ./src ./include ./test -type f ! -name README.md | xargs clang-format -i --style='{BasedOnStyle: llvm, PointerAlignment: Left, ColumnLimit: 0}' -Werror

check:
	find ./src ./include ./test -type f ! -name README.md | xargs clang-format -n --style='{BasedOnStyle: llvm, PointerAlignment: Left, ColumnLimit: 0}' -Werror || (echo -e "Please run \`make format\` to fix formatting issues." && exit 1)
	./scripts/check.sh

build/Makefile: build
	echo 'Syncing ./build/Makefile with ./build.mk...'
	rm -f ./build/Makefile
	ln ./build.mk ./build/Makefile

build:
	echo 'Clearing ./build...'
	rm -rf ./build && mkdir ./build
