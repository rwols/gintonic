# Configure glad
add_subdirectory(glad/cmake)

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
set(VIDEO_WAYLAND ON)
set(WAYLAND_SHARED ON)
add_subdirectory(SDL)

# Configure freetype
# Cannot build shared libs for freetype on Windows; just use
# -DBUILD_SHARED_LIBS=OFF on the command line.
add_subdirectory(freetype2)

# Configure pybind11
add_subdirectory(pybind11)
