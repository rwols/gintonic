/**
 * @file fonts.hpp
 * @brief Defines a font class and a fontstream class. You usually don't use
 * fonts directly, but rather use fontstreams.
 * @author Raoul Wols
 */

#pragma once

#include "../Foundation/Object.hpp"
#include "../Foundation/tuple.hpp"
#include "../Foundation/filesystem.hpp"

#include "../Math/vec2f.hpp"

#include "OpenGL/TextureObject.hpp"
#include "OpenGL/BufferObject.hpp"
#include "OpenGL/VertexArrayObject.hpp"

#include <iosfwd>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/stream.hpp>

namespace gintonic {

	/**
	 * @brief A font.
	 */
	class Font : public Object<Font, boost::filesystem::path>
	{
	public:

		/**
		 * @brief The character type. No widechar support.
		 */
		typedef char char_type;

		/**
		 * @brief Initialize fonts. Must be called before using any font.
		 */
		// static void init();

		Font(const char* filename, const unsigned int pointsize);
		Font(const std::string& filename, const unsigned int pointsize);
		Font(boost::filesystem::path filename, const unsigned int pointsize);

		inline unsigned int getPointSize() const noexcept
		{
			return mPointSize;
		}

		/// Destructor.
		virtual ~Font() = default;

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
			const char*        text,
			const std::size_t  length,
			const vec2f&       position,
			const vec2f&       scale)
		const noexcept;

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
			const vec2f&       position,
			const vec2f&       scale)
		const noexcept;

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
			const char*        text,
			const std::size_t  length,
			const vec2f&       position)
		const noexcept;

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
			const vec2f&       position)
		const noexcept;

	private:

		// //!@cond
		// // We need to give boost::flyweights access to this class.
		// friend boost::flyweights::detail::optimized_key_value
		// <
		// 	key_type, 
		// 	font, 
		// 	key_extractor
		// >;
		// //!@endcond

		// virtual void construct_from_key() final;

		// static void release();

		struct CharacterInfo
		{
			int16_t ax; // advance x
			int16_t ay; // advance y
			int16_t bw; // bitmap width
			int16_t bh; // bitmap height
			int16_t bl; // bitmap left
			int16_t bt; // bitmap top
			GLfloat tx; // x offset of glyph in texture coordinates
		} mChar[96];

		unsigned int mPointSize;

		GLsizei mAtlasWidth, mAtlasHeight;
		
		OpenGL::TextureObject mTextureObject;
		OpenGL::VertexArrayObject mVertexArrayObject;
		OpenGL::BufferObject mBufferObject;
	};

	//!@cond
	namespace Detail
	{
		class FontStream
		{
		public:

			typedef char char_type;
			typedef boost::iostreams::sink_tag category;

			FontStream(std::shared_ptr<Font> f);

			std::shared_ptr<Font> underlyingFont;

			vec2f scale;
			vec2f position;

			std::streamsize write(
				const char*           text, 
				const std::streamsize length) 
			const noexcept;

			//!@cond
			GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
			//!@endcond

		};
	} // namespace Detail
	//!@endcond

	/**
	 * @brief A font stream.
	 */
	typedef boost::iostreams::stream<Detail::FontStream> FontStream;

} // namespace gintonic
