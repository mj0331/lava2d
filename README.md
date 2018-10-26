# lava2d
Vulkan-based 2D graphics and app framework.
Still very very early in development.

### Build requirements
* 64bit Windows machine
* Git
* Vulkan 1.1.70 (or later, although not tested)
* CMake 3.0.0 (or newer)
* A 64bit C++14 compliant compiler (MSVC tested)

### Install and build
1. Clone the repository
2. Get the dependencies (added as submodules to the repo):  `git submodule update --init --recursive`
3. Generate project files with CMake (again, only tested for MSVC)
4. Build using the generated project files
  * On the first build, you might have to go into the CMakeLists.txt file and set `BUILD_VEZ_AND_NOTHING_ELSE_BECAUSE_CMAKE_SUCKS` to `y` because some issues with build order happened in the past.
  * On subsequent builds, turn it back on to `n` and all should work.
