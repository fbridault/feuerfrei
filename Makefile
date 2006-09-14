ifneq ($(CFLAGS),)
EXTRA_CMAKE_FLAGS += -DCMAKE_C_FLAGS="$(CFLAGS)"
endif

all: build
	$(MAKE) -C build $(MAKE_FLAGS) all
	mv build/src/rtflames .

build:
	@-mkdir build 2>/dev/null
	@-mkdir build/shadersObjects 2>/dev/null
	cd build && cmake .. $(EXTRA_CMAKE_FLAGS)

cmakepurge:
	rm -rf install_manifest.txt progress.make CMakeFiles CMakeCache.txt cmake_install.cmake 
	rm -rf */install_manifest.txt */progress.make */CMakeFiles */CMakeCache.txt */cmake_install.cmake 
	rm -rf */*/install_manifest.txt */*/progress.make */*/CMakeFiles */*/CMakeCache.txt */*/cmake_install.cmake 
	rm */Makefile */*/Makefile

clean:
	rm -rf build
	rm -f rtflames

%: build
	$(MAKE) -C build $(MAKE_FLAGS) $@

#ifneq ($(PREFIX),)
#install: build/Makefile
#	cd build && cmake .. -DCMAKE_INSTALL_PREFIX="$(PREFIX)" && $(MAKE) $(MAKE_FLAGS) install
#endif
