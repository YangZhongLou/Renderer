
#Build a single test
function(build_test test_name)
	SET(current_dir ${CMAKE_CURRENT_SOURCE_DIR}/${test_name})
	message(STATUS "Generating project file for example in ${current_dir}")
	
	# Main
	file(GLOB source *.cpp ${BASE_HEADERS} ${current_dir}/*.cpp)
	SET(main_cpp ${current_dir}/${test_name}.cpp)
	
	# Add shaders
	set(shader_dir "../data/shaders/${test_name}")
	
	file(GLOB shaders "${shader_dir}/*.vert" "${shader_dir}/*.frag" "${shader_dir}/*.comp" "${shader_dir}/*.geom" "${shader_dir}/*.tesc" "${shader_dir}/*.tese")
	source_group("Shaders" FILES ${shaders})
	
	#link
	if(WIN32)
		add_executable(${test_name} WIN32 ${main_cpp} ${source} ${shaders})
		target_link_libraries(${test_name} renderer ${Vulkan_LIBRARY} ${ASSIMP_LIBRARIES} ${WINLIBS})
	else(WIN32)
		add_executable(${test_name} ${main_cpp} ${source} ${shaders})
		target_link_libraries(${test_name} )
	endif(WIN32)
endfunction(build_test)

set(tests
	triangle)

function(build_tests)
	foreach(test_name ${tests})
		build_test(${test_name})
	endforeach(test_name)
endfunction(build_tests)

build_tests()
