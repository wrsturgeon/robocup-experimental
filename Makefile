COMMON_FLAGS=-I. -I./eigen -I./naoqi_driver/include -std=c++20 -march=native -funit-at-a-time -Wall -Wextra -Werror

localize: units
	clang++ ./src/localization/localize.cpp -o ./localize ${COMMON_FLAGS}

units: ./src/measurement/units.hpp
	clang++ ./src/measurement/units.hpp -o ./units ${COMMON_FLAGS}
