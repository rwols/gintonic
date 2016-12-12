/**
 * @file ShaderPrograms.hpp
 * @brief Defines all the shader classes.
 * @author Raoul Wols
 */

#pragma once

#include "../Foundation/allocator.hpp"

#include "OpenGL/ShaderProgram.hpp"
#include "OpenGL/BufferObject.hpp"
#include "OpenGL/utilities.hpp"

namespace gintonic {

namespace Uniform {

#define GT_STRINGIFY(x) #x

#define GT_PASTE_TOGETHER(x, y) x ## y

#define GT_DEFINE_UNIFORM(UNIFORM_TYPE, UNIFORM_NAME, UNIFORM_NAME_WITH_FIRST_CAPITAL)       \
/** @brief Class that encapsulates a uniform variable in a shader. */                        \
class UNIFORM_NAME : virtual public OpenGL::ShaderProgram                                    \
{                                                                                            \
private:                                                                                     \
	GLuint mLocation;                                                                        \
protected:                                                                                   \
	UNIFORM_NAME()                                                                           \
	{                                                                                        \
		mLocation = getUniformLocation(GT_STRINGIFY(UNIFORM_NAME));                          \
	}                                                                                        \
	virtual ~UNIFORM_NAME() noexcept = default;                                              \
public:                                                                                      \
	/**                                                                                      \
	 * @brief Set the value of the UNIFORM_NAME uniform in the shader program.               \
	 * @param [in] value The value to set.                                                   \
	 */                                                                                      \
	void GT_PASTE_TOGETHER(set, UNIFORM_NAME_WITH_FIRST_CAPITAL)                             \
		(UNIFORM_TYPE value) const noexcept                                                  \
	{                                                                                        \
		OpenGL::setUniform(mLocation, value);                                                \
	}                                                                                        \
};

GT_DEFINE_UNIFORM(const mat4f&, matrixPVM,                     MatrixPVM);
GT_DEFINE_UNIFORM(const mat4f&, matrixVM,                      MatrixVM);
GT_DEFINE_UNIFORM(const mat4f&, matrixPV,                      MatrixPV);
GT_DEFINE_UNIFORM(const mat4f&, matrixP,                       MatrixP);
GT_DEFINE_UNIFORM(const mat3f&, matrixN,                       MatrixN);

using Matrix4fArray = std::vector<mat4f, allocator<mat4f>>;
using Matrix3fArray = std::vector<mat3f>;

GT_DEFINE_UNIFORM(const Matrix4fArray&, matrixB, MatrixB);
GT_DEFINE_UNIFORM(const Matrix3fArray&, matrixBN, MatrixBN);

GT_DEFINE_UNIFORM(GLint,        instancedRendering,            InstancedRendering);
GT_DEFINE_UNIFORM(GLint,        hasTangentsAndBitangents,      HasTangentsAndBitangents);

GT_DEFINE_UNIFORM(GLint,        materialFlag,                  MaterialFlag);
GT_DEFINE_UNIFORM(GLint,        materialDiffuseTexture,        MaterialDiffuseTexture);
GT_DEFINE_UNIFORM(GLint,        materialSpecularTexture,       MaterialSpecularTexture);
GT_DEFINE_UNIFORM(GLint,        materialNormalTexture,         MaterialNormalTexture);
GT_DEFINE_UNIFORM(const vec4f&, materialDiffuseColor,          MaterialDiffuseColor);
GT_DEFINE_UNIFORM(const vec4f&, materialSpecularColor,         MaterialSpecularColor);

GT_DEFINE_UNIFORM(GLint,        depthTexture,                  DepthTexture);
GT_DEFINE_UNIFORM(GLfloat,      farPlane,                      FarPlane);

GT_DEFINE_UNIFORM(GLint,        geometryBufferPositionTexture, GeometryBufferPositionTexture);
GT_DEFINE_UNIFORM(GLint,        geometryBufferDiffuseTexture,  GeometryBufferDiffuseTexture);
GT_DEFINE_UNIFORM(GLint,        geometryBufferSpecularTexture, GeometryBufferSpecularTexture);
GT_DEFINE_UNIFORM(GLint,        geometryBufferNormalTexture,   GeometryBufferNormalTexture);

GT_DEFINE_UNIFORM(const vec2f&, viewportSize,                  ViewportSize);

GT_DEFINE_UNIFORM(const vec4f&, lightIntensity,                LightIntensity);
GT_DEFINE_UNIFORM(const vec4f&, lightAttenuation,              LightAttenuation);
GT_DEFINE_UNIFORM(const vec3f&, lightDirection,                LightDirection);
GT_DEFINE_UNIFORM(const vec3f&, lightPosition,                 LightPosition);
GT_DEFINE_UNIFORM(GLfloat,      lightCosineHalfAngle,          LightCosineHalfAngle);
GT_DEFINE_UNIFORM(GLint,        lightCastShadow,               LightCastShadow);
GT_DEFINE_UNIFORM(const mat4f&, lightShadowMatrix,             LightShadowMatrix);
GT_DEFINE_UNIFORM(GLint,        lightShadowDepthTexture,       LightShadowDepthTexture);

GT_DEFINE_UNIFORM(GLint,        debugFlag,                     DebugFlag);

GT_DEFINE_UNIFORM(GLint,        diffuseTexture,                DiffuseTexture);
GT_DEFINE_UNIFORM(const vec3f&, color,                         Color);
GT_DEFINE_UNIFORM(GLint,        glyphTexture,                  GlyphTexture);

GT_DEFINE_UNIFORM(const vec4f&, color4,                        Color4);
GT_DEFINE_UNIFORM(const vec2f&, position,                      Position);
GT_DEFINE_UNIFORM(const vec2f&, scale,                         Scale);
GT_DEFINE_UNIFORM(GLint,        withoutColorAlpha,             WithoutColorAlpha);
GT_DEFINE_UNIFORM(GLint,        hasGUITexture,                 HasGUITexture);
GT_DEFINE_UNIFORM(GLint,        GUITexture,                    GUITexture);

namespace Block {

#define GT_DEFINE_UNIFORM_BLOCK(UNIFORM_NAME, UNIFORM_NAME_WITH_FIRST_CAPITAL)               \
/** @brief Class that encapsulates a uniform block in a shader. */                           \
class UNIFORM_NAME : virtual public OpenGL::ShaderProgram                                    \
{                                                                                            \
private:                                                                                     \
	GLuint mIndex;                                                                           \
protected:                                                                                   \
	UNIFORM_NAME()                                                                           \
	{                                                                                        \
		mIndex = getUniformBlockIndex(GT_STRINGIFY(UNIFORM_NAME));                           \
	}                                                                                        \
	virtual ~UNIFORM_NAME() noexcept = default;                                              \
public:                                                                                      \
	/**                                                                                      \
	 * @brief Set the value of the UNIFORM_NAME uniform in the shader program.               \
	 * @param [in] value The value to set.                                                   \
	 */                                                                                      \
	inline GLuint GT_PASTE_TOGETHER(get, UNIFORM_NAME_WITH_FIRST_CAPITAL)() const noexcept   \
	{                                                                                        \
		return mIndex;                                                                       \
	}                                                                                        \
	inline void bindBuffer(const OpenGL::BufferObject& buffer) const noexcept                \
	{                                                                                        \
		glUniformBlockBinding(*this, mIndex, buffer);                                        \
	}                                                                                        \
};

GT_DEFINE_UNIFORM_BLOCK(Joint44, Joint44);
GT_DEFINE_UNIFORM_BLOCK(Joint33, Joint33);

} // namespace Block
} // namespace Uniform

/**
 * @brief Base class for a shader program. Defines the various
 * static methods common across all shaders.
 * @tparam DerivedShaderProgram The derived type.
 */
template <class DerivedShaderProgram>
class ShaderProgramBase
{
public:

	/**
	 * @brief Initialize this shader program.
	 * The Renderer takes care of that.
	 * @attention Never call this method yourself.
	 */
	inline static void initialize()
	{
		delete sInstance;
		sInstance = new DerivedShaderProgram();
	}

	/**
	 * @brief Release this shader.
	 * The Renderer takes care of that.
	 * @attention Never call this method yourself.
	 */
	inline static void release() noexcept
	{
		delete sInstance;
		sInstance = nullptr;
	}

	/**
	 * @brief Get a const reference to this shader program.
	 * @return A const reference to this shader program.
	 */
	inline static const DerivedShaderProgram& get() noexcept
	{
		return *sInstance;
	}
protected:

	/// Defaulted constructor.
	ShaderProgramBase() = default;

	/// Defaulted destructor.
	virtual ~ShaderProgramBase() noexcept = default;

private:

	static DerivedShaderProgram* sInstance;
};

//!@cond
template <class D> D* ShaderProgramBase<D>::sInstance = nullptr;
//!@endcond

/**
 * @brief Shader program for materials.
 */
class MaterialShaderProgram
: public ShaderProgramBase<MaterialShaderProgram>
, public Uniform::matrixPVM
, public Uniform::matrixVM
, public Uniform::matrixN
, public Uniform::matrixB
, public Uniform::matrixBN
// , public Uniform::instancedRendering
// , public Uniform::hasTangentsAndBitangents
, public Uniform::materialDiffuseColor
, public Uniform::materialSpecularColor
, public Uniform::materialDiffuseTexture
, public Uniform::materialSpecularTexture
, public Uniform::materialNormalTexture
, public Uniform::materialFlag
// , public Uniform::Block::Joint44
// , public Uniform::Block::Joint33
// , public Uniform::debugFlag
{
public:

	/// Defaulted constructor.
	MaterialShaderProgram();

	/// Defaulted destructor.
	virtual ~MaterialShaderProgram() noexcept = default;
};

class DepthBufferShaderProgram
: public ShaderProgramBase<DepthBufferShaderProgram>
, public Uniform::depthTexture
, public Uniform::viewportSize
, public Uniform::farPlane
{
public:
	DepthBufferShaderProgram();
	virtual ~DepthBufferShaderProgram() noexcept = default;
};

class ShadowShaderProgram
: public ShaderProgramBase<ShadowShaderProgram>
, public Uniform::matrixPVM
, public Uniform::instancedRendering
{
public:
	ShadowShaderProgram();
	virtual ~ShadowShaderProgram() noexcept = default;
};

class AmbientLightShaderProgram
: public ShaderProgramBase<AmbientLightShaderProgram>
, public Uniform::viewportSize
, public Uniform::geometryBufferDiffuseTexture
, public Uniform::lightIntensity
{
public:
	AmbientLightShaderProgram();
	virtual ~AmbientLightShaderProgram() noexcept = default;
};

/**
 * @brief Light pass shader for a directional light.
 */
class DirectionalLightShaderProgram
: public ShaderProgramBase<DirectionalLightShaderProgram>
, public Uniform::viewportSize
, public Uniform::geometryBufferPositionTexture
, public Uniform::geometryBufferDiffuseTexture
, public Uniform::geometryBufferSpecularTexture
, public Uniform::geometryBufferNormalTexture
, public Uniform::lightIntensity
, public Uniform::lightDirection
, public Uniform::lightCastShadow
, public Uniform::lightShadowMatrix
, public Uniform::lightShadowDepthTexture
{
public:
	DirectionalLightShaderProgram();
	virtual ~DirectionalLightShaderProgram() noexcept = default;
};

/**
 * @brief Light pass shader for a point light.
 */
class PointLightShaderProgram
: public ShaderProgramBase<PointLightShaderProgram>
, public Uniform::matrixPVM
, public Uniform::viewportSize
, public Uniform::geometryBufferPositionTexture
, public Uniform::geometryBufferDiffuseTexture
, public Uniform::geometryBufferSpecularTexture
, public Uniform::geometryBufferNormalTexture
, public Uniform::lightIntensity
, public Uniform::lightAttenuation
, public Uniform::lightPosition
#ifndef NDEBUG
, public Uniform::debugFlag
#endif
{
public:
	PointLightShaderProgram();
	virtual ~PointLightShaderProgram() noexcept = default;
};

/**
 * @brief Light pass shader for a spot light.
 */
class SpotLightShaderProgram
: public ShaderProgramBase<SpotLightShaderProgram>
, public Uniform::matrixPVM
, public Uniform::viewportSize
, public Uniform::geometryBufferPositionTexture
, public Uniform::geometryBufferDiffuseTexture
, public Uniform::geometryBufferSpecularTexture
, public Uniform::geometryBufferNormalTexture
, public Uniform::lightIntensity
, public Uniform::lightAttenuation
, public Uniform::lightDirection
, public Uniform::lightCosineHalfAngle
, public Uniform::lightPosition
, public Uniform::lightCastShadow
, public Uniform::lightShadowMatrix
, public Uniform::lightShadowDepthTexture
#ifndef NDEBUG
, public Uniform::debugFlag
#endif
{
public:
	SpotLightShaderProgram();
	virtual ~SpotLightShaderProgram() noexcept = default;
};

/**
 * @brief Shader for a skybox.
 */
class SkyboxShaderProgram
: public ShaderProgramBase<SkyboxShaderProgram>
, public Uniform::matrixPV
, public Uniform::diffuseTexture
{
public:
	SkyboxShaderProgram();
	virtual ~SkyboxShaderProgram() noexcept = default;
};

/**
 * @brief Simple shader for two-dimensional text.
 */
class FlatTextShaderProgram 
: public ShaderProgramBase<FlatTextShaderProgram>
, public Uniform::color
, public Uniform::glyphTexture
{
public:
	FlatTextShaderProgram();
	virtual ~FlatTextShaderProgram() noexcept = default;
};

class GUIShaderProgram
: public ShaderProgramBase<GUIShaderProgram>
, public Uniform::position
, public Uniform::scale
, public Uniform::color4
, public Uniform::hasGUITexture
, public Uniform::GUITexture
, public Uniform::withoutColorAlpha
{
public:
	GUIShaderProgram();
	virtual ~GUIShaderProgram() noexcept = default;
};

class SilhouetteShaderProgram
: public ShaderProgramBase<SilhouetteShaderProgram>
, public Uniform::matrixPVM
, public Uniform::lightPosition
, public Uniform::color
{
public:
	SilhouetteShaderProgram();
	virtual ~SilhouetteShaderProgram() noexcept = default;
};

class ShadowVolumeShaderProgram
: public ShaderProgramBase<ShadowVolumeShaderProgram>
, public Uniform::matrixPVM
, public Uniform::lightPosition
{
public:
	ShadowVolumeShaderProgram();
	virtual ~ShadowVolumeShaderProgram() noexcept = default;
};

class OctreeDebugShaderProgram
: public ShaderProgramBase<OctreeDebugShaderProgram>
, public Uniform::matrixPVM
, public Uniform::color
{
public:
	OctreeDebugShaderProgram();
	virtual ~OctreeDebugShaderProgram() noexcept = default;
};

} // namespace gintonic
