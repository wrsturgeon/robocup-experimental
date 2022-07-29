# Hard-linked into ./build/ in the main Makefile and called from inside
# Calling `make` will remake the hard link whenever this file changes;
# otherwise, it's unsafe to call this directly if build.mk changed

ifndef VERBOSE
.SILENT:
endif

.PHONY: eigen naoqi_driver

OS := $(shell if [ $(shell uname -s) = Darwin ]; then echo mac; else echo linux; fi) # fuck Windows 💪🤝🚫🪟
CORES := $(shell if [ ${OS} = linux ]; then nproc --all; else sysctl -n hw.ncpu; fi)
BITS := $(shell getconf LONG_BIT)

# Release: no debug symbols, no bullshit, just as fast as possible
release: dependencies

test: dependencies

debug: dependencies

# Dependencies
pull = echo "  $(1)"; cd ../third-party; cd $(1) 2>/dev/null && git pull -q || git clone -q $(2)
dependencies:
	mkdir -p ../third-party
	echo 'Pulling dependencies...'
	$(call pull,eigen,https://gitlab.com/libeigen/eigen.git)
	$(call pull,naoqi_driver,https://github.com/ros-naoqi/naoqi_driver)
