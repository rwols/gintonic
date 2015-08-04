# Locate FBX
# This module defines:
#
# FBX_INCLUDE_DIR
# FBX_LIBRARY
# FBX_FOUND
#
# If you have trouble with this, set
# $FBX_ROOT as an environment variable that
# points to the FBX SDK root folder.

if (APPLE)
    # set (FBX_LIBDIR "gcc4/ub")
    set (FBX_LIBDIR "clang")
elseif (CMAKE_COMPILER_IS_GNUCXX)
    set (FBX_LIBDIR "gcc4")
elseif (MSVC80)
    set (FBX_LIBDIR "vs2005")
elseif (MSVC90)
    set (FBX_LIBDIR "vs2008")
elseif (MSVC10)
    set (FBX_LIBDIR "vs2010")
elseif (MSVC11)
    set (FBX_LIBDIR "vs2012")
elseif (MSVC12 OR MSVC_VERSION>1800)
    set (FBX_LIBDIR "vs2013")
endif ()

if (APPLE)
    # do nothing
elseif (CMAKE_CL_64)
    set (FBX_LIBDIR ${FBX_LIBDIR}/x64)
elseif (CMAKE_COMPILER_IS_GNUCXX AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    set (FBX_LIBDIR ${FBX_LIBDIR}/x64)
else ()
    set (FBX_LIBDIR ${FBX_LIBDIR}/x86)
endif ()

#try to use 2015.1 or 2014.2 version

if (APPLE)
    # set (FBX_LIBNAME "libfbxsdk")
    set (FBX_LIBNAME "fbxsdk")
elseif (CMAKE_COMPILER_IS_GNUCXX)
    set (FBX_LIBNAME "fbxsdk")
else ()
    set (FBX_LIBNAME "libfbxsdk-md")
endif ()

# if (WIN32)
#     set (FBX_LIBNAME_DEBUG ${FBX_LIBNAME}d)
# endif ()

set (FBX_SEARCH_PATHS
    $ENV{FBX_ROOT}
    "$ENV{ProgramW6432}/Autodesk/FBX/FBX SDK/2016.0"
    "$ENV{programfiles}/Autodesk/FBX/FBX SDK/2016.0"
    "/Applications/Autodesk/FBX SDK/2016.0"
    "$ENV{ProgramW6432}/Autodesk/FBX/FBX SDK/2015.1"
    "$ENV{programfiles}/Autodesk/FBX/FBX SDK/2015.1"
    "/Applications/Autodesk/FBX SDK/2015.1"
    "$ENV{ProgramW6432}/Autodesk/FBX/FBX SDK/2014.2"
    "$ENV{programfiles}/Autodesk/FBX/FBX SDK/2014.2"
    "/Applications/Autodesk/FBX SDK/2014.2"
)
#I think the last line in the search path is an old typo, but let's search for 2014.1 anyway - LV

# search for headers & debug/release libraries
find_path(FBX_INCLUDE_DIR "fbxsdk.h"
    PATHS ${FBX_SEARCH_PATHS}
    PATH_SUFFIXES "include")
find_library(FBX_LIBRARY ${FBX_LIBNAME}
    PATHS ${FBX_SEARCH_PATHS}
    PATH_SUFFIXES "lib/${FBX_LIBDIR}/release" "lib/${FBX_LIBDIR}")

#Once one of the calls succeeds the result variable will be set  and stored in the cache so that no call will search again.

#no debug d suffix, search in debug folder only
find_library(FBX_LIBRARY_DEBUG ${FBX_LIBNAME}
    PATHS ${FBX_SEARCH_PATHS}
    PATH_SUFFIXES "lib/${FBX_LIBDIR}/debug")
find_library(FBX_LIBRARY_DEBUG ${FBX_LIBNAME_DEBUG}
    PATHS ${FBX_SEARCH_PATHS}
    PATH_SUFFIXES "lib/${FBX_LIBDIR}")

IF(FBX_LIBRARY AND FBX_LIBRARY_DEBUG AND FBX_INCLUDE_DIR)
    set (FBX_FOUND "YES")
else ()
    set (FBX_FOUND "NO")
endif ()

IF(NOT FBX_FOUND)
#try to use 2014.1 version
    if (APPLE)
        set (FBX_LIBNAME "fbxsdk-2014.1")
    elseif (CMAKE_COMPILER_IS_GNUCXX)
        set (FBX_LIBNAME "fbxsdk-2014.1")
    else ()
        set (FBX_LIBNAME "fbxsdk-2014.1")
    endif ()

    set (FBX_LIBNAME_DEBUG ${FBX_LIBNAME}d)

    set ( FBX_SEARCH_PATHS
        $ENV{FBX_ROOT}
        $ENV{ProgramW6432}/Autodesk/FBX/FBX SDK/2014.1
        $ENV{PROGRAMFILES}/Autodesk/FBX/FBX SDK/2014.1
        /Applications/Autodesk/FBXSDK20141
    )

    # search for headers & debug/release libraries
    find_path(FBX_INCLUDE_DIR "fbxsdk.h"
        PATHS ${FBX_SEARCH_PATHS}
        PATH_SUFFIXES "include")
    find_library( FBX_LIBRARY ${FBX_LIBNAME}
        PATHS ${FBX_SEARCH_PATHS}
        PATH_SUFFIXES "lib/${FBX_LIBDIR}")

    find_library( FBX_LIBRARY_DEBUG ${FBX_LIBNAME_DEBUG}
        PATHS ${FBX_SEARCH_PATHS}
        PATH_SUFFIXES "lib/${FBX_LIBDIR}")
    IF(FBX_LIBRARY AND FBX_LIBRARY_DEBUG AND FBX_INCLUDE_DIR)
        set (FBX_FOUND "YES")
    else ()
        set (FBX_FOUND "NO")
    endif ()

endif ()

IF(NOT FBX_FOUND)
#try to use 2013.3 version
    if (APPLE)
        set (FBX_LIBNAME "fbxsdk-2013.3-static")
    elseif (CMAKE_COMPILER_IS_GNUCXX)
        set (FBX_LIBNAME "fbxsdk-2013.3-static")
    else ()
        set (FBX_LIBNAME "fbxsdk-2013.3-md")
    endif ()

    set (FBX_LIBNAME_DEBUG ${FBX_LIBNAME}d)

    set (FBX_SEARCH_PATHS
        $ENV{FBX_ROOT}
        $ENV{ProgramW6432}/Autodesk/FBX/FBX SDK/2013.3
        $ENV{PROGRAMFILES}/Autodesk/FBX/FBX SDK/2013.3
        /Applications/Autodesk/FBXSDK20133
    )

    # search for headers & debug/release libraries
    find_path(FBX_INCLUDE_DIR "fbxsdk.h"
        PATHS ${FBX_SEARCH_PATHS}
        PATH_SUFFIXES "include")
    find_library( FBX_LIBRARY ${FBX_LIBNAME}
        PATHS ${FBX_SEARCH_PATHS}
        PATH_SUFFIXES "lib/${FBX_LIBDIR}")

    find_library( FBX_LIBRARY_DEBUG ${FBX_LIBNAME_DEBUG}
        PATHS ${FBX_SEARCH_PATHS}
        PATH_SUFFIXES "lib/${FBX_LIBDIR}")
    IF(FBX_LIBRARY AND FBX_LIBRARY_DEBUG AND FBX_INCLUDE_DIR)
        set (FBX_FOUND "YES")
    else ()
        set (FBX_FOUND "NO")
    endif ()

endif ()

set(FBX_LIBRARY optimized ${FBX_LIBRARY} debug ${FBX_LIBRARY_DEBUG})

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(FBX REQUIRED_VARS FBX_LIBRARY FBX_INCLUDE_DIR)

