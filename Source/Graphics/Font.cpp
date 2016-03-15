#include "Font.hpp"
#include "../Foundation/exception.hpp"
#include "vertices.hpp"
#include "renderer.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H

#define POINTSIZE 12

// namespace {

// 	FT_Library s_ft_library;
// 	bool s_was_already_initialized = false;

// } // namespace

namespace gintonic {

	// void font::init()
	// {
	// 	if (s_was_already_initialized) return;
	// 	if (FT_Init_FreeType(&s_ft_library)) 
	// 	{
	// 		throw std::runtime_error("Could not initialize font library.");
	// 	}
	// 	std::atexit(font::release);
	// 	s_was_already_initialized = true;
	// }

	// void font::release()
	// {
	// 	FT_Done_FreeType(s_ft_library);
	// 	s_was_already_initialized = false;
	// }

	// font::font(const key_type& filename_and_size) : object<font, key_type>(filename_and_size)
	// {
	// 	construct_from_key();
	// }

	// font::font(key_type&& filename_and_size) : object<font, key_type>(std::move(filename_and_size))
	// {
	// 	construct_from_key();
	// }

	// font::font(font&& other) 
	// : object<font, key_type>(std::move(other))
	// , m_atlas_width(std::move(other.m_atlas_width))
	// , m_atlas_height(std::move(other.m_atlas_height))
	// , m_tex(std::move(other.m_tex))
	// , m_vao(std::move(other.m_vao))
	// , m_vbo(std::move(other.m_vbo))
	// {
	// 	other.m_tex = 0;
	// 	std::memcpy(m_c, other.m_c, sizeof(character_info));
	// }
	
	// font& font::operator = (font&& other)
	// {
	// 	this->~font();
	// 	object<font, key_type>::operator=(std::move(other));
	// 	m_atlas_width = std::move(other.m_atlas_width);
	// 	m_atlas_height = std::move(other.m_atlas_height);
	// 	m_tex = std::move(other.m_tex);
	// 	m_vao = std::move(other.m_vao);
	// 	m_vbo = std::move(other.m_vbo);
	// 	other.m_tex = 0;
	// 	std::memcpy(m_c, other.m_c, sizeof(character_info));
	// 	return *this;
	// }

Font::Font(const char* filename) : Font(boost::filesystem::path(filename)) {}

Font::Font(const std::string& filename) : Font(boost::filesystem::path(filename)) {}

Font::Font(boost::filesystem::path filename)
: Object<Font, boost::filesystem::path>(std::move(filename))
{
	FT_Library lFreetypeLib;
	FT_Init_FreeType(&lFreetypeLib);
	FT_Face lFace;
	GLint lXOffset = 0;
	mAtlasWidth = 0;
	mAtlasHeight = 0;
	#ifdef BOOST_MSVC
	const auto lFilenameAsString = getName().string();
	const auto* lFilename = lFilenameAsString.c_str();
	#else
	const auto* lFilename = getName().c_str();
	#endif
	if (FT_New_Face(lFreetypeLib, lFilename, 0, &lFace))
	{
		exception lException("The font ");
		lException.append(lFilename);
		lException.append(" could not be opened.");
		throw lException;
	}
	const auto lGlyph = lFace->glyph;
	FT_Set_Pixel_Sizes(lFace, 0, POINTSIZE);
	glBindTexture(GL_TEXTURE_2D, mTextureObject);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindVertexArray(mVertexArrayObject);
	glBindBuffer(GL_ARRAY_BUFFER, mBufferObject);
	opengl::vertex_text2d::enable_attributes();
	for (int i = 32; i < 128; ++i)
	{
		if (FT_Load_Char(lFace, i, FT_LOAD_RENDER)) continue;
		mAtlasWidth += lGlyph->bitmap.width;
		mAtlasHeight = std::max(mAtlasHeight, static_cast<GLsizei>(lGlyph->bitmap.rows));
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mAtlasWidth, mAtlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
	for (int i = 32; i < 128; ++i)
	{
		if (FT_Load_Char(lFace, i, FT_LOAD_RENDER)) continue;
		glTexSubImage2D(GL_TEXTURE_2D, 0, lXOffset, 0, lGlyph->bitmap.width, lGlyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, lGlyph->bitmap.buffer);
		mChar[i-32].ax = static_cast<int16_t>(lGlyph->advance.x >> 6);
		mChar[i-32].ay = static_cast<int16_t>(lGlyph->advance.y >> 6);
		mChar[i-32].bw = static_cast<int16_t>(lGlyph->bitmap.width);
		mChar[i-32].bh = static_cast<int16_t>(lGlyph->bitmap.rows);
		mChar[i-32].bl = static_cast<int16_t>(lGlyph->bitmap_left);
		mChar[i-32].bt = static_cast<int16_t>(lGlyph->bitmap_top);
		mChar[i-32].tx = static_cast<GLfloat>(lXOffset) / static_cast<GLfloat>(mAtlasWidth);
		lXOffset += lGlyph->bitmap.width;
	}

	FT_Done_FreeType(lFreetypeLib);	
}

// void font::construct_from_key()
// {
// 	FT_Face face;
// 	GLint ox = 0;
// 	m_atlas_width = 0;
// 	m_atlas_height = 0;
// 	const auto filename = std::get<0>(key()).string();

// 	if (std::get<1>(key()) <= 0)
// 	{
// 		throw std::runtime_error("Cannot have a font size <= 0.");
// 	}
// 	if (FT_New_Face(s_ft_library, filename.c_str(), 0, &face))
// 	{
// 		throw std::runtime_error("Could not open font.");
// 	}
	
// 	const auto g = face->glyph;
// 	FT_Set_Pixel_Sizes(face, 0, std::get<1>(key()));

// 	glActiveTexture(GL_TEXTURE0);
// 	glGenTextures(1, &m_tex);
// 	glBindTexture(GL_TEXTURE_2D, m_tex);
// 	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// 	glBindVertexArray(m_vao);
// 	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
// 	opengl::vertex_text2d::enable_attributes();

// 	for (int i = 32; i < 128; ++i)
// 	{
// 		if (FT_Load_Char(face, i, FT_LOAD_RENDER)) continue;
// 		m_atlas_width += g->bitmap.width;
// 		m_atlas_height = std::max(m_atlas_height, static_cast<GLsizei>(g->bitmap.rows));
// 	}

// 	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_atlas_width, m_atlas_height, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

// 	for (int i = 32; i < 128; ++i)
// 	{
// 		if (FT_Load_Char(face, i, FT_LOAD_RENDER)) continue;
// 		glTexSubImage2D(GL_TEXTURE_2D, 0, ox, 0, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
// 		m_c[i-32].ax = static_cast<int16_t>(g->advance.x >> 6);
// 		m_c[i-32].ay = static_cast<int16_t>(g->advance.y >> 6);
// 		m_c[i-32].bw = static_cast<int16_t>(g->bitmap.width);
// 		m_c[i-32].bh = static_cast<int16_t>(g->bitmap.rows);
// 		m_c[i-32].bl = static_cast<int16_t>(g->bitmap_left);
// 		m_c[i-32].bt = static_cast<int16_t>(g->bitmap_top);
// 		m_c[i-32].tx = static_cast<GLfloat>(ox) / static_cast<GLfloat>(m_atlas_width);
// 		ox += g->bitmap.width;
// 	}

// 	glBindBuffer(GL_ARRAY_BUFFER, 0);
// 	glBindVertexArray(0);
// 	glBindTexture(GL_TEXTURE_2D, 0);

// 	FT_Done_Face(face);
// }

// font::~font()
// {
// 	glDeleteTextures(1, &m_tex);
// }

void Font::draw(const char* text, const std::size_t length, const vec2f& in_position, const vec2f& scale) const noexcept
{
	using vert = opengl::vertex_text2d;

	GLsizei n = 0;
	vec2f position(in_position);
	const auto aw = static_cast<GLfloat>(mAtlasWidth);
	const auto ah = static_cast<GLfloat>(mAtlasHeight);
	const auto lOriginalXPosition = position.x;
	std::size_t i, j;
	GLfloat x2, y2, w, h;
	std::vector<vert> lCoords(6 * length);

	for (j = 0; j < length; ++j)
	{
		if (text[j] == '\n')
		{
			position.x = lOriginalXPosition;
			position.y -= static_cast<GLfloat>(POINTSIZE * scale.y);
			continue;
		}

		i = static_cast<std::size_t>(text[j]);

		x2 = position.x + mChar[i-32].bl * scale.x;
		y2 = position.y + mChar[i-32].bt * scale.y;

		w  =  mChar[i-32].bw * scale.x;
		h  =  mChar[i-32].bh * scale.y;

		position.x += mChar[i-32].ax * scale.x;
		position.y += mChar[i-32].ay * scale.y;

		if (!w || !h) continue;
		
		lCoords[n++] = vert(x2,      y2,     mChar[i-32].tx,                       0.0f               );
		lCoords[n++] = vert(x2 + w,  y2,     mChar[i-32].tx + mChar[i-32].bw / aw, 0.0f               );
		lCoords[n++] = vert(x2,      y2 - h, mChar[i-32].tx,                       mChar[i-32].bh / ah);
		lCoords[n++] = vert(x2 + w,  y2,     mChar[i-32].tx + mChar[i-32].bw / aw, 0.0f               );
		lCoords[n++] = vert(x2,      y2 - h, mChar[i-32].tx,                       mChar[i-32].bh / ah);
		lCoords[n++] = vert(x2 + w,  y2 - h, mChar[i-32].tx + mChar[i-32].bw / aw, mChar[i-32].bh / ah);
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTextureObject);
	glBindVertexArray(mVertexArrayObject);
	glBindBuffer(GL_ARRAY_BUFFER, mBufferObject);
	gtBufferData(GL_ARRAY_BUFFER, lCoords, GL_DYNAMIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, n);
}

void Font::draw(const std::string& text, const vec2f& position, const vec2f& scale) const noexcept
{
	draw(text.c_str(), text.length(), position, scale);
}

void Font::draw(const char* text, const std::size_t length, const vec2f& position) const noexcept
{
	const vec2f scale(2.0f / (GLfloat)renderer::width(), 2.0f / (GLfloat)renderer::height());
	draw(text, length, position, scale);
}

void Font::draw(const std::string& text, const vec2f& position) const noexcept
{
	draw(text.c_str(), text.length(), position);
}

	namespace Detail
	{
		FontStream::FontStream(std::shared_ptr<Font> f) : underlyingFont(f)
		{
			scale.x = 2.0f / static_cast<GLfloat>(renderer::width());
			scale.y = 2.0f / static_cast<GLfloat>(renderer::height());
			position.x = -1.0f;
			position.y = 1.0f - scale.y * static_cast<GLfloat>(POINTSIZE);
		}

		std::streamsize FontStream::write(const char* text, const std::streamsize length) const noexcept
		{
			underlyingFont->draw(text, static_cast<const std::size_t>(length), position, scale);
			return length;
		}
	} // namespace Detail

} // namespace gintonic