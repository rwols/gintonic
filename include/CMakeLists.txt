set(GINTONIC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "The include directory containing all headers.")
set(GINTONIC_SHADER_DIR "${GINTONIC_INCLUDE_DIR}/Graphics/Shaders" CACHE INTERNAL "The shader directory.")
install(DIRECTORY . DESTINATION .)