# Simply clears & recreates ./build/, then copies ./build.mk there and calls it.

.PHONY: run

run: build
	@cd ./build && make

build: ./build.mk
	rm -rf ./build
	mkdir ./build
	ln ./build.mk ./build/Makefile
