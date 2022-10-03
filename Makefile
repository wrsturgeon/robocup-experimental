# Simply clears & recreates ./build/, then copies ./build.mk there and calls it.

ifndef VERBOSE
.SILENT: # This silences *every* Makefile here on down
endif

.PHONY: run check
FORMAT := find ./src ./test -type f -iname '*.*pp' | xargs clang-format --style=file

release test tidy: check submodules build/Makefile
	cd ./build && make $(@)

format:
	echo 'Formatting code...'
	$(FORMAT) -i

check:
	$(FORMAT) -n -Werror || (echo -e "Please run \`make format\` to fix formatting issues." && exit 1)
	./test/scripts/check.sh

submodules:
	git submodule update --init --recursive
# Add a new submodule (e.g. foo) with `git submodule add -f --name foo https://... third-party/foo`

build/Makefile: build
	echo 'Syncing ./build/Makefile with ./build.mk...'
	rm -f ./build/Makefile
	ln ./build.mk ./build/Makefile

build:
	echo 'Clearing ./build...'
	rm -rf ./build && mkdir ./build
