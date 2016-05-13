# What is it?
This is a test bench where I evaluate some cpp-related features:
* unit tests with coverage report and integration to IDE
* multithreading: lock-free, memory models, false cache lines, thread pools sharing etc.
* metaprogramming using Boost MPL, Boost Fusion, Hana and just ordinary templates
* c++ 11, 14 features
* profiling
* static code analysis
* cross-platform, cross-compiler code and related build tools. Well, only clang for linux left after I start using c++ 14 extensively. It still compiles in windows if disable using of async methods.

As for the functionality, it is really not that important what this code does. The idea was to find an estimate of a linear equation solution using non optimized Neuman-Ulam scheme with the use of various pseudo and quasi random numbers. The idea was to write the code in meta-programming techniques.

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