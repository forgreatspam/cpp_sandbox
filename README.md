# How to build
The project uses Meson build system. Check this site: http://mesonbuild.com.

Instructions for lazy:

1. Get a recent boost library
2. Open meson.build and provide path to boost in include_directories.
3. Get ninja executables. Use your package manager or download it from from https://github.com/ninja-build/ninja/releases. If you work in windows, add them to the PATH environment variable.
4. Install python3. If you use Windows, ensure that path to Python and Python\Scripts folders are in PATH environment variable.
5. Install meson by calling ```pip3 install meson```.
6. Create a folder named 'build'.

The next steps depend on the compiler:

7. gcc 6.0 for Linux. Run ```meson build --buildtype release```
8. clang 3.8 for Linux. Run ```CC=/usr/bin/clang-3.8 CXX=/usr/bin/clang++-3.8 meson build --buildtype release```
9. MSVC 2015 update 2. Open VS command prompt and run ```python3 full\path\to\meson build --buildtype release``` Notice, you might need to provide full path to 'meson' script which is normally located in python/scripts folder.

10. Go to 'build' folder and run ```ninja``` here.