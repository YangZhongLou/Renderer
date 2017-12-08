file(GLOB src "*.cpp" "../external/imgui/imgui.cpp" "../external/imgui/imgui_draw.cpp")

function(make_library)
	if(WIN32)
		add_library(renderer STATIC ${src})
		target_link_libraries(renderer ${Vulkan_LIBRARY} ${ASSIMP_LIBRARIES} ${WINLIBS})
	else(WIN32)
		#Add more
	endif(WIN32)
endfunction(make_library)
make_library()