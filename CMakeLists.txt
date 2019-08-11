cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(VulkanTutorial)

set(CMAKE_CXX_STANDARD 17)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(COMPILE_FLAGS
		-pipe
		-march=native
		-Wall
		-Wextra
		-pedantic)

	set(LINKER_OPTIONS
		-Wl,--sort-common,--as-needed)
	set(LINKER_FLAGS
		stdc++
		m)

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
	set(COMPILE_FLAGS
		-pipe
		-march=native
		-m64
		-xHost
		-Wall
		-Wextra
		-pedantic)

	set(LINKER_OPTIONS
		-Wl,--sort-common,--as-needed)

	set(LINKER_FLAGS
		m)
endif ()

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)

include_directories(${Vulkan_INCLUDE_DIR} ${GLM_INCLUDE_DIRS})


add_executable(VulkanTutorial main.cpp hello_triangle_app.cpp hello_triangle_app.h)
target_compile_options(VulkanTutorial PRIVATE ${COMPILE_FLAGS})
target_link_options(VulkanTutorial PRIVATE ${LINKER_OPTIONS})
target_link_libraries(VulkanTutorial ${LINKER_FLAGS} ${GLFW_LIBRARIES} Vulkan::Vulkan)