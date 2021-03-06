cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(VulkanTutorial CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(COMPILE_FLAGS
	-pipe
	-march=native
	-Wall
	-Wextra
	-pedantic
	-funroll-loops
	-flto
	-fuse-ld=gold
	-fuse-linker-plugin)

set(LINKER_OPTIONS
	-Wl,--sort-common,--as-needed
	-flto
	-fuse-ld=gold
	-fuse-linker-plugin)
set(LINKER_FLAGS
	stdc++
	m)

find_package(OpenMP REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

find_package(Vulkan REQUIRED)
#find_package(GLM REQUIRED)

include_directories(${Vulkan_INCLUDE_DIRS} #[[${GLM_INCLUDE_DIRS}]])


add_executable(VulkanTutorial main.cpp hello_triangle_app.cpp hello_triangle_app.h)
target_precompile_headers(VulkanTutorial PRIVATE hello_triangle_app.h)
target_compile_options(VulkanTutorial PRIVATE ${COMPILE_FLAGS})
target_link_options(VulkanTutorial PRIVATE ${LINKER_OPTIONS})
target_link_libraries(VulkanTutorial ${LINKER_FLAGS} ${CMAKE_DL_LIBS} ${GLFW_LIBRARIES} Vulkan::Vulkan OpenMP::OpenMP_CXX)