#include "fonts.hpp"
#include "vertices.hpp"

namespace {

	#ifdef BOOST_MSVC

	#elif defined(__linux__)

	static FT_Library s_ft_library;

	#else

	#error "Implement me!"

	#endif

} // namespace

namespace gintonic {

	void font::init()
	{
		#ifdef BOOST_MSVC

		#elif defined(__linux__)

		if (FT_Init_FreeType(&s_ft_library)) 
		{
			throw std::runtime_error("Could not initialize font library.");
		}

		#else

		#error "Implement me!"

		#endif

		std::atexit(font::release);
	}

	void font::release()
	{

		#ifdef BOOST_MSVC

		#elif defined(__linux__)
		FT_Done_FreeType(s_ft_library);
		#endif
	}

	font::font(const key_type& filename) : object<font, key_type>(filename)
	{
		init_class();
	}

	font::font(key_type&& filename) : object<font, key_type>(std::move(filename))
	{
		init_class();
	}

	void font::init_class()
	{
		#ifdef BOOST_MSVC
		std::cout << "User tried to initialize font " << key() << '\n';
		#elif defined (__linux__)
		if (FT_New_Face(s_ft_library, key().c_str(), 0, &m_face))
		{
			throw std::runtime_error("Could not open font.");
		}
		#endif

		#ifdef BOOST_MSVC
		std::cout << "User is trying to set font size to " << pixels << " pixels.\n";
		#elif defined(__linux__)
		FT_Set_Pixel_Sizes(m_face, 0, 48);
		#endif

		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &m_tex);
		glBindTexture(GL_TEXTURE_2D, m_tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
	}

	font::~font()
	{
		glDeleteTextures(1, &m_tex);

		#ifdef BOOST_MSVC

		#elif defined(__linux__)
		FT_Done_Face(m_face);
		#endif
	}

	// void font::set_size(const int pixels)
	// {

	// }

	void font::draw(const std::string& text, vec2f position, const vec2f& scale) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_tex);
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		for (const auto c : text)
		{
			if (FT_Load_Char(m_face, c, FT_LOAD_RENDER)) continue;
			
			const auto g = m_face->glyph;

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, g->bitmap.width, g->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);

			const GLfloat x2 = position[0] + g->bitmap_left * scale[0];
			const GLfloat y2 = -position[1] - g->bitmap_top * scale[1];
			const GLfloat w = g->bitmap.width * scale[0];
			const GLfloat h = g->bitmap.rows * scale[1];

			GLfloat box[4][4] =
			{
				{x2,     -y2,     0, 0},
				{x2 + w, -y2,     1, 0},
				{x2,     -y2 - h, 0, 1},
				{x2 + w, -y2 - h, 1, 1}
			};

			DEBUG_GL_ERROR;
			glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_DYNAMIC_DRAW);
			DEBUG_GL_ERROR;
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			DEBUG_GL_ERROR;

			position[0] += (g->advance.x >> 6) * scale[0];
			position[1] += (g->advance.y >> 6) * scale[1];
		}
	}

} // namespace gintonic