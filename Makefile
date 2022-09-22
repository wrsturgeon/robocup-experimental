# Simply clears & recreates ./build/, then copies ./build.mk there and calls it.

ifndef VERBOSE
.SILENT: # This silences *every* Makefile here on down
endif

.PHONY: run check
FORMAT := find ./src ./test -type f -iname '*.*pp' | xargs clang-format --style=file

release test tidy: format check submodules build/Makefile
	cd ./build && make $(@)

format:
	$(FORMAT) -i

check:
	$(FORMAT) -n -Werror || (echo -e "Please run \`make format\` to fix formatting issues." && exit 1)
	./test/scripts/check.sh

submodules:
	git submodule update --init --recursive

build/Makefile: build
	echo 'Syncing ./build/Makefile with ./build.mk...'
	rm -f ./build/Makefile
	ln ./build.mk ./build/Makefile

build:
	echo 'Clearing ./build...'
	rm -rf ./build && mkdir ./build
