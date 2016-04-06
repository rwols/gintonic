#include "ShaderPrograms.hpp"

namespace gintonic {

DepthBufferShaderProgram::DepthBufferShaderProgram()
: OpenGL::ShaderProgram("Shaders/DepthBuffer.vert", "Shaders/DepthBuffer.frag")
{
	/* Empty on purpose. */
}

ShadowShaderProgram::ShadowShaderProgram()
: OpenGL::ShaderProgram("Shaders/Shadow.vert", "Shaders/Shadow.frag")
{
	/* Empty on purpose. */
}

MaterialShaderProgram::MaterialShaderProgram()
: OpenGL::ShaderProgram("Shaders/Material.vert", "Shaders/Material.frag")
{
	/* Empty on purpose. */
}

AmbientLightShaderProgram::AmbientLightShaderProgram()
: OpenGL::ShaderProgram("Shaders/LightPass.vert", "Shaders/AmbientLight.frag")
{
	/* Empty on purpose. */
}

DirectionalLightShaderProgram::DirectionalLightShaderProgram()
: OpenGL::ShaderProgram("Shaders/LightPass.vert", "Shaders/DirectionalLight.frag")
{
	/* Empty on purpose. */
}

PointLightShaderProgram::PointLightShaderProgram()
: OpenGL::ShaderProgram("Shaders/LightPassPVM.vert", "Shaders/PointLight.frag")
{
	/* Empty on purpose. */
}

SpotLightShaderProgram::SpotLightShaderProgram()
: OpenGL::ShaderProgram("Shaders/LightPassPVM.vert", "Shaders/SpotLight.frag")
{
	/* Empty on purpose. */
}

SkyboxShaderProgram::SkyboxShaderProgram()
: OpenGL::ShaderProgram("Shaders/Skybox.vert", "Shaders/Skybox.frag")
{
	/* Empty on purpose. */
}

FlatTextShaderProgram::FlatTextShaderProgram()
: OpenGL::ShaderProgram("Shaders/FlatTextUniformColor.vert", "Shaders/FlatTextUniformColor.frag")
{
	/* Empty on purpose. */
}

GUIShaderProgram::GUIShaderProgram()
: OpenGL::ShaderProgram("Shaders/GUI.vert", "Shaders/GUI.frag")
{
	/* Empty on purpose. */
}

} // namespace gintonic