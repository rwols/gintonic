#*******************************************************************************
# gintonic
#
# This is the top-level cmake file for the gintonic project. It first checks
# wether the user invoked the cmake command from within the source directory.
# If so, we stop immediately and tell the user to invoke the cmake command from
# a "build" directory so that the source directory does not get polluted with
# build artefacts.
# After that, we check wether this file is the top-level cmake file. If so,
# we build the examples, tools and unit tests by default (but that can be 
# disabled if so desired).
# After that, we include the relevant subdirectories and cmake handles the rest.
#
# Important variables that this cmake file defines are:
#
# - gintonic_VERSION_MAJOR    -- The major version
# - gintonic_VERSION_MINOR    -- The minor version
# - gintonic_VERSION_PATCH    -- The patch level
# - gintonic_BUILD_UNIT_TESTS -- Wether to build the unit tests
# - gintonic_BUILD_EXAMPLES   -- Wether to build the examples
# - gintonic_BUILD_TOOLS      -- Wether to build the tools
#
#*******************************************************************************

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
	message(FATAL_ERROR "Prevented in-source build. \
		Please create a build directory outside of the \
		Gintonic source code and call CMake from there.")
endif()

project(Gintonic VERSION 0.1 LANGUAGES CXX)

set(gintonic_WE_ARE_ROOT OFF CACHE INTERNAL "")

# If the current source directory equals the root source directory, then we're
# building gintonic as a "master" project. Otherwise, we must have been included
# via a call to add_subdirectory somewhere up in the directory tree. If that's 
# the case, we disable a few options by default like unit tests, examples, and
# tools. They can be enabled again though, if so desired.
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
	set(gintonic_WE_ARE_ROOT ON CACHE INTERNAL "")
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
	if (NOT CMAKE_BUILD_TYPE)
		set(CMAKE_BUILD_TYPE Debug CACHE STRING 
			"The build type \(Debug/Release\)")
		set(BUILD_SHARED_LIBS ON CACHE BOOL 
			"Wether to build shared or static libraries.")
	endif()
	if (CMAKE_BUILD_TYPE STREQUAL Release)
		set(CMAKE_C_VISIBILITY_PRESET hidden CACHE INTERNAL "")
		set(CMAKE_CXX_VISIBILITY_PRESET hidden CACHE INTERNAL "")
		set(CMAKE_VISIBILITY_INLINES_HIDDEN 1 CACHE INTERNAL "")
	endif()
endif()

option(gintonic_BUILD_UNIT_TESTS 
	"Wether to build the unit tests." ${gintonic_WE_ARE_ROOT})
option(gintonic_BUILD_EXAMPLES 
	"Wether to build the examples." ${gintonic_WE_ARE_ROOT})
option(gintonic_BUILD_TOOLS 
	"Wether to build the tools." ${gintonic_WE_ARE_ROOT})

add_subdirectory(thirdparty)
add_subdirectory(assets)
add_subdirectory(include)
add_subdirectory(lib)

if (gintonic_BUILD_UNIT_TESTS)
	if (gintonic_WE_ARE_ROOT)
		enable_testing()
	endif (gintonic_WE_ARE_ROOT)
	add_subdirectory(test)
endif (gintonic_BUILD_UNIT_TESTS)

if (gintonic_BUILD_TOOLS)
	add_subdirectory(tools)
endif (gintonic_BUILD_TOOLS)

if (gintonic_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif (gintonic_BUILD_EXAMPLES)

if (gintonic_WE_ARE_ROOT)
	include(InstallRequiredSystemLibraries)
	set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Game Engine")
	set(CPACK_PACKAGE_VENDOR "Raoul Wols")
	set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
	# set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/Copyright.txt")
	set(CPACK_PACKAGE_VERSION_MAJOR ${gintonic_VERSION_MAJOR})
	set(CPACK_PACKAGE_VERSION_MINOR ${gintonic_VERSION_MINOR})
	set(CPACK_PACKAGE_VERSION_PATCH ${gintonic_VERSION_PATCH})
	set(CPACK_PACKAGE_INSTALL_DIRECTORY 
		"gintonic ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")
	if (WIN32 AND NOT UNIX)
		# There is a bug in NSI that does not handle full unix paths properly. Make
		# sure there is at least one set of four (4) backlasshes.
		set(CPACK_PACKAGE_ICON 
			"${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
		set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
		set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}")
		set(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.primef.actor")
		set(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.primef.actor")
		set(CPACK_NSIS_CONTACT "r@primef.actor")
		set(CPACK_NSIS_MODIFY_PATH ON)
	else (WIN32 AND NOT UNIX)
		set(CPACK_STRIP_FILES "bin/MyExecutable")
		set(CPACK_SOURCE_STRIP_FILES "")
	endif (WIN32 AND NOT UNIX)
	# set(CPACK_PACKAGE_EXECUTABLES "MyExecutable" "My Executable")
	include(CPack)
endif (gintonic_WE_ARE_ROOT)
