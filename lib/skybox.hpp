#ifndef gintonic_skybox_hpp
#define gintonic_skybox_hpp

#include "textures.hpp"

namespace gintonic {

class skybox
{
public:

	skybox() = default;

	// Use move constructor for opengl::cube_texture
	skybox(opengl::cube_texture diffuse_texture);

	skybox(const skybox&) = default;
	skybox& operator = (const skybox&) = default;
	skybox(skybox&&);
	skybox& operator = (skybox&&);

	opengl::cube_texture diffuse_texture;

	void draw() const BOOST_NOEXCEPT_OR_NOTHROW;

private:

};

} // namespace gintonic

#endif