#include "skybox.hpp"
#include "Renderer.hpp"
#include "shaders.hpp"
#include "basic_shapes.hpp"

namespace gintonic {

skybox::skybox(OpenGL::cube_texture diffuse_texture)
: diffuse_texture(std::move(diffuse_texture))
{
	/* Empty on purpose. */
}

skybox::skybox(skybox&& other)
: diffuse_texture(std::move(other.diffuse_texture))
{
	/* Empty on purpose. */
}

skybox& skybox::operator = (skybox&& other)
{
	diffuse_texture = std::move(other.diffuse_texture);
	return *this;
}

void skybox::draw() const noexcept
{
	const auto& s = Renderer::get_skybox_shader();
	s.activate();
	s.set_matrix_PV(Renderer::matrix_P() * Renderer::matrix_V());
	s.set_skybox_diffuse(0);
	diffuse_texture.bind(0);

	// We must enable depth testing.
	glEnable(GL_DEPTH_TEST);

	// // We render from the inside of a cube, so we must flip
	// // our definition of what we call a front face triangle.
	// glFrontFace(GL_CW);

	// Change depth function so depth test passes when values
	// are equal to depth buffer's content
	glDepthFunc(GL_LEQUAL);
	
	Renderer::get_unit_cube_P_flipped().draw();

	// Restore default values.
	glDepthFunc(GL_LESS);
	// glFrontFace(GL_CCW);
}

} // namespace gintonic
