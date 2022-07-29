# Simply clears & recreates ./build/, then copies ./build.mk there and calls it.

.PHONY: run

run: build/Makefile
	@cd ./build && make

build/Makefile: build
	@echo 'Syncing ./build/Makefile with ./build.mk...'
	@rm -f ./build/Makefile
	@ln ./build.mk ./build/Makefile

build:
	@echo 'Clearing ./build...'
	@rm -rf ./build && mkdir ./build
