.PHONY: test_compile_all

COMMON_FLAGS=-D_BITS=$$(getconf LONG_BIT) -I./src -I./eigen -I./naoqi_driver/include -std=c++20 -march=native -funit-at-a-time -Wall -Wextra -Werror

test_compile_all:
	find ./src -type f -name '*.*pp' | xargs -I{} clang++ {} -o ./tmp_compiled ${COMMON_FLAGS}
	find ./src -type f -name '*.c' | xargs -I{} clang {} -o ./tmp_compiled ${COMMON_FLAGS}
	rm -f ./tmp_compiled

units: ./src/measurement/units.hpp
	clang++ ./src/measurement/units.hpp -o ./units ${COMMON_FLAGS}
