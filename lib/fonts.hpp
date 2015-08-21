#ifndef gintonic_fonts_hpp
#define gintonic_fonts_hpp

#include "opengl/buffer_object.hpp"
#include "opengl/vertex_array_object.hpp"
#include "object.hpp"
#include "tuple.hpp"
#include "filesystem.hpp"
#include "vec2f.hpp"
#include <iosfwd>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/stream.hpp>

namespace gintonic {

	/* Use the flyweight mechanism to obtain a font. Use the fontstream class
	 * to actually render text.
	 */
	class font 
	: public object<font, std::tuple<boost::filesystem::path, int>>
	{
	public:

		typedef char char_type;

		static void init();
		virtual ~font();
		font(const font&) = delete;
		font& operator = (const font&) = delete;

		void draw(
			const char* text,         // The text to draw
			const std::size_t length, // The length of the text
			const vec2f& position,    // The position in clip coordinates
			const vec2f& scale)       // The scale (both x and y direction)
		const BOOST_NOEXCEPT_OR_NOTHROW;

		void draw(
			const std::string& text, // The text to draw
			const vec2f& position,   // The position in clip coordinates
			const vec2f& scale)      // The scale (both x and y direction)
		const BOOST_NOEXCEPT_OR_NOTHROW;

		void draw(
			const char* text,         // The text to draw
			const std::size_t length, // The length of the text
			const vec2f& position)    // The position in clip coordinates
		const BOOST_NOEXCEPT_OR_NOTHROW;

		//
		void draw(
			const std::string& text, // The text to draw 
			const vec2f& position)   // The position in clip coordinates
		const BOOST_NOEXCEPT_OR_NOTHROW;

		font(font&&);
		font& operator = (font&&);

	private:

		font(const key_type&);
		font(key_type&&);

		friend boost::flyweights::detail::optimized_key_value
		<
			key_type, 
			font, 
			key_extractor
		>;

		virtual void construct_from_key() final;

		static void release();

		struct character_info
		{
			int16_t ax; // advance x
			int16_t ay; // advance y
			int16_t bw; // bitmap width
			int16_t bh; // bitmap height
			int16_t bl; // bitmap left
			int16_t bt; // bitmap top
			GLfloat tx; // x offset of glyph in texture coordinates
		} m_c[96];

		GLsizei m_atlas_width, m_atlas_height;
		
		GLuint m_tex;
		opengl::vertex_array_object m_vao;
		opengl::buffer_object m_vbo;
	};

	namespace detail
	{
		class fontstream
		{
		public:

			typedef char char_type;
			typedef boost::iostreams::sink_tag category;

			fontstream(const font::flyweight& f);
			fontstream(font::flyweight&& f);

			font::flyweight underlying_font;

			vec2f scale;
			vec2f position;

			std::streamsize write(
				const char* text, 
				const std::streamsize length) 
			const BOOST_NOEXCEPT_OR_NOTHROW;

		};
	} // namespace detail

	typedef boost::iostreams::stream<detail::fontstream> fontstream;

} // namespace gintonic

#endif