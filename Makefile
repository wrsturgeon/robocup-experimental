# Simply clears & recreates ./build/, then copies ./build.mk there and calls it.

ifndef VERBOSE
.SILENT: # This silences *every* Makefile here on down
endif

.PHONY: check
FORMAT := find ./cpp -type f -iname '*.*pp' | xargs clang-format --style=file

release tidy debug profile-compilation: | check submodules build/Makefile
	cd ./build && make $(@)

ci: check #debug profile-compilation release tidy

format:
	echo 'Formatting code...'
	$(FORMAT) -i

check:
	$(FORMAT) -n -Werror || (echo -e "Please run \`make format\` to fix formatting issues." && $(FORMAT) -n -Werror && exit 1)
	./sh/check

submodules:
	echo 'Initializing submodules...'
	mkdir -p ./ext
	git submodule update --init --recursive
# Add a new submodule (e.g. foo) with `git submodule add -f --name foo https://... ext/foo`

build/Makefile: build
	rm -f ./build/Makefile
	ln ./build.mk ./build/Makefile

build:
	rm -rf ./build && mkdir ./build
