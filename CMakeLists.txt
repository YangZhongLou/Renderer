
function(init_cmake)
	cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
	cmake_policy(VERSION 2.8)
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
	
	# Compiler specific stuff
	IF(MSVC)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
	ENDIF(MSVC)
	
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
endfunction(init_cmake)
init_cmake()

set(NAME Renderer)

project($(NAME))

include_directories(external)
include_directories(external/glm)
include_directories(external/gli)
include_directories(external/assimp)
include_directories(external/imgui)

#Currently only support win32
function(find_vk_library)
	IF(WIN32)
		IF (NOT Vulkan_FOUND)
			find_library(Vulkan_LIBRARY NAMES vulkan-1 vulkan PATHS ${CMAKE_SOURCE_DIR}/libs/vulkan)
			IF (Vulkan_LIBRARY)
				set(Vulkan_FOUND ON)
				MESSAGE("Using bundled Vulkan library version")
			ENDIF()
		ENDIF()
		find_library(ASSIMP_LIBRARIES NAMES assimp libassimp.dll.a PATHS ${CMAKE_SOURCE_DIR}/libs/assimp)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR")
	ELSE(WIN32)
		#Add more
	ENDIF(WIN32)
endfunction(find_vk_library)
find_vk_library()

add_subdirectory(src)
add_subdirectory(tests)