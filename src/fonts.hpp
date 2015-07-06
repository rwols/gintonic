#ifndef gintonic_fonts_hpp
#define gintonic_fonts_hpp

#include "opengl.hpp"
#include "object.hpp"

#ifdef BOOST_MSVC
#include <Dwrite.h>
#elif defined(__linux__)
#include <ft2build.h>
#include FT_FREETYPE_H
#endif

namespace gintonic {

	class font : public object<font, boost::filesystem::path>
	{
	public:
		void draw(const std::string& text, vec2f position, const vec2f& scale) const BOOST_NOEXCEPT_OR_NOTHROW;
		virtual ~font();

		static void init();

		// void set_size(const int pixels);

		struct error : virtual exception {};
		struct font_not_found : virtual error {};

	private:

		friend boost::flyweights::detail::optimized_key_value<key_type, font, key_extractor>;

		font(const key_type&);
		font(key_type&&);
		void init_class();

		static void release();

		#ifdef BOOST_MSVC
		IDWriteFactory* m_factory;
		IDWriteTextFormat* m_text_format;
		#elif defined (__linux__)
		FT_Face m_face;
		#endif

		GLuint m_tex;
		opengl::vertex_array_object m_vao;
		opengl::buffer_object m_vbo;
	};

} // namespace gintonic

#endif