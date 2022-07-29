# Simply clears & recreates ./build/, then copies ./build.mk there and calls it.

build: ./build.mk
	rm -rf ./build
	mkdir ./build
	ln ./build.mk ./build/Makefile
	cd ./build && make
