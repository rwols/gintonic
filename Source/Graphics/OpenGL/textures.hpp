/**
 * @file textures.hpp
 * @brief Defines various texture classes.
 * @author Raoul Wols
 */

#pragma once

#include "TextureObject.hpp"
#include "../../Foundation/filesystem.hpp"
#include <array>

namespace gintonic {
namespace OpenGL {

/*!
\brief A structure carrying information about texture parameters.

Since there are so many parameters for a texture to set, there's a dedicated
structure to set all the parameters and pass that to one of the methods of
the texture2d class.

The texture_parameters structure only has public member variables, it is a
simple plain old data type.
*/
struct texture_parameters
{
	// Specifies the mode used to read from depth-stencil format textures.
	// depth_stencil_mode must be one of GL_DEPTH_COMPONENT or GL_STENCIL_COMPONENT.
	// If the depth stencil mode is GL_DEPTH_COMPONENT, then reads from depth-stencil
	// format textures will return the depth component of the texel in Rt and the stencil
	// component will be discarded. If the depth stencil mode is GL_STENCIL_COMPONENT
	// then the stencil component is returned in Rt and the depth component is discarded.
	// The initial value is GL_DEPTH_COMPONENT.
	//GLenum depth_stencil_mode;

	
	/*!
	\brief Specifies the number of mipmaps used.

	\sa texture_parameters::base_level
	\sa texture_parameters::minimum_lod
	\sa texture_parameters::maximum_lod
	\sa texture_parameters::max_level
	*/
	GLint mipmaps;

	/*!
	\brief Specifies the index of the lowest defined mipmap level.
	
	This is an integer value. The initial value is 0.

	\sa texture_parameters::mipmaps
	\sa texture_parameters::minimum_lod
	\sa texture_parameters::maximum_lod
	\sa texture_parameters::max_level
	*/
	GLint base_level;

	/*!
	\brief The data in params specifies four values that define the border values that
	should be used for border texels.

	If a texel is sampled from the border of
	the texture, the values of `GL_TEXTURE_BORDER_COLOR` are interpreted as an RGBA
	color to match the texture's internal format and substituted for the non-existent
	texel data. If the texture contains depth components, the first component of
	`GL_TEXTURE_BORDER_COLOR` is interpreted as a depth value. The initial value is 
	`(0.0, 0.0, 0.0, 0.0)`.
	*/
	std::array<GLfloat, 4> border_color;

	/*!
	\brief Specifies the comparison operator used when texture_parameters::compare_mode
	is set to `GL_COMPARE_REF_TO_TEXTURE`.

	Permissible values are
	- `GL_LEQUAL`, 
	- `GL_GEQUAL`,
	- `GL_LESS`,
	- `GL_GREATER`,
	- `GL_EQUAL`,
	- `GL_NOTEQUAL`,
	- `GL_ALWAYS` or
	- `GL_NEVER`.
	*/
	GLint compare_func;

	/*!
	\brief Specifies the texture comparison mode for currently bound depth textures.
	
	That is, a texture whose internal format is `GL_DEPTH_COMPONENT_*`;
	(see [glTexImage2D](https://www.opengl.org/sdk/docs/man/html/glTexImage2D.xhtml))
	Permissible values are `GL_COMPARE_REF_TO_TEXTURE` or `GL_NONE`.
	*/
	GLint compare_mode;

	/*!
	\brief specifies a fixed bias value that is to be added to the level-of-detail
	parameter for the texture before texture sampling.

	The specified value is added to the shader-supplied bias value (if any) and
	subsequently clamped into the implementation-defined range `[−biasmax,biasmax]`, where
	biasmax is the value of the implementation defined constant
	`GL_MAX_TEXTURE_LOD_BIAS`. The initial value is `0.0`.
	*/
	GLfloat lod_bias;

	/*!
	\brief The texture minifying function is used whenever the level-of-detail function
	used when sampling from the texture determines that the texture should be
	minified.

	There are six defined minifying functions. Two of them use either
	the nearest texture elements or a weighted average of multiple texture elements
	to compute the texture value. The other four use mipmaps.
	minification_filter supplies a function for minifying the texture as one of the
	following:
	- `GL_NEAREST`,
	- `GL_LINEAR`,
	- `GL_NEAREST_MIPMAP_NEAREST`,
	- `GL_LINEAR_MIPMAP_NEAREST`,
	- `GL_NEAREST_MIPMAP_LINEAR` or
	- `GL_LINEAR_MIPMAP_LINEAR`.

	\sa texture_parameters::magnification_filter
	*/
	GLint minification_filter;

	/*!
	\brief The texture magnification function is used whenever the level-of-detail function
	used when sampling from the texture determines that the texture should be magified.

	It sets the texture magnification function to either `GL_NEAREST` or `GL_LINEAR`.
	`GL_NEAREST` is generally faster than `GL_LINEAR`, but it can produce
	textured images with sharper edges because the transition between texture elements
	is not as smooth. The initial value is `GL_LINEAR`.

	\sa texture_parameters::minification_filter
	*/
	GLint magnification_filter;

	/*!
	\brief Sets the minimum level-of-detail parameter.

	This floating-point value limits the selection of highest resolution mipmap
	(lowest mipmap level). The initial value is -1000.

	\sa texture_parameters::mipmaps
	\sa texture_parameters::maximum_lod
	\sa texture_parameters::max_level
	*/
	GLfloat minimum_lod;

	/*!
	\brief Sets the maximum level-of-detail parameter.

	This floating-point value limits the selection of the lowest resolution mipmap
	(highest mipmap level). The initial value is 1000.
	*/
	GLfloat maximum_lod;

	/*!
	\brief Sets the index of the highest defined mipmap level.

	This is an integer value. The initial value is 1000.
	*/
	GLint max_level;

	/*!
	\brief Sets the swizzle that will be applied to the r, g, b or a component of a texel before it is
	returned to the shader.

	Valid values for the params are `GL_RED`, `GL_GREEN`, `GL_BLUE`, `GL_ALPHA`, `GL_ZERO` and `GL_ONE`.
	If `texture_parameters::swizzle[0]` is `GL_RED`, the value for `r` will be taken from the first
	channel of the fetched texel. If `texture_parameters::swizzle[0]` is `GL_GREEN`, the value for `r`
	will be taken from the second channel of the fetched texel. If `texture_parameters::swizzle[0]` is
	`GL_BLUE`, the value for `r` will be taken from the third channel of the fetched texel. If
	`texture_parameters::swizzle[0]` is `GL_ALPHA`, the value for `r` will be taken from the fourth
	channel of the fetched texel. If `texture_parameters::swizzle[0]` is `GL_ZERO`, the value
	for `r` will be subtituted with `0.0`. If `texture_parameters::swizzle[0]` is `GL_ONE`, the value
	for `r` will be subtituted with `1.0`. The initial value is `GL_RED`.
	Analogous for `texture_parameters::swizzle[1]`, `texture_parameters::swizzle[2]` and
	`texture_parameters::swizzle[3]`.
	*/
	std::array<GLint, 4> swizzle;

	/*!
	\brief Sets the wrap parameter for texture coordinate s to either `GL_CLAMP_TO_EDGE`,
	`GL_CLAMP_TO_BORDER`, `GL_MIRRORED_REPEAT`, `GL_REPEAT`, or `GL_MIRROR_CLAMP_TO_EDGE`.

	`GL_CLAMP_TO_EDGE` causes the `s` coordinates to be clamped to the range `[12N,1−12N]`, where
	`N` is the size of the texture in the direction of clamping. `GL_CLAMP_TO_BORDER` evaluates
	the `s` coordinates in a similar manner to `GL_CLAMP_TO_EDGE`. However, in cases where clamping
	would have occurred in `GL_CLAMP_TO_EDGE` mode, the fetched texel data is substituted with the values
	specified by `GL_TEXTURE_BORDER_COLOR`. `GL_REPEAT` causes the integer part of the `s` coordinate to
	be ignored; OpenGL uses only the fractional part, thereby creating a repeating pattern.
	`GL_MIRRORED_REPEAT` causes the `s` coordinate to be set to the fractional part of the texture
	coordinate if the integer part of `s` is even; if the integer part of `s` is odd, then the `s`
	texture coordinate is set to `1−frac(s)`, where `frac(s)` represents the fractional part of `s`.
	`GL_MIRROR_CLAMP_TO_EDGE` causes the the `s` coordinate to be repeated as for `GL_MIRRORED_REPEAT`
	for one reptition of the texture, at which point the coordinate to be clamped as in
	`GL_CLAMP_TO_EDGE`. Initially, `GL_TEXTURE_WRAP_S` is set to `GL_REPEAT`.
	Analogous for the `t` and `r` component.
	*/
	std::array<GLint, 3> wrap;

	/*!
	\brief The constructor constructs sane and default values for the texture 
	parameters.
	*/
	texture_parameters();
};

/**
 * @brief Base class for textures.
 * 
 * @details The base class houses a few useful protected methods so that
 * derivers can easily import image files from disk.
 */
class texture : public std::enable_shared_from_this<texture>
{
public:

	/**
	 * @brief Initialize textures.
	 * @details This static method needs to be called after initializing the
	 * renderer in order to use textures correctly.
	 */
	static void init();

	/**
	 * @brief Texture parameters.
	 * @deprecated I will probably remove this later. It's pointless.
	 */
	static texture_parameters parameter;

	/**
	 * @brief Bind the texture with at the given texture unit index.
	 * @param texture_unit The texture unit index.
	 * @note In OpenGL, you are probably used to seeing GL_TEXTURE0,
	 * GL_TEXTURE1, GL_TEXTURE2	and so on as texture units. But here we start
	 * from the number 0. So, GL_TEXTURE0 corresponds to 0, GL_TEXTURE1
	 * corresponds to 1, GL_TEXTURE2 corresponds to 2, and so on. This is to
	 * unify the calling convention for setting up the sampler2D uniform in
	 * shaders.
	 */
	virtual void bind(const GLint texture_unit) const noexcept = 0;

	/// Destructor.
	virtual ~texture();

	/// Copy constructor.
	texture(const texture&) = default;

	/// Copy assignment operator.
	texture& operator = (const texture&) = default;

	/// Move constructor.
	texture(texture&&);

	/// Move assignment operator.
	texture& operator = (texture&&);

protected:

	/// Default constructor (does nothing).
	texture() = default;

	/**
	 * @brief Initialize a TGA image from disk.
	 * 
	 * @param filename The filepath to the TGA image.
	 * @param width Will be set to the width of the image.
	 * @param height Will be set to the height of the image.
	 * @param format Will be set to the format of the image.
	 * @param type Will be set to the type of the image.
	 * @param data Will be filled with the contents of the image.
	 * @exception May throw an exception.
	 */
	static void init_tga_image(
		const boost::filesystem::path& filename, 
		GLsizei& width, 
		GLsizei& height, 
		GLenum& format, 
		GLenum& type, 
		std::vector<char>& data);
	
	#ifdef BOOST_MSVC

	/**
	 * @brief Initialize most image formats from disk.
	 * 
	 * @param filename The filepath to the TGA image.
	 * @param width Will be set to the width of the image.
	 * @param height Will be set to the height of the image.
	 * @param format Will be set to the format of the image.
	 * @param type Will be set to the type of the image.
	 * @param data Will be filled with the contents of the image.
	 * @exception May throw an exception.
	 */
	static void init_wic_image(
		const boost::filesystem::path& filename, 
		GLsizei& width, 
		GLsizei& height, 
		GLenum& format, 
		GLenum& type, 
		std::vector<char>& data);

	#elif defined __APPLE__
	
	/**
	 * @brief Initialize a PNG image formats from disk.
	 * 
	 * @param filename The filepath to the TGA image.
	 * @param width Will be set to the width of the image.
	 * @param height Will be set to the height of the image.
	 * @param format Will be set to the format of the image.
	 * @param type Will be set to the type of the image.
	 * @param data Will be filled with the contents of the image.
	 * @exception May throw an exception.
	 */
	static void init_png_image(
		const boost::filesystem::path& filename, 
		GLsizei& width, 
		GLsizei& height, 
		GLenum& format, 
		GLenum& type, 
		std::vector<char>& data);

	/**
	 * @brief Initialize a JPEG image formats from disk.
	 * 
	 * @param filename The filepath to the TGA image.
	 * @param width Will be set to the width of the image.
	 * @param height Will be set to the height of the image.
	 * @param format Will be set to the format of the image.
	 * @param type Will be set to the type of the image.
	 * @param data Will be filled with the contents of the image.
	 * @exception May throw an exception.
	 */
	static void init_jpeg_image(
		const boost::filesystem::path& filename, 
		GLsizei& width, 
		GLsizei& height, 
		GLenum& format, 
		GLenum& type, 
		std::vector<char>& data);

	#elif defined __linux__
	
	/**
	 * @brief Initialize a PNG image formats from disk.
	 * 
	 * @param filename The filepath to the TGA image.
	 * @param width Will be set to the width of the image.
	 * @param height Will be set to the height of the image.
	 * @param format Will be set to the format of the image.
	 * @param type Will be set to the type of the image.
	 * @param data Will be filled with the contents of the image.
	 * @exception May throw an exception.
	 */
	static void init_png_image(
		const boost::filesystem::path& filename, 
		GLsizei& width, 
		GLsizei& height, 
		GLenum& format, 
		GLenum& type, 
		std::vector<char>& data);

	/**
	 * @brief Initialize a JPEG image formats from disk.
	 * 
	 * @param filename The filepath to the TGA image.
	 * @param width Will be set to the width of the image.
	 * @param height Will be set to the height of the image.
	 * @param format Will be set to the format of the image.
	 * @param type Will be set to the type of the image.
	 * @param data Will be filled with the contents of the image.
	 * @exception May throw an exception.
	 */
	static void init_jpeg_image(
		const boost::filesystem::path& filename, 
		GLsizei& width, 
		GLsizei& height, 
		GLenum& format, 
		GLenum& type, 
		std::vector<char>& data);

	#else
	
	#error Platform not supported.
	
	#endif
	
	/**
	 * @brief Initialize most image formats from disk.
	 * 
	 * @param filename The filepath to the TGA image.
	 * @param width Will be set to the width of the image.
	 * @param height Will be set to the height of the image.
	 * @param format Will be set to the format of the image.
	 * @param type Will be set to the type of the image.
	 * @param data Will be filled with the contents of the image.
	 * @exception May throw an exception.
	 */
	static void init_generic_image(
		const boost::filesystem::path& filename, 
		GLsizei& width, 
		GLsizei& height, 
		GLenum& format, 
		GLenum& type, 
		std::vector<char>& data);

private:

	static void release();
};

/**
 * @brief Your basic two-dimensional texture object.
 */
class texture2d : public texture
{
public:

	/**
	 * @brief Constructor.
	 * @param filename Initialize a texture2d with the given image located
	 * at the filepath.
	 * @exception exception Can throw an exception.
	 */
	texture2d(const boost::filesystem::path& filename);

	/// Destructor.
	virtual ~texture2d();

	/// Copy constructor.
	texture2d(const texture2d&) = default;

	/// Copy assignment operator.
	texture2d& operator = (const texture2d&) = default;

	/// Move constructor.
	texture2d(texture2d&&);

	/// Move assignment operator.
	texture2d& operator = (texture2d&&);

	virtual void bind(const GLint texture_unit) const 
		noexcept final;

	/**
	 * @brief Get the width of the texture.
	 * @details The texture needs to be bound before calling this method.
	 * 
	 * @param level The mipmap level.
	 * @return The width of the texture at the given mipmap level.
	 */
	GLint width(const GLint level = 0) const noexcept;

	/**
	 * @brief Get the height of the texture.
	 * @details The texture needs to be bound before calling this method.
	 * 
	 * @param level The mipmap level.
	 * @return The height of the texture at the given mipmap level.
	 */
	GLint height(const GLint level = 0) const noexcept;

	/**
	 * @brief Get the depth of the texture.
	 * @details The texture needs to be bound before calling this method.
	 * 
	 * @param level The mipmap level.
	 * @return The depth of the texture at the given mipmap level.
	 * 
	 * @deprecated The depth of a two-dimensional texture doesn't make any
	 * sense.
	 */
	GLint depth(const GLint level = 0) const noexcept;

	/**
	 * @brief Get the internal format of the texture.
	 * @details The texture needs to be bound before calling this method.
	 * 
	 * @param level The mipmap level.
	 * @return The internal format of the texture at the given mipmap level.
	 */
	GLint internal_format(const GLint level = 0) const 
		noexcept;

	/**
	 * @brief Check wether this texture2d is compressed.
	 * @details The texture needs to be bound before calling this method.
	 * 
	 * @param level The mipmap level.
	 * @return True if the texture2d at the given mipmap level is compressed,
	 * false otherwise.
	 */
	bool is_compressed(const GLint level = 0) const noexcept;

	/**
	 * @brief Get the format of the texture.
	 * @details The texture needs to be bound before calling this method.
	 * 
	 * @param level The mipmap level.
	 * @return The format of the texture at the given mipmap level.
	 */
	GLint format(const GLint level = 0) const noexcept;

	/**
	 * @brief Get the compressed size of the texture.
	 * @details The texture needs to be bound before calling this method.
	 * 
	 * @param level The mipmap level.
	 * @return The compressed size of the texture at the given mipmap level.
	 */
	GLint compressed_size(const GLint level = 0) const 
		noexcept;

	/// Get the underlying OpenGL handle with a static_cast.
	inline operator GLuint() const noexcept { return m_tex; }

private:

	TextureObject m_tex;
};

/**
 * @brief Your basic cube texture.
 * 
 * @details A cube has six sides, so a cube texture needs six image files.
 */
class cube_texture : public texture
{
public:

	/**
	 * @brief Constructor.
	 * 
	 * @param positive_X_file The +X side.
	 * @param negative_X_file The -X side.
	 * @param positive_Y_file The +Y side.
	 * @param negative_Y_file The -Y side.
	 * @param positive_Z_file The +Z side.
	 * @param negative_Z_file The -Z side.
	 */
	cube_texture(
		const boost::filesystem::path& positive_X_file,
		const boost::filesystem::path& negative_X_file,
		const boost::filesystem::path& positive_Y_file,
		const boost::filesystem::path& negative_Y_file,
		const boost::filesystem::path& positive_Z_file,
		const boost::filesystem::path& negative_Z_file);

	/// Destructor.
	virtual ~cube_texture();

	/// Copy constructor.
	cube_texture(const cube_texture&) = default;

	/// Copy assignment operator.
	cube_texture& operator = (const cube_texture&) = default;

	/// Move constructor.
	cube_texture(cube_texture&&);

	/// Move assignment operator.
	cube_texture& operator = (cube_texture&&);

	virtual void bind(const GLint texture_unit) const 
		noexcept final;

private:

	TextureObject m_tex;
};

} // end of namespace OpenGL
} // end of namespace gintonic
