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

if (${UNIX})
	find_package(PkgConfig REQUIRED)
	pkg_search_module(GLFW REQUIRED glfw3)

	if (${GLFW_FOUND})
		set(LINKER_FLAGS ${LINKER_FLAGS} ${GLFW_LIBRARIES})
	else ()
		message(AUTHOR_WARNING "GLFW not found")
	endif ()

elseif (${WIN32})
	include_directories(C:/VulkanSDK/1.1.92.1/Include)
	include_directories(./glfw-3.2.1.bin.WIN64/include)
	set(LINKER_FLAGS ${LINKER_FLAGS} ./glfw-3.2.1.bin.WIN64/lib-mingw-w64/glfw3.dll)
endif ()

find_package(Vulkan REQUIRED)

if (${Vulkan_FOUND})
	include_directories(${Vulkan_INCLUDE_DIRS})
	set(LINKER_FLAGS ${LINKER_FLAGS} ${Vulkan_LIBRARIES})
else ()
	message(AUTHOR_WARNING "Vulkan not found")
endif ()


add_executable(VulkanTutorial main.cpp hello_triangle_app.cpp hello_triangle_app.h)
target_compile_options(VulkanTutorial ${COMPILE_FLAGS})
target_link_libraries(VulkanTutorial ${LINKER_FLAGS})