/**
 * @file Font.hpp
 * @brief Defines a font class and a fontstream class. You usually don't use
 * fonts directly, but rather use fontstreams.
 * @author Raoul Wols
 */

#pragma once

#include "../Foundation/Object.hpp"
#include "../Foundation/filesystem.hpp"
#include "../Foundation/tuple.hpp"

#include "../Math/vec2f.hpp"

#include "OpenGL/BufferObject.hpp"
#include "OpenGL/TextureObject.hpp"
#include "OpenGL/VertexArrayObject.hpp"

#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/stream.hpp>
#include <iosfwd>

namespace gintonic {

/**
 * @brief A font.
 */
class GINTONIC_EXPORT Font : public Object<Font, boost::filesystem::path>
{
  public:
    /**
     * @brief Gets thrown when the Font constructor failed
     * to load the given file.
     */
    class InitException : public std::exception
    {
      public:
        inline InitException() noexcept {}
        virtual ~InitException() noexcept = default;

        /**
         * @brief Returns the literal string "Font::InitException".
         * @return The literal string "Font::InitException".
         */
        const char* what() const noexcept { return "Font::InitException"; }
    };

    /**
     * @brief The character type. No widechar support.
     */
    typedef char char_type;

    template <class... Args> inline static SharedPtr create(Args&&... args)
    {
        return SharedPtr(new Font(std::forward<Args>(args)...));
    }

  private:
    /**
     * @brief Construct a Font from the given filename with the
     * given point size.
     * @param [in] filename The filename where the TTF font resides.
     * @param [in] pointsize The point size in pixels.
     * @throws InitException when the supplied filename could not be loaded.
     */
    Font(const char* filename, const unsigned int pointsize);

    /**
     * @brief Construct a Font from the given filename with the
     * given point size.
     * @param [in] filename The filename where the TTF font resides.
     * @param [in] pointsize The point size in pixels.
     * @throws InitException when the supplied filename could not be loaded.
     */
    Font(const std::string& filename, const unsigned int pointsize);

    /**
     * @brief Construct a Font from the given filename with the
     * given point size.
     * @param [in] filename The filename where the TTF font resides.
     * @param [in] pointsize The point size in pixels.
     * @throws InitException when the supplied filename could not be loaded.
     */
    Font(boost::filesystem::path filename, const unsigned int pointsize);

  public:
    /**
     * @brief Returns the point size in pixels.
     * @return The point size in pixels.
     */
    inline unsigned int getPointSize() const noexcept { return mPointSize; }

    /**
     * @brief Default destructor.
     */
    virtual ~Font() = default;

    /**
     * @brief Draw this font. Some shader must be activated before calling this
     * method.
     * @param text The text to render.
     * @param length The length of the text.
     * @param position The position, in `CLIP` space coordinates, of where
     * to start rendering the text.
     * @param scale The scale of the text.
     * @param maxHorizontalOffset the maximal horizontal offset of the text.
     * @return The position of the last drawn character.
     */
    vec2f draw(const char* text, const std::size_t length,
               const vec2f& position, const vec2f& scale,
               const float maxHorizontalOffset = 10000.0f) const noexcept;

    /**
     * @brief Draw this font. Some shader must be activated before calling this
     * method.
     * @param text The text to render.
     * @param position The position, in `CLIP` space coordinates, of where
     * to start rendering the text.
     * @param scale The scale of the text.
     */
    void draw(const std::string& text, const vec2f& position,
              const vec2f& scale) const noexcept;

    /**
     * @brief Draw this font. Some shader must be activated before calling this
     * method. The scale is chosen such that it corresponds to the point size of
     * this font.
     * @param text The text to render.
     * @param length The length of the text.
     * @param position The position, in `CLIP` space coordinates, of where
     * to start rendering the text.
     */
    void draw(const char* text, const std::size_t length,
              const vec2f& position) const noexcept;

    /**
     * @brief Draw this font. Some shader must be activated before calling this
     * method. The scale is chosen such that it corresponds to the point size of
     * this font.
     * @param text The text to render.
     * @param position The position, in `CLIP` space coordinates, of where
     * to start rendering the text.
     */
    void draw(const std::string& text, const vec2f& position) const noexcept;

  private:
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

    OpenGL::TextureObject     mTextureObject;
    OpenGL::VertexArrayObject mVertexArrayObject;
    OpenGL::BufferObject      mBufferObject;
};

//!@cond
namespace Detail {
class FontStream
{
  public:
    typedef char                       char_type;
    typedef boost::iostreams::sink_tag category;

    FontStream(std::shared_ptr<Font> f);

    std::shared_ptr<Font> underlyingFont;

    vec2f scale;
    vec2f position;

    std::streamsize write(const char* text, const std::streamsize length) const
        noexcept;

    GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
};
} // namespace Detail
//!@endcond

/**
 * @brief A font stream.
 */
typedef boost::iostreams::stream<Detail::FontStream> FontStream;

} // namespace gintonic
