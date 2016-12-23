################################################################################
# Locate FBX
# This module defines:
#
# FBX_INCLUDE_DIRS -- The FBX SDK include directories.
# FBX_LIBARIES     -- The FBX SDK libraries. Depending on the build type, the
#                     debug or release version is chosen. If BUILD_SHARED_LIBS
#                     is ON, then this variable will contain the shared library
#                     variants. Otherwise, it will contain the static library
#                     variants.
# FBX_FOUND        -- Wether everything went okay.
#
# If CMake has trouble finding the FBX library, you can define an environment 
# variable called FBX_ROOT that should point to the root folder of the FBX SDK.
# For example, if the FBX SDK is located at C:\FBX\2016.0, then FBX_ROOT should 
# resolve to C:\FBX\2016.0. It is also possible to specify the FBX_ROOT variable
# as a CMake cache variable.
#
# If you want to use a specific FBX version, you can define a variable named
# FBX_PREFER_VERSION. For example, set(FBX_PREFER_VERSION 2015.1 CACHE INTERNAL)
################################################################################

if (FBX_PREFER_VERSION)
    set(versions ${FBX_PREFER_VERSION})
else ()
    # Prepend more version numbers when they get released
    set(versions
        2017.0
        2016.1.2
        2016.1
        2016
        2015.1
        2015
        2014.2.1
        2014.1
        2013.3
        2012.2
        2011.3.1
        2011.3
        2011.2
        2010.2
        2010.0.2
        2009.3
        2009.1
        2006.11.1
        2005.12.1
        2005.12
        )
endif ()

if (WIN32) # FIXME!
    set(compiler_id vs2005 vs2008 vs2010 vs2012 vs2013 vs2015)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(arch x64)
    else ()
        set(arch x86)
    endif()
    set (search_path_prefixes 
        "$ENV{ProgramW6432}/Autodesk/FBX/FBX\ SDK"
        "$ENV{programfiles}/Autodesk/FBX/FBX\ SDK"
        )
    set(libdir_suffix lib/${compiler_id}/${arch}/release)
    set(libdir_suffix_debug lib/${compiler_id}/${arch}/debug)
    set(fbx_libname libfbxsdk.lib)
elseif (APPLE)
    set(compiler_id clang)
    set(search_path_prefixes "/Applications/Autodesk/FBX\ SDK")
    if (BUILD_SHARED_LIBS)
        set(fbx_libname libfbxsdk.dylib)
    else (BUILD_SHARED_LIBS)
        set(fbx_libname libfbxsdk.a)
    endif (BUILD_SHARED_LIBS)
    set(libdir_suffix lib/${compiler_id})
    if (FBX_USE_LIBCXX)
        set(libdir_suffix ${libdir_suffix}/libstdcpp)
    endif (FBX_USE_LIBCXX)
    set(libdir_suffix_debug ${libdir_suffix}/debug)
    set(libdir_suffix ${libdir_suffix}/release)
elseif (UNIX)
    set(compiler_id gcc4)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(arch x64)
    else ()
        set(arch x86)
    endif()
    set(search_path_prefixes /usr/local)
    set(libdir_suffix lib/${compiler_id}/${arch}/release)
    set(libdir_suffix_debug lib/${compiler_id}/${arch}/debug)
    if (BUILD_SHARED_LIBS)
        set(fbx_libname libfbxsdk.so)
    else (BUILD_SHARED_LIBS)
        set(fbx_libname libfbxsdk.a)
    endif (BUILD_SHARED_LIBS)
else (WIN32)
    message(FATAL_ERROR "Could not determine platform!")
endif (WIN32)

list(APPEND search_paths "${FBX_ROOT}")
list(APPEND search_paths "$ENV{FBX_ROOT}")

if (APPLE OR WIN32)
    foreach (search_path_prefix ${search_path_prefixes})
        foreach (version ${versions})
            list(APPEND search_paths "${search_path_prefix}/${version}")
        endforeach (version)
    endforeach (search_path_prefix)
else ()
    message(STATUS "${search_path_prefixes}")
    foreach (search_path_prefix ${search_path_prefixes})
        list(APPEND search_paths "${search_path_prefix}")
    endforeach (search_path_prefix)
endif ()

find_path(FBX_INCLUDE_DIRS 
    NAMES fbxsdk.h
    PATHS ${search_paths}
    PATH_SUFFIXES include
    NO_DEFAULT_PATH)

find_library(FBX_LIBRARY 
    NAMES ${fbx_libname}
    PATHS ${search_paths}
    PATH_SUFFIXES ${libdir_suffix}
    NO_DEFAULT_PATH)

find_library(FBX_LIBRARY_DEBUG 
    NAMES ${fbx_libname}
    PATHS ${search_paths}
    PATH_SUFFIXES ${libdir_suffix_debug}
    NO_DEFAULT_PATH)

set(FBX_LIBRARIES optimized ${FBX_LIBRARY} debug ${FBX_LIBRARY_DEBUG})

if (IS_DIRECTORY "${FBX_INCLUDE_DIRS}")
    file(READ "${FBX_INCLUDE_DIRS}/fbxsdk/fbxsdk_version.h" fbx_version_file)
    string(REGEX MATCH "#define FBXSDK_VERSION_MAJOR\t[0-9]+" 
        fbxver_major "${fbx_version_file}")
    string(REGEX REPLACE "#define FBXSDK_VERSION_MAJOR\t([0-9]+)" "\\1" 
        fbxver_major "${fbxver_major}")
    string(REGEX MATCH "#define FBXSDK_VERSION_MINOR\t[0-9]+" 
        fbxver_minor "${fbx_version_file}")
    string(REGEX REPLACE "#define FBXSDK_VERSION_MINOR\t([0-9]+)" "\\1" 
        fbxver_minor "${fbxver_minor}")
    string(REGEX MATCH "#define FBXSDK_VERSION_POINT\t[0-9]+" 
        fbxver_point "${fbx_version_file}")
    string(REGEX REPLACE "#define FBXSDK_VERSION_POINT\t([0-9]+)" "\\1" 
        fbxver_point "${fbxver_point}")
else ()
    message(SEND_ERROR "FBX include directory not found!")
endif ()

if (DEFINED fbxver_major AND DEFINED fbxver_minor AND DEFINED fbxver_point)
    set(FBX_VERSION ${fbxver_major}.${fbxver_minor}.${fbxver_point})
else ()
    message(SEND_ERROR "Could not determine FBX version!")
endif ()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FBX 
    FOUND_VAR FBX_FOUND 
    REQUIRED_VARS FBX_LIBRARIES FBX_INCLUDE_DIRS 
    VERSION_VAR FBX_VERSION)

if (FBX_FOUND)
    add_library(FBX INTERFACE)
    if (WIN32)
        target_compile_definitions(FBX INTERFACE FBX_SHARED)
    endif ()
    target_include_directories(FBX INTERFACE ${FBX_INCLUDE_DIRS})
    target_link_libraries(FBX INTERFACE ${FBX_LIBRARIES})
endif ()

mark_as_advanced(FBX_FOUND)
mark_as_advanced(FBX_VERSION)
mark_as_advanced(FBX_INCLUDE_DIRS)
mark_as_advanced(FBX_LIBRARIES)
