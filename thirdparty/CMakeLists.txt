# Configure glad
set(GLAD_PROFILE "core" CACHE STRING "OpenGL profile")
if (CMAKE_BUILD_TYPE STREQUAL Debug)
    set(GLAD_GENERATOR "c-debug" CACHE STRING "")
endif ()
add_subdirectory(glad)

# Configure SDL2
if (BUILD_SHARED_LIBS)
    # Sigh... SDL should just respect BUILD_SHARED_LIBS
    set(SDL_SHARED_ENABLED_BY_DEFAULT ON CACHE BOOL 
        "Build a shared version of the library" FORCE)
    set(SDL_STATIC OFF CACHE BOOL "Build a static version of the library" FORCE)
    set(SDL2_LIBRARY SDL2 CACHE INTERNAL "")
else()
    set(SDL_SHARED_ENABLED_BY_DEFAULT OFF CACHE BOOL 
        "Build a shared version of the library" FORCE)
    set(SDL_STATIC ON CACHE BOOL "Build a static version of the library" FORCE)
    set(SDL2_LIBRARY SDL2-static CACHE INTERNAL "")
endif()
if(WIN32)
    set(LIBC ON CACHE BOOL "")
endif()
add_subdirectory(SDL-mirror)

# Configure freetype
add_subdirectory(freetype2)

# Configure pybind11
add_subdirectory(pybind11)
