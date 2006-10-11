ifneq ($(CFLAGS),)
EXTRA_CMAKE_FLAGS += -DCMAKE_C_FLAGS="$(CFLAGS)"
endif

all: build
	$(MAKE) -C build $(MAKE_FLAGS)
	mv build/src/rtflames build/src/rtfluids .

flames: build
	$(MAKE) -C build $(MAKE_FLAGS) rtflames
	mv build/src/rtflames . 

fluids: build 
	$(MAKE) -C build $(MAKE_FLAGS) rtfluids
	mv build/src/rtfluids .
build:
	@-mkdir build 2>/dev/null
	@-mkdir build/shadersObjects 2>/dev/null
	@-mkdir captures 2>/dev/null
	cd build && cmake .. $(EXTRA_CMAKE_FLAGS)

cmakepurge:
	rm -rf install_manifest.txt progress.make CMakeFiles CMakeCache.txt cmake_install.cmake 
	rm -rf */install_manifest.txt */progress.make */CMakeFiles */CMakeCache.txt */cmake_install.cmake 
	rm -rf */*/install_manifest.txt */*/progress.make */*/CMakeFiles */*/CMakeCache.txt */*/cmake_install.cmake 
	rm */Makefile */*/Makefile

clean:
	rm -rf build
	rm -f rtflames rtfluids

#%: build
#	$(MAKE) -C build $(MAKE_FLAGS) $@

#ifneq ($(PREFIX),)
#install: build/Makefile
#	cd build && cmake .. -DCMAKE_INSTALL_PREFIX="$(PREFIX)" && $(MAKE) $(MAKE_FLAGS) install
#endif
