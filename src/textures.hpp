#ifndef textures_hpp
#define textures_hpp

#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>
#include "opengl.hpp"
#include "object.hpp"

namespace gintonic {
namespace opengl {

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
	\brief The constructor constructs sane and default values for the texture parameters.
	*/
	texture_parameters();
};

/*!
\brief Abstraction for 2D OpenGL textures.

You cannot directly instantiate this class. You need to use the factory mechanism.
That is, you request a texture2d with texture2d::request with the path to a filename.
This method returns an abstraction of a pointer. That is, it returns a texture2d::pointer.
The size of a texture2d::pointer really is the same size as a pointer, so do not worry about
size constraints. The reason to use this strategy is so that multiple handles to the same texture
use the same data underneath. This prevents unneccesary data duplication on the GPU.

The texture2d::pointer takes care of reference counting.
When the pointer gets destroyed by its destructor, the reference count of the texture
is decremented. When the reference count reaches zero, the texture is deleted from the GPU.

What follows is an example on how to obtain and instantiate a new texture2d.

\code
using namespace gintonic::opengl;
try
{
	texture2d::pointer tex = texture2d::request("path/to/texture.png");
	std::cout << "Texture width: " << tex->width();
	std::cout << "Texture height: " << tex->height();

	// ...

	tex->bind(GL_TEXTURE0);
	// Drawing ...

}
catch (const std::exception& e)
{
	std::cerr << "Could not fetch texture: " << e.what() << '\n';
}
\endcode
*/
class texture2d : public object<texture2d, boost::filesystem::path>
{
private:

	friend boost::flyweights::detail::optimized_key_value<key_type, texture2d, key_extractor>;

	void class_init();
	void init(const GLsizei width, const GLsizei height, const GLenum format, const GLenum type, const std::vector<char>& data);
	void init_tga_image(GLsizei& width, GLsizei& height, GLenum& format, GLenum& type, std::vector<char>& data);
	#ifdef BOOST_MSVC
	void init_wic_image(GLsizei& width, GLsizei& height, GLenum& format, GLenum& type, std::vector<char>& data);
	#elif defined APPLE
	void init_png_image(GLsizei& width, GLsizei& height, GLenum& format, GLenum& type, std::vector<char>& data);
	void init_jpeg_image(GLsizei& width, GLsizei& height, GLenum& format, GLenum& type, std::vector<char>& data);
	#elif defined __linux__
	void init_png_image(GLsizei& width, GLsizei& height, GLenum& format, GLenum& type, std::vector<char>& data);
	void init_jpeg_image(GLsizei& width, GLsizei& height, GLenum& format, GLenum& type, std::vector<char>& data);
	#else
	#error Platform not supported.
	#endif
	GLuint m_buffer;

	texture2d(const key_type& image_filename);
	texture2d(key_type&& image_filename);

	friend class boost::serialization::access;
	template <class Archive> void save(Archive& ar, const unsigned int version) const
	{
		ar & boost::serialization::base_object<object<texture2d, key_type>>(*this);
	}
	template <class Archive> void load(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<object<texture2d, key_type>>(*this);
		init();
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER()

	static void release();

public:

	/*!
	\brief The destructor.
	*/
	virtual ~texture2d() BOOST_NOEXCEPT_OR_NOTHROW;

	static void init();

	texture2d(const texture2d&) = delete;
	texture2d& operator = (const texture2d&) = delete;

	texture2d(texture2d&& other) BOOST_NOEXCEPT_OR_NOTHROW;
	texture2d& operator = (texture2d&& other) BOOST_NOEXCEPT_OR_NOTHROW;

	struct error : virtual exception {};
	struct empty_filename_error : virtual error {};
	struct not_a_regular_file_error : virtual error {};
	struct file_extension_error : virtual error {};
	struct handle_error : virtual error {};
	struct unknown_tga_format_error : virtual error {};
	struct png_ran_out_of_memory_error : virtual error {};
	struct png_read_error : virtual error {};
	struct png_invalid_signature_error : virtual error {};
	struct png_unknown_color_format_error : virtual error {};
	struct jpeg_read_error : virtual error {};

	#ifdef BOOST_MSVC
	struct wic_error : virtual error {};
	struct wic_initialization_error : virtual error {};
	typedef boost::error_info<struct tag_wic_errorcode, HRESULT> errinfo_wic;
	#endif

	/*!
	\brief Static member to define the texture parameters when a new
	texture is constructed.

	I chose this over passing a texture_parameter
	structure to the constructor every time a new texture is constructed
	because I feel that in practise the texture parameters are set at the
	start of the program and then kept that way until the end.
	*/
	static texture_parameters parameter;
	


	/*!
	\brief Bind this texture2d.

	Effectively calls glBindTexture and glActiveTexture.

	**Warning.** The active_texture_unit argument is not one of
	* `GL_TEXTURE0`
	* `GL_TEXTURE1`
	* `GL_TEXTURE2`
	* Etcetera,

	but rather a number corresponding to the texture unit.

	\param [in] active_texture_unit The texture unit to set for this texture.
	*/
	void bind(const GLint active_texture_unit) const BOOST_NOEXCEPT_OR_NOTHROW;

	/*!
	\brief The width of the texture.

	This method calls the OpenGL API, so if you need this information
	often it is recommended to store it in memory somewhere.

	\param [in] level The mipmap level. The default is 0 (the original size).
	\return The width of this texture at the specified mipmap level.
	*/
	GLint width(const GLint level = 0) const BOOST_NOEXCEPT_OR_NOTHROW;

	/*!
	\brief The height of the texture.

	This method calls the OpenGL API, so if you need this information
	often it is recommended to store it in memory somewhere.

	\param [in] level The mipmap level. The default is 0 (the original size).
	\return The height of this texture at the specified mipmap level.
	*/
	GLint height(const GLint level = 0) const BOOST_NOEXCEPT_OR_NOTHROW;

	/*!
	\brief The depth of the texture.

	This method calls the OpenGL API, so if you need this information
	often it is recommended to store it in memory somewhere.

	For 2D textures, this will always be 1.

	\param [in] level The mipmap level. The default is 0 (the original size).
	\return The depth of this texture at the specified mipmap level.
	*/
	GLint depth(const GLint level = 0) const BOOST_NOEXCEPT_OR_NOTHROW;

	/*!
	\brief Obtain the internal format of this texture at the specified mipmap level.
	\param[in] level The mipmap level. The default is 0.
	\return The internal format of this texture at the specified mipmap level.
	*/
	GLint internal_format(const GLint level = 0) const BOOST_NOEXCEPT_OR_NOTHROW;

	/*!
	\brief Returns `GL_TRUE` if the texture is stored in a compressed format, otherwise returns `GL_FALSE`.
	\param [in] level The mipmap level. The default is 0.
	\return Returns `GL_TRUE` if the texture is stored in a compressed format, otherwise returns `GL_FALSE`.
	*/
	bool is_compressed(const GLint level = 0) const BOOST_NOEXCEPT_OR_NOTHROW;
	GLint format(const GLint level = 0) const BOOST_NOEXCEPT_OR_NOTHROW;
	GLint compressed_size(const GLint level = 0) const BOOST_NOEXCEPT_OR_NOTHROW;
	// GLint buffer_offset() const BOOST_NOEXCEPT_OR_NOTHROW;
	// GLint buffer_size() const BOOST_NOEXCEPT_OR_NOTHROW;

	// Cast the unique ID that this texture2d handles to a plain GLuint, if needed.
	inline operator GLuint() const BOOST_NOEXCEPT_OR_NOTHROW { return m_buffer; }
};

} // end of namespace opengl
} // end of namespace gintonic



// namespace boost {
// 	namespace serialization {
// 		template <class Archive>
// 		inline void save_construct_data(Archive & ar, const gintonic::opengl::texture2d* t, const unsigned int version)
// 		{
// 			// save data required to construct instance
// 			ar & t->key();
// 		}
// 		template <class Archive>
// 		inline void load_construct_data(Archive& ar, gintonic::opengl::texture2d* t, const unsigned int version)
// 		{
// 			// retrieve data from archive required to construct new instance
// 			boost::filesystem::path the_key;
// 			ar & the_key;
// 			// invoke inplace constructor to initialize instance of A
// 			::new(t)gintonic::opengl::texture2d(std::move(the_key));
// 		}
// 	}
// }

BOOST_CLASS_VERSION(gintonic::opengl::texture2d, 1)

#endif