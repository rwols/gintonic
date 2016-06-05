cmake_minimum_required (VERSION 2.8)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "Prevented in-source build. \
  	Please create a build directory outside of the \
  	Gintonic source code and call CMake from there.")
endif()

set(CMAKE_BUILD_TYPE Debug CACHE STRING "The build type. Can be Debug or Release or whatnot")

set(SSE_VERSION 30 CACHE STRING "The SSE version.")

set(shader_folder "${CMAKE_CURRENT_SOURCE_DIR}/Source/Graphics/Shaders/")

set(lua_src_dir "lua-5.3.1/src")
set(source_directory "${CMAKE_CURRENT_SOURCE_DIR}/Source")
file(GLOB_RECURSE gintonic_source_files "${source_directory}/*.cpp")
file(GLOB lua_source_files "${source_directory}/${lua_src_dir}/*.c")
list(REMOVE_ITEM lua_source_files "${source_directory}/${lua_src_dir}/luac.c")
file(GLOB shader_source_files "${source_directory}/Graphics/Shaders/*.vert" "${source_directory}/Graphics/Shaders/*.frag")
set(src_files ${gintonic_source_files} ${lua_source_files})
set_source_files_properties(${lua_source_files} PROPERTIES LANGUAGE CXX)

project(gintonic C CXX)

set(CMAKE_MACOSX_RPATH ON)

# use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH OFF)

# when building, don't use the install RPATH already
# (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH OFF) 

# the RPATH to be used when installing
set(CMAKE_INSTALL_RPATH "")

# don't add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH OFF)

enable_testing()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR};${CMAKE_MODULE_PATH}")

find_package(Boost
	COMPONENTS
		system
		filesystem
		serialization
		program_options
		unit_test_framework
	REQUIRED)

if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
	set (BOOST_SYSTEM_DLL ${Boost_SYSTEM_LIBRARY_DEBUG})
	set (BOOST_FILESYSTEM_DLL ${Boost_FILESYSTEM_LIBRARY_DEBUG})
	set (BOOST_SERIALIZATION_DLL ${Boost_SERIALIZATION_LIBRARY_DEBUG})
	set (BOOST_PROGRAM_OPTIONS_DLL ${Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG})
	set (BOOST_UNIT_TEST_FRAMEWORK_DLL ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY_DEBUG})
else ()
	set (BOOST_SYSTEM_DLL ${Boost_SYSTEM_LIBRARY_RELEASE})
	set (BOOST_FILESYSTEM_DLL ${Boost_FILESYSTEM_LIBRARY_RELEASE})
	set (BOOST_SERIALIZATION_DLL ${Boost_SERIALIZATION_LIBRARY_RELEASE})
	set (BOOST_PROGRAM_OPTIONS_DLL ${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE})
	set (BOOST_UNIT_TEST_FRAMEWORK_DLL ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY_RELEASE})
endif ()

string(REPLACE ".lib" ".dll" BOOST_SYSTEM_DLL ${BOOST_SYSTEM_DLL})
string(REPLACE ".lib" ".dll" BOOST_FILESYSTEM_DLL ${BOOST_FILESYSTEM_DLL})
string(REPLACE ".lib" ".dll" BOOST_SERIALIZATION_DLL ${BOOST_SERIALIZATION_DLL})
string(REPLACE ".lib" ".dll" BOOST_PROGRAM_OPTIONS_DLL ${BOOST_PROGRAM_OPTIONS_DLL})
string(REPLACE ".lib" ".dll" BOOST_UNIT_TEST_FRAMEWORK_DLL ${BOOST_UNIT_TEST_FRAMEWORK_DLL})

find_package(OpenGL REQUIRED)
# set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
# find_package(Threads REQUIRED)
find_package(Freetype REQUIRED)
find_package(FBX REQUIRED)
find_package(SDL2 REQUIRED)
find_package(Doxygen)

if (NOT DOXYGEN_FOUND)
	message(WARNING "Doxygen is not present on this system. Documentation cannot be built.")
else ()
	option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})
endif ()

option(WITH_PROFILING "Profile various math functions." OFF)

option(WITH_MEMORY_PROFILING "Profile various memory allocations." OFF)

if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
	option(ENABLE_DEBUG_TRACE "Enable debug tracing via the renderer." ON)
else ()
	option(ENABLE_DEBUG_TRACE "Enable debug tracing via the renderer." OFF)
endif ()

option(HIDE_CONSOLE "Hide the console on Windows." ON)

option(REDIRECT_OUTPUT_WHEN_HIDDEN_CONSOLE "Redirect the standard output to the renderer's error stream." ${ENABLE_DEBUG_TRACE})

option(USE_NEW_CLANG_CPP_LIB "Use libc++ instead of stdlib." OFF)

if (WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /D_SCL_SECURE_NO_WARNINGS /DNOMINMAX /D_USE_MATH_DEFINES /DBOOST_ALL_NO_LIB /arch:SSE")
else ()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -msse3 -DLUA_USE_POSIX")
	if (USE_NEW_CLANG_CPP_LIB)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
	endif ()
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(ENABLE_DEBUG_TRACE ON CACHE BOOL "Enable debug tracing via the renderer.")	
endif ()

if (BUILD_DOCUMENTATION)
	configure_file(Doxyfile.cmake ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
	add_custom_target(doc ALL
		COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating documentation." VERBATIM)
endif()

include_directories(
	${SDL2_INCLUDE_DIR}
	${Boost_INCLUDE_DIR}
	${FREETYPE_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIRS}
	${FBX_INCLUDE_DIR}
	${CMAKE_CURRENT_BINARY_DIR} # For the config file
	"Source/lua-5.3.1/src" # For Lua
	)

configure_file(Source/config.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.hpp)

add_library(gintonic SHARED ${src_files})

# set_target_properties(gintonic PROPERTIES INSTALL_RPATH "")

if (WIN32 AND HIDE_CONSOLE)
	set_target_properties(gintonic PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
endif ()

if (WIN32)
	target_link_libraries(
		gintonic 
		${Boost_LIBRARIES} 
		${OPENGL_LIBRARIES} 
		${SDL2_LIBRARY} 
		${FREETYPE_LIBRARIES}
		${FBX_LIBRARY} 
		${CMAKE_THREAD_LIBS_INIT})
elseif (APPLE)
	target_link_libraries(
		gintonic 
		${Boost_LIBRARIES} 
		${OPENGL_LIBRARIES} 
		${SDL2_LIBRARY} 
		# ${JPEG_LIBRARIES} 
		# ${PNG_LIBRARIES} 
		${FREETYPE_LIBRARIES}
		${FBX_LIBRARY} 
		# ${CMAKE_THREAD_LIBS_INIT})
		)
else ()
	target_link_libraries(
		gintonic 
		${Boost_LIBRARIES} 
		${OPENGL_LIBRARIES} 
		${SDL2_LIBRARY} 
		# ${JPEG_LIBRARIES} 
		# ${PNG_LIBRARIES} 
		${FREETYPE_LIBRARIES}
		${FBX_LIBRARY} 
		# ${CMAKE_THREAD_LIBS_INIT}
		# -ldl
		)
endif ()

add_subdirectory (Source)
add_subdirectory (Tools)
add_subdirectory (Examples)
add_subdirectory (UnitTest)

install(TARGETS gintonic EXPORT gintonic LIBRARY DESTINATION lib)
install(EXPORT gintonic DESTINATION .)
if (BUILD_DOCUMENTATION)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION doc)
endif()