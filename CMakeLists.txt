cmake_minimum_required(VERSION 3.12)
project(VulkanTutorial)

set(CMAKE_CXX_STANDARD 17)

set(COMPILE_FLAGS
		PRIVATE -pipe
		PRIVATE -march=native
		PRIVATE -Wall
		PRIVATE -Wextra)

set(LINKER_FLAGS
		-Wl,--as-needed
		-lstdc++
		-lm)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

if (${GLFW_FOUND})
	include_directories(${GLFW_INCLUDE_DIRS})
	set(LINKER_FLAGS ${LINKER_FLAGS} ${GLFW_LIBRARIES})
endif ()

find_package(Vulkan REQUIRED)

if (${Vulkan_FOUND})
	include_directories(${Vulkan_INCLUDE_DIRS})
	set(LINKER_FLAGS ${LINKER_FLAGS} ${Vulkan_LIBRARIES})
endif ()


add_executable(VulkanTutorial main.cpp hello_triangle_app.cpp hello_triangle_app.h)
target_compile_options(VulkanTutorial ${COMPILE_FLAGS})
target_link_libraries(VulkanTutorial ${LINKER_FLAGS})