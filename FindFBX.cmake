# Locate FBX
# This module defines:
#
# FBX_INCLUDE_DIR -- The FBX SDK include directory
# FBX_LIBRARY -- The FBX SDK library (debug or release, depending on CMake's configuration)
# FBX_NAMESPACE -- The C++ name of the FBX SDK namespace. This is useful for forward-declaring FBX types.
# FBX_FOUND -- Wether everything went okay.
#
# If CMake has trouble finding the FBX library, you can define
# and environment variable called FBX_ROOT that should point tro
# the root folder of the FBX SDK. For example, if the FBX SDK is
# located at C:\FBX\2016.0, then FBX_ROOT should resolve to that folder.
#
# If you want to use a specific FBX version, you define a variable
# named FBX_PREFER_VERSION. For example, set(FBX_PREFER_VERSION "2015.1")

# Prepend more version numbers when they get released
# REMARK: PREPEND, not APPEND.
if (FBX_PREFER_VERSION)
    set(versions ${FBX_PREFER_VERSION})
else ()
    set (versions "2016.0" "2015.1" "2014.2" "2014.1")
endif ()

set(GCC_OR_CLANG OFF)

set(CLANG_COMPILER OFF)
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CLANG_COMPILER ON)
endif ()

if (CMAKE_COMPILER_IS_GNUCXX OR CLANG_COMPILER)
    set(GCC_OR_CLANG ON)
endif ()

if (APPLE)
    # set (libdir "gcc4/ub")
    set (libdir "clang")
elseif (GCC_OR_CLANG)
    set (libdir "gcc4")
elseif (MSVC80)
    set (libdir "vs2005") 
elseif (MSVC90)
    set (libdir "vs2008")
elseif (MSVC10)
    set (libdir "vs2010")
elseif (MSVC11)
    set (libdir "vs2012")
elseif (MSVC12 OR MSVC_VERSION>1800)
    set (libdir "vs2013")
endif ()

if (APPLE)
    # do nothing
elseif (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set (libdir "${libdir}/x64")
else ()
    set (libdir "${libdir}/x86")
endif ()

set (search_path_prefixes 
    "$ENV{ProgramW6432}/Autodesk/FBX/FBX SDK"
    "$ENV{programfiles}/Autodesk/FBX/FBX SDK"
    "/Applications/Autodesk/FBX SDK")

foreach (search_path IN LISTS search_path_prefixes)
    foreach (version IN LISTS versions)
        list(APPEND search_paths "${search_path}/${version}")
    endforeach()
endforeach ()

set (fbx_libname "fbxsdk")

if (WIN32)
    set (fbx_libname "lib${fbx_libname}.lib")
    add_definitions(/D FBXSDK_SHARED)
endif ()

list(APPEND search_paths "$ENV{FBX_ROOT}")

find_path(FBX_INCLUDE_DIR "fbxsdk.h"
    PATHS ${search_paths}
    PATH_SUFFIXES "include")

find_library(FBX_LIBRARY ${fbx_libname}
    PATHS ${search_paths}
    PATH_SUFFIXES "lib/${libdir}/release")

find_library(FBX_LIBRARY_DEBUG ${fbx_libname}
    PATHS ${search_paths}
    PATH_SUFFIXES "lib/${libdir}/debug")

if (FBX_INCLUDE_DIR)
    if (WIN32 OR APPLE)
    foreach (version IN LISTS versions)
        string(REGEX MATCH ${version} FBX_NAMESPACE ${FBX_INCLUDE_DIR})
        if (FBX_NAMESPACE)
            string(REPLACE "." "_" FBX_NAMESPACE ${FBX_NAMESPACE})
            set(FBX_NAMESPACE "fbxsdk_${FBX_NAMESPACE}" CACHE STRING "The Fbx namespace.")
            break()
        endif ()
    endforeach ()
    else ()
        # BIG HACK!!! IMPROVE THIS
        set(FBX_NAMESPACE "fbxsdk_2015_1" CACHE STRING "BIG HACK!!!")
    endif ()
endif ()

IF(FBX_LIBRARY AND FBX_LIBRARY_DEBUG AND FBX_INCLUDE_DIR)
    set (FBX_FOUND ON CACHE BOOL "Fbx has been found.")
else ()
    set (FBX_FOUND OFF CACHE BOOL "Fbx has not been found.")
endif ()

set(FBX_LIBRARY optimized ${FBX_LIBRARY} debug ${FBX_LIBRARY_DEBUG} CACHE STRING "Fbx Library location.")

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(FBX REQUIRED_VARS FBX_LIBRARY FBX_INCLUDE_DIR)

