# Hard-linked into ./build/ in the main Makefile and called from inside
# Calling `make` will remake the hard link whenever this file changes;
# otherwise, it's unsafe to call this directly if build.mk changed

.PHONY: eigen naoqi_driver

OS := $(shell if [ $(shell uname -s) = Darwin ]; then echo mac; else echo linux; fi) # fuck Windows 💪🤝🚫🪟
CORES := $(shell if [ ${OS} = linux ]; then nproc --all; else sysctl -n hw.ncpu; fi)
BITS := $(shell getconf LONG_BIT)

# Release: no debug symbols, no bullshit, just as fast as possible
release: dependencies
	@echo ${OS}${BITS} ${CORES}

# Dependencies
dependencies: eigen naoqi_driver
../third-party:
	mkdir -p ../third-party
../third-party/eigen: | ../third-party
	@cd ../third-party && git clone https://gitlab.com/libeigen/eigen.git
../third-party/naoqi_driver: | ../third-party
	@cd ../third-party && git clone https://github.com/ros-naoqi/naoqi_driver
eigen: ../third-party/eigen
	@echo 'Updating Eigen...'
	@cd ../third-party/eigen && git pull
naoqi_driver: ../third-party/naoqi_driver
	@echo 'Updating NAOqi Driver...'
	@cd ../third-party/naoqi_driver && git pull
