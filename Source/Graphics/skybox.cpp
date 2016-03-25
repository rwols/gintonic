#include "skybox.hpp"
#include "Mesh.hpp"
#include "Renderer.hpp"
#include "ShaderPrograms.hpp"

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
	const auto& lProgram = SkyboxShaderProgram::get();
	lProgram.activate();
	lProgram.setMatrixPV(Renderer::matrix_P() * Renderer::matrix_V());
	lProgram.setDiffuseTexture(0);
	diffuse_texture.bind(0);

	// We must enable depth testing.
	glEnable(GL_DEPTH_TEST);

	// // We render from the inside of a cube, so we must flip
	// // our definition of what we call a front face triangle.
	// glFrontFace(GL_CW);

	// Change depth function so depth test passes when values
	// are equal to depth buffer's content
	glDepthFunc(GL_LEQUAL);
	
	Renderer::getInsideOutUnitCube()->draw();

	// Restore default values.
	glDepthFunc(GL_LESS);
	// glFrontFace(GL_CCW);
}

} // namespace gintonic
