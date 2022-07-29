# Do *not* call manually; hard-linked into ./build/ in the main Makefile
# Calling `make` will remake the hard link whenever this file changes;
# otherwise, it's unsafe to call this directly if build.mk changed.

OS := $(shell if [ $(shell uname -s) = Darwin ]; then echo mac; else echo linux; fi) # fuck Windows 💪🤝🚫🪟
BITS := $(shell if [ ${OS} = linux ]; then nproc --all; else sysctl -n hw.ncpu; fi)
