# How to build
The project uses Meson build system. Check this site: http://mesonbuild.com.

Instructions for lazy:

1. Get a recent boost library
2. Open meson.build and provide path to boost in include_directories.
3. Get ninja executables. Use your package manager or download it from from https://github.com/ninja-build/ninja/releases. If you work in windows, add them to the PATH environment variable.
4. Install python3. If you use Windows, ensure that path to Python and Python\Scripts folders are in PATH environment variable.
5. Install meson by calling ```pip3 install meson```.
6. Create a folder named 'build'.
7. run ```python3 meson build --buildtype release```. Notice, you might need to provide full path to 'meson' script which is normally located in python/scripts folder. In case of MSVC, you should run this command from VS command prompt.
8. go to 'build' folder and run ```ninja``` here.