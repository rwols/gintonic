#ifndef gintonic_fonts_hpp
#define gintonic_fonts_hpp

#include "opengl.hpp"
#include "object.hpp"
#include <iosfwd>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/stream.hpp>

namespace gintonic {

	class font : public object<font, std::pair<boost::filesystem::path, int>>
	{
	public:

		typedef char char_type;

		struct error : virtual exception {};
		struct font_not_found : virtual error {};

		static void init();
		virtual ~font();
		font(const font&) = delete;
		font& operator = (const font&) = delete;

		void draw(const char* text, const std::size_t length, vec2f position, const vec2f& scale) const BOOST_NOEXCEPT_OR_NOTHROW;
		void draw(const std::string& text, vec2f position, const vec2f& scale) const BOOST_NOEXCEPT_OR_NOTHROW;
		void draw(const char* text, const std::size_t length, vec2f position) const BOOST_NOEXCEPT_OR_NOTHROW;
		void draw(const std::string& text, vec2f position) const BOOST_NOEXCEPT_OR_NOTHROW;

	private:

		friend boost::flyweights::detail::optimized_key_value<key_type, font, key_extractor>;

		font(const key_type&);
		font(key_type&&);
		font(font&&);
		font& operator = (font&&);

		void init_class();

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

			std::streamsize write(const char* text, const std::streamsize length) const BOOST_NOEXCEPT_OR_NOTHROW;
		};
	} // namespace detail

	typedef boost::iostreams::stream<detail::fontstream> fontstream;

} // namespace gintonic

#endif