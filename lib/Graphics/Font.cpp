#include <gintonic/Graphics/Font.hpp>
#include <gintonic/Graphics/OpenGL/Vertices.hpp>
#include <gintonic/Graphics/Renderer.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

using namespace gintonic;

const FilePath& InitException::getFilePath() const noexcept
{
    return mFilePath;
}

InitException::InitException(FilePath filepath)
    : std::invalid_argument("Font::InitException"),
      mFilePath(std::move(filepath))
{
}

std::shared_ptr<Font> Font::fromNativeFontFile(const char* filename,
                                               const unsigned int pointsize)
{
    return fromNativeFontFile(FilePath(filename));
}

std::shared_ptr<Font> Font::fromNativeFontFile(const std::string& filename,
                                               const unsigned int pointsize)
{
    return fromNativeFontFile(FilePath(filename));
}

std::shared_ptr<Font> Font::fromNativeFontFile(FilePath filename,
                                               const unsigned int pointsize)
{
    auto font = std::make_shared<Font>(filename.stem().string());
    font->mPointSize = pointsize;
    font->loadFromFileNow();
    return font;
}

void Font::loadFromFileNow()
{
    FT_Library freetypeLib;
    FT_Init_FreeType(&freetypeLib);
    FT_Face lFace;
    GLint lXOffset = 0;
    mAtlasWidth = 0;
    mAtlasHeight = 0;
    const auto filename =
        getAssetFolder() + "/" + prefixFolder() + "/" + shortName() + ".ttf";
    if (FT_New_Face(freetypeLib, filename.c_str(), 0, &lFace))
    {
        throw InitException(FilePath(filename));
    }
    const auto lGlyph = lFace->glyph;
    FT_Set_Pixel_Sizes(lFace, 0, mPointSize);
    glBindTexture(GL_TEXTURE_2D, mTextureObject);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindVertexArray(mVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, mBufferObject);
    OpenGL::vertex_text2d::enable_attributes();
    for (int i = 32; i < 128; ++i)
    {
        if (FT_Load_Char(lFace, i, FT_LOAD_RENDER)) continue;
        mAtlasWidth += lGlyph->bitmap.width;
        mAtlasHeight =
            std::max(mAtlasHeight, static_cast<GLsizei>(lGlyph->bitmap.rows));
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mAtlasWidth, mAtlasHeight, 0, GL_RED,
                 GL_UNSIGNED_BYTE, nullptr);
    for (int i = 32; i < 128; ++i)
    {
        if (FT_Load_Char(lFace, i, FT_LOAD_RENDER)) continue;
        glTexSubImage2D(GL_TEXTURE_2D, 0, lXOffset, 0, lGlyph->bitmap.width,
                        lGlyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE,
                        lGlyph->bitmap.buffer);
        mChar[i - 32].ax = static_cast<int16_t>(lGlyph->advance.x >> 6);
        mChar[i - 32].ay = static_cast<int16_t>(lGlyph->advance.y >> 6);
        mChar[i - 32].bw = static_cast<int16_t>(lGlyph->bitmap.width);
        mChar[i - 32].bh = static_cast<int16_t>(lGlyph->bitmap.rows);
        mChar[i - 32].bl = static_cast<int16_t>(lGlyph->bitmap_left);
        mChar[i - 32].bt = static_cast<int16_t>(lGlyph->bitmap_top);
        mChar[i - 32].tx =
            static_cast<GLfloat>(lXOffset) / static_cast<GLfloat>(mAtlasWidth);
        lXOffset += lGlyph->bitmap.width;
    }

    FT_Done_FreeType(freetypeLib);
}

vec2f Font::draw(const char* text, const std::size_t length,
                 const vec2f& in_position, const vec2f& scale,
                 const float maxHorizontalOffset) const noexcept
{
    using vert = OpenGL::vertex_text2d;

    GLsizei n = 0;
    vec2f lPosition(in_position);
    const auto aw = static_cast<GLfloat>(mAtlasWidth);
    const auto ah = static_cast<GLfloat>(mAtlasHeight);
    const auto lOriginalXPosition = lPosition.x;
    std::size_t i, j;
    GLfloat x2, y2, w, h;
    std::vector<vert> lCoords(6 * length);

    for (j = 0; j < length; ++j)
    {
        if (text[j] == '\n')
        {
            lPosition.x = lOriginalXPosition;
            lPosition.y -= static_cast<GLfloat>(mPointSize * scale.y);
            continue;
        }

        i = static_cast<std::size_t>(text[j]);

        if (maxHorizontalOffset * scale.x <=
            (lPosition.x + 2.0f * mChar[i - 32].bl * scale.x))
        {
            lPosition.x = lOriginalXPosition;
            lPosition.y -= static_cast<GLfloat>(mPointSize * scale.y);
        }

        x2 = lPosition.x + mChar[i - 32].bl * scale.x;
        y2 = lPosition.y + mChar[i - 32].bt * scale.y;

        w = mChar[i - 32].bw * scale.x;
        h = mChar[i - 32].bh * scale.y;

        lPosition.x += mChar[i - 32].ax * scale.x;
        lPosition.y += mChar[i - 32].ay * scale.y;

        if (!w || !h) continue;

        lCoords[n++] = vert(x2, y2, mChar[i - 32].tx, 0.0f);
        lCoords[n++] =
            vert(x2 + w, y2, mChar[i - 32].tx + mChar[i - 32].bw / aw, 0.0f);
        lCoords[n++] =
            vert(x2, y2 - h, mChar[i - 32].tx, mChar[i - 32].bh / ah);
        lCoords[n++] =
            vert(x2 + w, y2, mChar[i - 32].tx + mChar[i - 32].bw / aw, 0.0f);
        lCoords[n++] =
            vert(x2, y2 - h, mChar[i - 32].tx, mChar[i - 32].bh / ah);
        lCoords[n++] =
            vert(x2 + w, y2 - h, mChar[i - 32].tx + mChar[i - 32].bw / aw,
                 mChar[i - 32].bh / ah);
    }

    mTextureObject.bind(GL_TEXTURE_2D, 0);
    glBindVertexArray(mVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, mBufferObject);
    gtBufferData(GL_ARRAY_BUFFER, lCoords, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, n);

    return lPosition;
}

void Font::draw(const std::string& text, const vec2f& position,
                const vec2f& scale) const noexcept
{
    draw(text.c_str(), text.length(), position, scale);
}

void Font::draw(const char* text, const std::size_t length,
                const vec2f& position) const noexcept
{
    const vec2f scale(2.0f / (GLfloat)Renderer::width(),
                      2.0f / (GLfloat)Renderer::height());
    draw(text, length, position, scale);
}

void Font::draw(const std::string& text, const vec2f& position) const noexcept
{
    draw(text.c_str(), text.length(), position);
}

namespace Detail
{
FontStream::FontStream(std::shared_ptr<Font> f) : underlyingFont(std::move(f))
{
    scale.x = 2.0f / static_cast<GLfloat>(Renderer::width());
    scale.y = 2.0f / static_cast<GLfloat>(Renderer::height());
    position.x = -1.0f;
    position.y =
        1.0f - scale.y * static_cast<GLfloat>(underlyingFont->getPointSize());
}

std::streamsize FontStream::write(const char* text,
                                  const std::streamsize length) const noexcept
{
    underlyingFont->draw(text, static_cast<const std::size_t>(length), position,
                         scale);
    return length;
}
} // namespace Detail

} // namespace gintonic
