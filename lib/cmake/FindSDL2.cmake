################################################################################
#
# Use like this:
#
# find_package(SDL2 REQUIRED)
# target_link_libraries(my_exe_or_lib PUBLIC SDL2)
#
################################################################################

find_program(SDL2_CONFIG_PROGRAM sdl2-config)
if(NOT SDL2_CONFIG_PROGRAM)
    message(SEND_ERROR "Could not find sdl2-config")
    set(SDL2_FOUND OFF)
else()
    set(SDL2_FOUND ON)
endif(NOT SDL2_CONFIG_PROGRAM)

if (SDL2_FOUND)
    execute_process(
        COMMAND
            "${SDL2_CONFIG_PROGRAM}" --version
        RESULT_VARIABLE
            status
        OUTPUT_VARIABLE
            version
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if (NOT "${status}" EQUAL 0)
        message(SEND_ERROR "${SDL2_CONFIG_PROGRAM} exited with status ${status}")
        set(SDL2_FOUND OFF)
    endif()
endif(SDL2_FOUND)

if (SDL2_FOUND)
    execute_process(
        COMMAND
            "${SDL2_CONFIG_PROGRAM}" --cflags
        RESULT_VARIABLE
            status
        OUTPUT_VARIABLE
            flags
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if (NOT "${status}" EQUAL 0)
        message(SEND_ERROR "${SDL2_CONFIG_PROGRAM} exited with status ${status}")
        set(SDL2_FOUND OFF)
    endif()
endif(SDL2_FOUND)

if (SDL2_FOUND)
    if (BUILD_SHARED_LIBS OR NOT BUILD_STATIC_LIBS)
        set(param --libs)
    else()
        set(param --static-libs)
    endif()
    execute_process(
        COMMAND
            "${SDL2_CONFIG_PROGRAM}" ${param}
        RESULT_VARIABLE
            status
        OUTPUT_VARIABLE
            libs
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if (NOT "${status}" EQUAL 0)
        message(SEND_ERROR "${SDL2_CONFIG_PROGRAM} exited with status ${status}")
        set(SDL2_FOUND OFF)
    endif()
endif(SDL2_FOUND)

if (SDL2_FOUND)
    add_library(SDL2 INTERFACE)
    if (UNIX)
        set(command_type UNIX_COMMAND)
    elseif(WIN32)
        set(command_type WINDOWS_COMMAND)
    else()
        message(SEND_ERROR "Unsupported platform (must be either UNIX or WIN32)")
        set(SDL2_FOUND OFF)
    endif()
    separate_arguments(flags ${command_type} "${flags}")
    separate_arguments(libs ${command_type} "${libs}")
    target_compile_options(SDL2 INTERFACE ${flags})
    target_link_libraries(SDL2 INTERFACE ${libs})
endif(SDL2_FOUND)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2
    FOUND_VAR SDL2_FOUND
    REQUIRED_VARS flags libs
    VERSION_VAR version)

if (SDL2_FOUND)
    message(STATUS "SDL2 version: ${version}")
endif(SDL2_FOUND)
