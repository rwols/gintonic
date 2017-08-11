# Configure glad
set(GLAD_PROFILE "core" CACHE STRING "OpenGL profile")
if (CMAKE_BUILD_TYPE STREQUAL Debug)
    set(GLAD_GENERATOR "c-debug" CACHE STRING "")
endif ()
add_subdirectory(glad)

# Configure pybind11
add_subdirectory(pybind11)
