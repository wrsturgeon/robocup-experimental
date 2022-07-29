# Do *not* call manually; hard-linked into ./build/ in the main Makefile
# Calling `make` will remake the hard link whenever this file changes;
# otherwise, it's unsafe to call this directly if build.mk changed.

.PHONY: eigen naoqi_driver

OS := $(shell if [ $(shell uname -s) = Darwin ]; then echo mac; else echo linux; fi) # fuck Windows 💪🤝🚫🪟
BITS := $(shell if [ ${OS} = linux ]; then nproc --all; else sysctl -n hw.ncpu; fi)

# Dependencies
dependencies: eigen naoqi_driver
../third-party:
	mkdir -p ../third-party
../third-party/eigen: | ../third-party
	cd ../third-party && git clone https://gitlab.com/libeigen/eigen.git
../third-party/naoqi_driver: | ../third-party
	cd ../third-party && git clone https://github.com/ros-naoqi/naoqi_driver
eigen: ../third-party/eigen
	cd ../third-party/eigen && git pull
naoqi_driver: ../third-party/naoqi_driver
	cd ../third-party/naoqi_driver && git pull
