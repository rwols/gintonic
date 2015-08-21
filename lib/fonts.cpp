#include "fonts.hpp"
#include "vertices.hpp"
#include "renderer.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace {

	FT_Library s_ft_library;
	bool s_was_already_initialized = false;

} // namespace

namespace gintonic {

	void font::init()
	{
		if (s_was_already_initialized) return;
		if (FT_Init_FreeType(&s_ft_library)) 
		{
			throw std::runtime_error("Could not initialize font library.");
		}
		std::atexit(font::release);
		s_was_already_initialized = true;
	}

	void font::release()
	{
		FT_Done_FreeType(s_ft_library);
		s_was_already_initialized = false;
	}

	font::font(const key_type& filename_and_size) : object<font, key_type>(filename_and_size)
	{
		construct_from_key();
	}

	font::font(key_type&& filename_and_size) : object<font, key_type>(std::move(filename_and_size))
	{
		construct_from_key();
	}

	font::font(font&& other) 
	: object<font, key_type>(std::move(other))
	, m_atlas_width(std::move(other.m_atlas_width))
	, m_atlas_height(std::move(other.m_atlas_height))
	, m_tex(std::move(other.m_tex))
	, m_vao(std::move(other.m_vao))
	, m_vbo(std::move(other.m_vbo))
	{
		other.m_tex = 0;
		std::memcpy(m_c, other.m_c, sizeof(character_info));
	}
	
	font& font::operator = (font&& other)
	{
		this->~font();
		object<font, key_type>::operator=(std::move(other));
		m_atlas_width = std::move(other.m_atlas_width);
		m_atlas_height = std::move(other.m_atlas_height);
		m_tex = std::move(other.m_tex);
		m_vao = std::move(other.m_vao);
		m_vbo = std::move(other.m_vbo);
		other.m_tex = 0;
		std::memcpy(m_c, other.m_c, sizeof(character_info));
		return *this;
	}

	void font::construct_from_key()
	{
		FT_Face face;
		GLint ox = 0;
		m_atlas_width = 0;
		m_atlas_height = 0;
		const auto filename = std::get<0>(key()).string();

		if (std::get<1>(key()) <= 0)
		{
			throw std::runtime_error("Cannot have a font size <= 0.");
		}
		if (FT_New_Face(s_ft_library, filename.c_str(), 0, &face))
		{
			throw std::runtime_error("Could not open font.");
		}
		
		const auto g = face->glyph;
		FT_Set_Pixel_Sizes(face, 0, std::get<1>(key()));

		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &m_tex);
		glBindTexture(GL_TEXTURE_2D, m_tex);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		opengl::vertex_text2d::enable_attributes();

		for (int i = 32; i < 128; ++i)
		{
			if (FT_Load_Char(face, i, FT_LOAD_RENDER)) continue;
			m_atlas_width += g->bitmap.width;
			m_atlas_height = std::max(m_atlas_height, static_cast<GLsizei>(g->bitmap.rows));
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_atlas_width, m_atlas_height, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

		for (int i = 32; i < 128; ++i)
		{
			if (FT_Load_Char(face, i, FT_LOAD_RENDER)) continue;
			glTexSubImage2D(GL_TEXTURE_2D, 0, ox, 0, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
			m_c[i-32].ax = static_cast<int16_t>(g->advance.x >> 6);
			m_c[i-32].ay = static_cast<int16_t>(g->advance.y >> 6);
			m_c[i-32].bw = static_cast<int16_t>(g->bitmap.width);
			m_c[i-32].bh = static_cast<int16_t>(g->bitmap.rows);
			m_c[i-32].bl = static_cast<int16_t>(g->bitmap_left);
			m_c[i-32].bt = static_cast<int16_t>(g->bitmap_top);
			m_c[i-32].tx = static_cast<GLfloat>(ox) / static_cast<GLfloat>(m_atlas_width);
			ox += g->bitmap.width;
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		FT_Done_Face(face);
	}

	font::~font()
	{
		glDeleteTextures(1, &m_tex);
	}

	void font::draw(const char* text, const std::size_t length, const vec2f& in_position, const vec2f& scale) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		using vert = opengl::vertex_text2d;

		GLsizei n = 0;
		vec2f position(in_position);
		const auto aw = static_cast<GLfloat>(m_atlas_width);
		const auto ah = static_cast<GLfloat>(m_atlas_height);
		const auto original_x_position = position.x;
		std::size_t j, i;
		GLfloat x2, y2, w, h;
		std::vector<vert> coords(6 * length);

		for (j = 0; j < length; ++j)
		{
			if (text[j] == '\n')
			{
				position.x = original_x_position;
				position.y -= static_cast<GLfloat>(std::get<1>(key()) * scale.y);
				continue;
			}

			i = static_cast<std::size_t>(text[j]);

			x2 = position.x + m_c[i-32].bl * scale.x;
			y2 = position.y + m_c[i-32].bt * scale.y;

			w  =  m_c[i-32].bw * scale.x;
			h  =  m_c[i-32].bh * scale.y;

			position.x += m_c[i-32].ax * scale.x;
			position.y += m_c[i-32].ay * scale.y;

			if (!w || !h) continue;
			
			coords[n++] = vert(x2,      y2,     m_c[i-32].tx,                     0.0f             );
			coords[n++] = vert(x2 + w,  y2,     m_c[i-32].tx + m_c[i-32].bw / aw, 0.0f             );
			coords[n++] = vert(x2,      y2 - h, m_c[i-32].tx,                     m_c[i-32].bh / ah);
			coords[n++] = vert(x2 + w,  y2,     m_c[i-32].tx + m_c[i-32].bw / aw, 0.0f             );
			coords[n++] = vert(x2,      y2 - h, m_c[i-32].tx,                     m_c[i-32].bh / ah);
			coords[n++] = vert(x2 + w,  y2 - h, m_c[i-32].tx + m_c[i-32].bw / aw, m_c[i-32].bh / ah);
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_tex);
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		gtBufferData(GL_ARRAY_BUFFER, coords, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, n);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void font::draw(const std::string& text, const vec2f& position, const vec2f& scale) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		
		draw(text.c_str(), text.length(), position, scale);
	}

	void font::draw(const char* text, const std::size_t length, const vec2f& position) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		const vec2f scale(2.0f / (GLfloat)renderer::width(), 2.0f / (GLfloat)renderer::height());
		draw(text, length, position, scale);
	}

	void font::draw(const std::string& text, const vec2f& position) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		draw(text.c_str(), text.length(), position);
	}

	namespace detail
	{
		fontstream::fontstream(const font::flyweight& f) : underlying_font(f)
		{
			scale.x = 2.0f / static_cast<GLfloat>(renderer::width());
			scale.y = 2.0f / static_cast<GLfloat>(renderer::height());
			position.x = -1.0f;
			position.y = 1.0f - scale.y * static_cast<GLfloat>(std::get<1>(underlying_font.get_key()));
		}
		fontstream::fontstream(font::flyweight&& f) : underlying_font(std::move(f))
		{
			scale.x = 2.0f / static_cast<GLfloat>(renderer::width());
			scale.y = 2.0f / static_cast<GLfloat>(renderer::height());
			position.x = -1.0f;
			position.y = 1.0f - scale.y * static_cast<GLfloat>(std::get<1>(underlying_font.get_key()));	
		}

		std::streamsize fontstream::write(const char* text, const std::streamsize length) const BOOST_NOEXCEPT_OR_NOTHROW
		{
			underlying_font.get().draw(text, static_cast<const std::size_t>(length), position, scale);
			return length;
		}
	} // namespace detail

} // namespace gintonic