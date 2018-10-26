# lava2d
Vulkan-based 2D graphics and app framework.
Still very very early in development.

### Build requirements
* 64bit Windows machine
* Git
* Vulkan 1.1.70 (or later, although not tested)
* CMake 3.0.0 (or newer)
* A 64bit C++ compiler (MSVC tested)

### Install and build
1. Clone the repository
2. Get the dependencies (added as submodules to the repo):  `git submodule update --init --recursive`
3. Generate project files with CMake (again, only tested for MSVC)
4. Build using the generated project files
