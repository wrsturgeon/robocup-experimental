.PHONY: test_compile_all

COMMON_FLAGS=-D_BITS=$$(getconf LONG_BIT) -D_IMAGE_W=1920 -D_IMAGE_H=1080 -I./src -I./eigen -I./naoqi_driver/include -march=native -funit-at-a-time -Wall -Wextra -Werror -D_XOPEN_SOURCE=700
CPPFLAGS=-std=c++20 ${COMMON_FLAGS}
CFLAGS=-std=c17 ${COMMON_FLAGS}

test_compile_all:
	find ./src -type f -name '*.*pp' | xargs -I{} clang++ -c {} -o ./tmp_compiled ${CPPFLAGS}
	find ./src -type f -name '*.c' | xargs -I{} clang -c {} -o ./tmp_compiled ${CFLAGS}
	rm -f ./tmp_compiled

units: ./src/measurement/units.hpp
	clang++ ./src/measurement/units.hpp -o ./units ${CPPFLAGS}

shit: ./shit.cpp
	clang++ ./shit.cpp -o ./shit ${CPPFLAGS}
