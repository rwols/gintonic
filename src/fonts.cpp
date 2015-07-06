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

		FT_Face face;
		int ox = 0;
		int oy = 0;
		m_atlas_width = 0;
		m_atlas_height = 0;

		DEBUG_PRINT;

		if (FT_New_Face(s_ft_library, key().c_str(), 0, &face))
		{
			throw std::runtime_error("Could not open font.");
		}
<<<<<<< HEAD
		auto g = face->glyph;
		FT_Set_Pixel_Sizes(face, 0, 48);
=======
		#endif

		#ifdef BOOST_MSVC
		// ...
		#elif defined(__linux__)
		FT_Set_Pixel_Sizes(m_face, 0, 48);
		#endif
>>>>>>> cf6459f5f5ca7e8673947cdbbc2c66f3a60aa85c

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
		opengl::vertex_text2d<GLfloat>::EnableVertexAttribArray();

		for (int i = 32; i < 128; ++i)
		{
			if (FT_Load_Char(face, i, FT_LOAD_RENDER))
			{
				std::cerr << "Loading of character \'" << (char)i << "\' failed!\n";
				continue;
			}
			m_atlas_width += g->bitmap.width + 1;
			m_atlas_height = std::max(m_atlas_height, (int)g->bitmap.rows);
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_atlas_width, m_atlas_height, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

		for (int i = 32; i < 128; ++i)
		{
			if (FT_Load_Char(face, i, FT_LOAD_RENDER)) continue;
			glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
			m_char_info[i].ax = g->advance.x >> 6;
			m_char_info[i].ay = g->advance.y >> 6;
			m_char_info[i].bw = g->bitmap.width;
			m_char_info[i].bh = g->bitmap.rows;
			m_char_info[i].bl = g->bitmap_left;
			m_char_info[i].bt = g->bitmap_top;
			m_char_info[i].tx = (GLfloat)ox / (GLfloat)m_atlas_width;
			m_char_info[i].ty = (GLfloat)oy / (GLfloat)m_atlas_height;

			ox += g->bitmap.width + 1;
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		FT_Done_Face(face);

		#endif
	}

	font::~font()
	{
		glDeleteTextures(1, &m_tex);
	}

	// void font::set_size(const int pixels)
	// {

	// }

	void font::draw(const std::string& text, vec2f position, const vec2f& scale) const BOOST_NOEXCEPT_OR_NOTHROW
	{

		#ifdef BOOST_MSVC
		std::cout << "User attempts to draw the text:\n";
		std::cout << '\t' << text << "\n\n";
		#elif defined(__linux__)
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_tex);
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		std::vector<opengl::vertex_text2d<GLfloat>> coords(6 * text.length());

		GLsizei n = 0;

		for (const auto c : text)
		{
			const std::size_t i(c);
			const GLfloat x2 = position[0] + m_char_info[i].bl * scale[0];
			const GLfloat y2 = -position[1] - m_char_info[i].bt * scale[1];
			const GLfloat w = m_char_info[i].bw * scale[0];
			const GLfloat h = m_char_info[i].bh * scale[1];

			position[0] += m_char_info[i].ax * scale[0];
			position[1] += m_char_info[i].ay * scale[1];

			if (!w || !h) continue;

			
			coords[n++] = opengl::vertex_text2d<GLfloat>(x2,     -y2,     m_char_info[i].tx,                                                m_char_info[i].ty);
			coords[n++] = opengl::vertex_text2d<GLfloat>(x2 + w, -y2,     m_char_info[i].tx + (GLfloat)(m_char_info[i].bw / m_atlas_width), m_char_info[i].ty);
			coords[n++] = opengl::vertex_text2d<GLfloat>(x2,     -y2 - h, m_char_info[i].tx,                                                (GLfloat)(m_char_info[i].bh / m_atlas_height));
			coords[n++] = opengl::vertex_text2d<GLfloat>(x2 + w, -y2,     m_char_info[i].tx + (GLfloat)(m_char_info[i].bw / m_atlas_width), m_char_info[i].ty);
			coords[n++] = opengl::vertex_text2d<GLfloat>(x2,     -y2 - h, m_char_info[i].tx,                                                (GLfloat)(m_char_info[i].bh / m_atlas_height));
			coords[n++] = opengl::vertex_text2d<GLfloat>(x2 + w, -y2 - h, m_char_info[i].tx + (GLfloat)(m_char_info[i].bw / m_atlas_width), (GLfloat)(m_char_info[i].bh / m_atlas_height));
		}
<<<<<<< HEAD

		gtBufferData(GL_ARRAY_BUFFER, coords, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, n);

		// for (const auto c : text)
		// {


		// 	if (FT_Load_Char(m_face, c, FT_LOAD_RENDER)) continue;
		// 	const auto g = m_face->glyph;

		// 	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, g->bitmap.width, g->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);

		// 	const GLfloat x2 = position[0] + g->bitmap_left * scale[0];
		// 	const GLfloat y2 = -position[1] - g->bitmap_top * scale[1];
		// 	const GLfloat w = g->bitmap.width * scale[0];
		// 	const GLfloat h = g->bitmap.rows * scale[1];

		// 	opengl::vertex_text2d<GLfloat> box[4] =
		// 	{
		// 		{x2,     -y2,     0.0f, 0.0f},
		// 		{x2 + w, -y2,     1.0f, 0.0f},
		// 		{x2,     -y2 - h, 0.0f, 1.0f},
		// 		{x2 + w, -y2 - h, 1.0f, 1.0f}
		// 	};

		// 	glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_DYNAMIC_DRAW);
		// 	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// 	position[0] += (g->advance.x >> 6) * scale[0];
		// 	position[1] += (g->advance.y >> 6) * scale[1];
		// }
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
=======
		#endif
>>>>>>> cf6459f5f5ca7e8673947cdbbc2c66f3a60aa85c
	}

} // namespace gintonic