# Automatically hard-linked into ./build/ in the main Makefile and called from inside.

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
	echo '  naoqi-sdk'
	if [ ! -d ../third-party/naoqi-sdk ]; then \
		wget -q -O naoqi-sdk.tar.gz "https://community-static.aldebaran.com/resources/2.1.4.13/sdk-c%2B%2B/naoqi-sdk-2.1.4.13-$(strip $(OS))$(BITS).tar.gz" && \
		tar -xzf naoqi-sdk.tar.gz && \
		rm naoqi-sdk.tar.gz && \
		find . -type d -maxdepth 1 -iname 'naoqi-sdk*' -print -quit | xargs -I{} mv {} ../third-party/naoqi-sdk; \
	fi
