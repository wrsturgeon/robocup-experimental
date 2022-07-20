.PHONY: test_compile_all ./SDL/build

# Options
DISPLAY_ON?=0

COMMON_FLAGS:=-D_BITS=$$(getconf LONG_BIT) -D_IMAGE_W=1280 -D_IMAGE_H=960 -D_DISPLAY_ON=${DISPLAY_ON} -I./src -I./eigen -I./naoqi_driver/include $$(sdl2-config --cflags) -march=native -funit-at-a-time -Wall -Wextra -Werror -D_XOPEN_SOURCE=700
CPPFLAGS:=-std=c++20 ${COMMON_FLAGS}
CFLAGS:=-std=c17 ${COMMON_FLAGS}

test_compile_all: ./SDL/build
	find ./src -type f -name '*.*pp' | xargs -t -I{} clang++ -c {} -o ./tmp_compiled ${CPPFLAGS}
	find ./src -type f -name '*.c' | xargs -t -I{} clang -c {} -o ./tmp_compiled ${CFLAGS}
	rm -f ./tmp_compiled
