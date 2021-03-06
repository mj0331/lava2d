cmake_minimum_required(VERSION 3.0.0)
project(lava2d)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})

set(BUILD_VEZ_AND_NOTHING_ELSE_BECAUSE_CMAKE_SUCKS n)

find_package(Vulkan REQUIRED)

add_subdirectory(./dependencies/V-EZ)
add_subdirectory(./dependencies/spdlog)

include_directories(
    ${Vulkan_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/dependencies/V-EZ/Libs/Assimp/include
    ${PROJECT_SOURCE_DIR}/dependencies/V-EZ/Libs/glew/include
    ${PROJECT_SOURCE_DIR}/dependencies/V-EZ/Libs/glfw/include
    ${PROJECT_SOURCE_DIR}/dependencies/V-EZ/Libs/glm/include
    ${PROJECT_SOURCE_DIR}/dependencies/V-EZ/Libs/stb/include
    ${PROJECT_SOURCE_DIR}/dependencies/V-EZ/Libs/VulkanMemoryAllocator/include
    ${PROJECT_SOURCE_DIR}/dependencies/V-EZ/Source
    ${PROJECT_SOURCE_DIR}/dependencies/spdlog/include
    ${PROJECT_SOURCE_DIR}/include
)

if(BUILD_VEZ_AND_NOTHING_ELSE_BECAUSE_CMAKE_SUCKS MATCHES y)

file(GLOB LAVA2D_DUMMY "./dummy/*.cpp")
add_executable(lava2d_dummy  ${LAVA2D_DUMMY})

else() # not BUILD_VEZ_AND_NOTHING_ELSE_BECAUSE_CMAKE_SUCKS

file(GLOB_RECURSE LAVA2D_SOURCES "./src/*.cpp")
file(GLOB LAVA2D_TEST "./test/src/*.cpp")
add_library(lava2d STATIC ${LAVA2D_SOURCES})
add_executable(lava2d_test  ${LAVA2D_TEST})
add_dependencies(lava2d_test lava2d)

#Find V-EZ
if(CMAKE_BUILD_TYPE MATCHES Debug)
    find_library(VEZ_LIB NAMES VEZd PATHS ${PROJECT_SOURCE_DIR}/dependencies/V-EZ/Bin/x86_64 NO_DEFAULT_PATH)
else()
    find_library(VEZ_LIB NAMES VEZ PATHS ${PROJECT_SOURCE_DIR}/dependencies/V-EZ/Bin/x86_64 NO_DEFAULT_PATH)
endif(CMAKE_BUILD_TYPE MATCHES Debug)

#Find AssImp
if(UNIX)
    find_library(ASSIMP_LIB NAMES assimp PATHS ${PROJECT_SOURCE_DIR}/dependencies/V-EZ/Libs/Assimp/lib/linux64 NO_DEFAULT_PATH)
elseif(WIN32)
    find_library(ASSIMP_LIB NAMES assimp PATHS ${PROJECT_SOURCE_DIR}/dependencies/V-EZ/Libs/Assimp/lib/win64 NO_DEFAULT_PATH)
endif(UNIX)

#Find GLEW
find_library(GLEW_LIB NAMES glew32s PATHS ${PROJECT_SOURCE_DIR}/dependencies/V-EZ/Libs/glew/lib NO_DEFAULT_PATH)

#Find GLFW3
if(UNIX)
    find_library(GLFW_LIB NAMES glfw PATHS ${PROJECT_SOURCE_DIR}/dependencies/V-EZ/Libs/glfw/lib/linux64 NO_DEFAULT_PATH)
elseif(WIN32)
    find_library(GLFW_LIB NAMES glfw PATHS ${PROJECT_SOURCE_DIR}/dependencies/V-EZ/Libs/glfw/lib/win64 NO_DEFAULT_PATH)
endif(UNIX)

target_link_libraries( lava2d
    ${ASSIMP_LIB}
    ${GLEW_LIB}
    ${GLFW_LIB}
    ${VEZ_LIB}
    ${Vulkan_LIBRARIES}
)

target_link_libraries( lava2d_test
    ${ASSIMP_LIB}
    ${GLEW_LIB}
    ${GLFW_LIB}
    ${VEZ_LIB}
    ${Vulkan_LIBRARIES}
    lava2d
)

endif(BUILD_VEZ_AND_NOTHING_ELSE_BECAUSE_CMAKE_SUCKS MATCHES y)

