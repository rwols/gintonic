#*******************************************************************************
# gintonic/lib
#
# The purpose of this cmake file is to create the "gintonic" library target. It
# attempts to find the packages "OpenGL" and "Boost". Every modern OS has
# OpenGL, so that will probably pose no problems. However Boost requires
# manual installation of the user. If you are on Linux, the Boost libraries are
# probably in your package manager. For instance, on Ubuntu you can install
# Boost with the command `apt get install libboost-dev`.
# Other dependencies include SDL, freetype, glad, assimp and pybind11, but all
# those are included as git submodules, so nothing has to be done and they will
# "just work".
#
# Important variables that this cmake file defines are:
#
# - gintonic_LIB_DIR -- This directory (containing the implementation files)
# - gintonic_SSE_VERSION -- The SSE target (as a string) to compile against
# - gintonic_WITH_PROFILING -- Profile various math functions
# - gintonic_WITH_MEMORY_PROFILING -- Profile memory allocations
# - gintonic_ENABLE_DEBUG_TRACE -- Enable debug tracing via the Renderer
# - gintonic_HIDE_CONSOLE -- Hide the console (only applicable to Windows)
# - gintonic_REDIRECT_OUTPUT_WHEN_HIDDEN_CONSOLE -- When the console is hidden
#     on Windows, wether to redirect standard output streams to files.
# - gintonic_USE_LIBCXX -- Wether to use Clang's libc++ library. Only applicable
#     when the compiler is Clang.
#
#*******************************************************************************

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
if (NOT BUILD_SHARED_LIBS)
    set(Boost_USE_STATIC_LIBS ON)
endif()
find_package(Boost COMPONENTS system filesystem serialization REQUIRED)

set(gintonic_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL
    "The directory containing implementation files.")

set(gintonic_source_files

    # Foundation
    Foundation/Clock.cpp
    Foundation/ReadWriteLock.cpp
    Foundation/exception.cpp
    Foundation/Profiler.cpp
    Foundation/WriteLock.cpp
    Foundation/simd.cpp
    Foundation/filesystem.cpp
    Foundation/Octree.cpp

    # Graphics/OpenGL
    Graphics/OpenGL/BufferObject.cpp
    Graphics/OpenGL/ShaderProgram.cpp
    Graphics/OpenGL/VertexArrayObject.cpp
    Graphics/OpenGL/Vertices.cpp
    Graphics/OpenGL/utilities.cpp
    Graphics/OpenGL/TextureObject.cpp
    Graphics/OpenGL/SourceCode.cpp
    Graphics/OpenGL/Framebuffer.cpp

    # Graphics
    Graphics/PointShadowBuffer.cpp
    Graphics/skybox.cpp
    Graphics/AnimationClip.cpp
    Graphics/Skeleton.cpp
    Graphics/AmbientLight.cpp
    Graphics/Renderer.cpp
    Graphics/GUI/Base.cpp
    Graphics/GUI/Panel.cpp
    Graphics/GUI/StringView.cpp
    Graphics/GUI/StringPointerView.cpp
    Graphics/Texture2D.cpp
    Graphics/Light.cpp
    Graphics/PointLight.cpp
    Graphics/SpotLight.cpp
    Graphics/Material.cpp
    Graphics/Mesh.cpp
    Graphics/GeometryBuffer.cpp
    Graphics/SpotShadowBuffer.cpp
    Graphics/DirectionalLight.cpp
    Graphics/Font.cpp
    Graphics/ShaderPrograms.cpp
    Graphics/DirectionalShadowBuffer.cpp

    # Math
    Math/vec3f.cpp
    Math/mat3f.cpp
    Math/mat2f.cpp
    Math/vec2f.cpp
    Math/quatf.cpp
    Math/box2f.cpp
    Math/mat4f.cpp
    Math/SQT.cpp
    Math/vec4f.cpp
    Math/box3f.cpp

    # ???
    Application.cpp
    ApplicationStateMachine.cpp
    Asset.cpp
    Behaviour.cpp
    BoxCollider.cpp
    Camera.cpp
    Collider.cpp
    Component.cpp
    Component.cpp
    Entity.cpp
    # EntityBase.cpp
    EntityVisitor.cpp
    gintonic.cpp
    Image.cpp
    MeshRenderer.cpp
    OctreeComp.cpp
    Renderer.cpp
    RenderStrategy.cpp
    RunLoop.cpp
    Scene.cpp
    SDLRenderContext.cpp
    SDLRunLoop.cpp
    SDLWindow.cpp
    Texture2D.cpp
    Transform.cpp
    Window.cpp

    # imgui
    ../thirdparty/imgui/imgui.cpp
    ../thirdparty/imgui/imgui_demo.cpp
    ../thirdparty/imgui/imgui_draw.cpp
    )

set(gintonic_SSE_VERSION 30 CACHE STRING "The SSE version.")
option(gintonic_WITH_PROFILING "Profile various math functions." OFF)
option(gintonic_WITH_MEMORY_PROFILING "Profile various memory allocations." OFF)
if (CMAKE_BUILD_TYPE STREQUAL Debug)
    option(gintonic_ENABLE_DEBUG_TRACE
        "Enable debug tracing via the renderer." ON)
else ()
    option(gintonic_ENABLE_DEBUG_TRACE
        "Enable debug tracing via the renderer." OFF)
endif ()
option(gintonic_HIDE_CONSOLE "Hide the console on Windows." ON)
option(gintonic_REDIRECT_OUTPUT_WHEN_HIDDEN_CONSOLE
    "Redirect the standard output to the renderer's error stream."
    ${gintonic_ENABLE_DEBUG_TRACE})
option(gintonic_USE_LIBCXX "Use libc++ instead of stdlib." OFF)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(gintonic_ENABLE_DEBUG_TRACE ON CACHE BOOL
        "Enable debug tracing via the renderer.")
endif ()
configure_file(cmake/config.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.hpp)

add_library(gintonic ${gintonic_source_files})

if (WIN32)
    target_compile_definitions(gintonic PUBLIC _SCL_SECURE_NO_WARNINGS)
    target_compile_definitions(gintonic PUBLIC NOMINMAX)
    target_compile_definitions(gintonic PUBLIC _USE_MATH_DEFINES)
    target_compile_definitions(gintonic PUBLIC BOOST_ALL_NO_LIB)
    if (CMAKE_SIZEOF_VOID_P EQUAL 4)
        target_compile_options(gintonic PUBLIC /arch:SSE2)
    endif()
    target_compile_options(gintonic PRIVATE /MP)
    if(NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL Debug)
        target_compile_options(gintonic PUBLIC /MDd)
    else()
        target_compile_options(gintonic PUBLIC /MD)
    endif()
else ()
    target_compile_options(gintonic PUBLIC -msse3)
    target_compile_options(gintonic PRIVATE -Wall)
    target_compile_options(gintonic PUBLIC -fvisibility=hidden)
    target_compile_options(gintonic PUBLIC -fvisibility-inlines-hidden)
    if (CMAKE_STRIP AND CMAKE_BUILD_TYPE STREQUAL Release)
        if (APPLE)
            add_custom_command(TARGET gintonic POST_BUILD COMMAND ${CMAKE_STRIP} -x $<TARGET_FILE:gintonic>)
        else ()
            add_custom_command(TARGET gintonic POST_BUILD COMMAND ${CMAKE_STRIP} -u -r $<TARGET_FILE:gintonic>)
        endif()
    endif ()
    if (gintonic_USE_LIBCXX)
        target_compile_options(gintonic PUBLIC -stdlib=libc++)
    endif ()
endif ()

set_target_properties(gintonic PROPERTIES CXX_STANDARD 14)

if (WIN32 AND gintonic_HIDE_CONSOLE)
    set_target_properties(gintonic PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
endif ()

target_include_directories(gintonic SYSTEM PUBLIC
    ${GLAD_BINARY_DIR}/include # Generated by glad
    ${Boost_INCLUDE_DIR}
    ${PYBIND11_INCLUDE_DIR}
    # ${PYTHON3_INCLUDE_DIRS}
    ${freetype_SOURCE_DIR}/include
    ${SDL2_SOURCE_DIR}/include
    ../thirdparty/imgui # For including imgui.h
    )

target_include_directories(gintonic PUBLIC
    ${Gintonic_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR} # For the config file
    )

target_link_libraries(gintonic PUBLIC
    ${Boost_LIBRARIES}
    ${SDL2_LIBRARY}
    freetype
    glad
    )

# function(target_precompiled_header target headerfile)
#     if (CMAKE_CXX_COMPILER_ID MATCHES Clang)
#         get_filename_component(name ${headerfile} NAME)
#         set(source "${CMAKE_CURRENT_SOURCE_DIR}/${headerfile}")
#         set(output "${CMAKE_CURRENT_BINARY_DIR}/${headerfile}.gch")
#         set(guardfile "${CMAKE_CURRENT_BINARY_DIR}/${headerfile}")
#         string(TOUPPER CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE} flags)
#         set(flags ${${flags}})
#         get_target_property(items ${target} INCLUDE_DIRECTORIES)
#         foreach(item ${items})
#             list(APPEND flags -I${item})
#         endforeach(item)
#         get_target_property(items ${target} COMPILE_DEFINITIONS)
#         if (items)
#             foreach(item ${items})
#                 list(APPEND flags ${item})
#             endforeach(item)
#         endif (items)
#         get_target_property(items ${target} COMPILE_OPTIONS)
#         if (items)
#             foreach(item ${items})
#                 list(APPEND flags ${item})
#             endforeach(item)
#         endif (items)
#         list(APPEND flags "-std=gnu++14")
#         separate_arguments(flags)
#         add_custom_command(
#             OUTPUT ${output}
#             COMMAND ${CMAKE_CXX_COMPILER}
#                     ${flags}
#                     -c -x c++-header ${source}
#                     -o ${output}
#             MAIN_DEPENDENCY ${source})
#         file(WRITE "${guardfile}" "#error Failed to include precompiled header!")
#         add_custom_target(${target}-precompiled DEPENDS ${output})
#         add_dependencies(${target} ${target}-precompiled)
#         target_compile_options(${target} BEFORE PUBLIC -include "${guardfile}")
#     else()
#         message(WARNING "Not generating a precompiled header for ${CMAKE_CXX_COMPILER_ID}.")
#     endif()
# endfunction(target_precompiled_header)

# target_precompiled_header(gintonic Precompiled.hpp)

install(TARGETS gintonic DESTINATION lib)
if (WIN32)
    # add_executable(maintest main.cpp)
else()
    add_executable(maintest main.cpp)
    target_link_libraries(maintest PRIVATE gintonic)
    set_target_properties(maintest PROPERTIES CXX_STANDARD 14)
endif()


