/**
 * @file fonts.hpp
 * @author Raoul Wols
 */

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

	/**
	 * @brief A font.
	 */
	class font 
	: public object<font, std::tuple<boost::filesystem::path, int>>
	{
	public:

		/**
		 * @brief The character type. No widechar support.
		 */
		typedef char char_type;

		/**
		 * @brief Initialize fonts. Must be called before using any font.
		 */
		static void init();

		/// Destructor.
		virtual ~font();

		/// You cannot copy fonts.
		font(const font&) = delete;

		/// You cannot copy fonts.
		font& operator = (const font&) = delete;

		/**
		 * @brief Draw this font.
		 * 
		 * @details Some shader must be activated before calling this method.
		 * 
		 * @param text The text to render.
		 * @param length The length of the text.
		 * @param position The position, in `CLIP` space coordinates, of where
		 * to start rendering the text.
		 * @param scale The scale of the text.
		 */
		void draw(
			const char* text,
			const std::size_t length,
			const vec2f& position,
			const vec2f& scale)
		const BOOST_NOEXCEPT_OR_NOTHROW;

		/**
		 * @brief Draw this font.
		 * 
		 * @details Some shader must be activated before calling this method.
		 * 
		 * @param text The text to render.
		 * @param position The position, in `CLIP` space coordinates, of where
		 * to start rendering the text.
		 * @param scale The scale of the text.
		 */
		void draw(
			const std::string& text,
			const vec2f& position,
			const vec2f& scale)
		const BOOST_NOEXCEPT_OR_NOTHROW;

		/**
		 * @brief Draw this font.
		 * 
		 * @details Some shader must be activated before calling this method.
		 * The scale is chosen such that it corresponds to the point size of
		 * this font.
		 * 
		 * @param text The text to render.
		 * @param length The length of the text.
		 * @param position The position, in `CLIP` space coordinates, of where
		 * to start rendering the text.
		 */
		void draw(
			const char* text,
			const std::size_t length,
			const vec2f& position)
		const BOOST_NOEXCEPT_OR_NOTHROW;

		/**
		 * @brief Draw this font.
		 * 
		 * @details Some shader must be activated before calling this method.
		 * The scale is chosen such that it corresponds to the point size of
		 * this font.
		 * 
		 * @param text The text to render.
		 * @param position The position, in `CLIP` space coordinates, of where
		 * to start rendering the text.
		 */
		void draw(
			const std::string& text,
			const vec2f& position)
		const BOOST_NOEXCEPT_OR_NOTHROW;

		/// Move constructor.
		font(font&&);

		/// Move assignment operator.
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

	//!@cond
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
	//!@endcond

	/**
	 * @brief A font stream.
	 */
	typedef boost::iostreams::stream<detail::fontstream> fontstream;

} // namespace gintonic

#endif