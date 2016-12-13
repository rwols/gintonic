set(Boost_USE_STATIC_LIBS OFF)
find_package(Boost COMPONENTS unit_test_framework REQUIRED)

function(gintonic_add_test test_name)
	set(options "")
	set(oneValueArgs "")
	set(multiValueArgs SOURCES ASSETS)
	cmake_parse_arguments(gintonic_add_test
		"${options}"
		"${oneValueArgs}"
		"${multiValueArgs}"
		${ARGN})
	if (NOT gintonic_add_test_SOURCES)
		message(FATAL_ERROR "SOURCES argument required for test case ${test_name}")
	endif (NOT gintonic_add_test_SOURCES)
	add_executable(${test_name} ${gintonic_add_test_SOURCES})
	set_target_properties(${test_name} PROPERTIES CXX_STANDARD 14)
	target_compile_definitions(${test_name} PUBLIC BOOST_TEST_DYN_LINK)
	target_include_directories(${test_name} PUBLIC ${Boost_INCLUDE_DIR})
	target_link_libraries(${test_name} PUBLIC gintonic ${Boost_LIBRARIES})
	if (gintonic_add_test_ASSETS)
		gintonic_target_depends_on_assets(
			TARGET ${test_name} 
			SOURCE_ASSET_DIRECTORY ${GINTONIC_ASSET_DIR} 
			ASSETS ${gintonic_add_test_ASSETS})
	endif (gintonic_add_test_ASSETS)
	add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

gintonic_add_test(Entity SOURCES Entity.cpp)
gintonic_add_test(OctreeTest SOURCES OctreeTest.cpp)
gintonic_add_test(Reflection SOURCES Reflection.cpp)
gintonic_add_test(SQT SOURCES SQT.cpp)

gintonic_add_test(SerializationOfLights 
	SOURCES SerializationOfLights.cpp)

gintonic_add_test(SimdTest SOURCES SimdTest.cpp)
gintonic_add_test(box2f SOURCES box2f.cpp)
gintonic_add_test(box3f SOURCES box3f.cpp)
gintonic_add_test(mat2f SOURCES mat2f.cpp)
gintonic_add_test(mat3f SOURCES mat3f.cpp)
gintonic_add_test(mat4f SOURCES mat4f.cpp)

gintonic_add_test(materials 
	SOURCES materials.cpp 
	ASSETS Inconsolata-Regular.ttf bricks.jpg bricks_SPEC.png bricks_NRM.png)

gintonic_add_test(quatf SOURCES quatf.cpp)
gintonic_add_test(vec2f SOURCES vec2f.cpp)
gintonic_add_test(vec3f SOURCES vec3f.cpp)
gintonic_add_test(vec4f SOURCES vec4f.cpp)
