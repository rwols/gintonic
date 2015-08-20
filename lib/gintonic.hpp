#ifndef gintonic_hpp
#define gintonic_hpp

#include "renderer.hpp"
#include "actors.hpp"
#include "materials.hpp"
#include "mesh.hpp"
#include "opengl.hpp"
#include "shaders.hpp"
#include "fonts.hpp"
#include "basic_shapes.hpp"
#include "textures.hpp"
#include "vertices.hpp"
#include "lights.hpp"
#include "skybox.hpp"
#include "scancodes.hpp"

namespace gintonic {

camera& get_default_camera() BOOST_NOEXCEPT_OR_NOTHROW;

void init_all(const char* window_title);

} // namespace gintonic

#endif