#ifndef gintonic_fonts_hpp
#define gintonic_fonts_hpp

#include "opengl.hpp"
#include "object.hpp"

#ifdef BOOST_MSVC
#include <Dwrite.h>
#endif

namespace gintonic {

	class font : public object<font, boost::filesystem::path>
	{
	public:
		void draw(const std::string& text, const vec3f& position);
		virtual ~font();

		struct error : virtual exception {};
		struct font_not_found : virtual error {};

	private:

		friend boost::flyweights::detail::optimized_key_value<key_type, font, key_extractor>;

		font(const key_type&);
		font(key_type&&);
		void init_class();

		#ifdef BOOST_MSVC
		IDWriteFactory* m_factory;
		IDWriteTextFormat* m_text_format;
		#endif
	};

} // namespace gintonic

#endif