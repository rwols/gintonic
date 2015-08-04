#include "renderer.hpp"
#include "basic_shapes.hpp"
#include "shaders.hpp"
#include "fonts.hpp"
#include <iostream>
#include <SDL.h>

#ifdef BOOST_MSVC
	#include <Objbase.h> // for CoInitializeEx
#endif

namespace gintonic {

	#ifdef ENABLE_DEBUG_TRACE
	font::flyweight* debug_font = nullptr;
	fontstream* debug_stream = nullptr;
	#endif

	SDL_Window* s_window = nullptr;
	SDL_GLContext s_context;
	SDL_Event s_event = SDL_Event();
	
	bool renderer::s_should_close = false;
	bool renderer::s_fullscreen = false;
	int renderer::s_width = 800;
	int renderer::s_height = 640;
	float renderer::s_aspect_ratio = (float)renderer::s_width / (float)renderer::s_height;

	renderer::time_point_type renderer::s_start_time;
	renderer::duration_type renderer::s_delta_time = renderer::duration_type();
	renderer::duration_type renderer::s_prev_elapsed_time = renderer::duration_type();
	renderer::duration_type renderer::s_elapsed_time = renderer::duration_type();
	boost::circular_buffer<renderer::duration_type> renderer::s_circle_buffer = boost::circular_buffer<renderer::duration_type>();
	vec2f renderer::s_mouse_delta = vec2f(0,0);

	Uint8* s_key_prev_state = nullptr;
	const Uint8* s_key_state = nullptr;
	int s_key_state_count;

	bool renderer::s_matrix_VM_dirty = true;
	bool renderer::s_matrix_PVM_dirty = true;
	bool renderer::s_matrix_N_dirty = true;

	mat4f renderer::s_matrix_M = mat4f();
	mat4f renderer::s_matrix_VM = mat4f();
	mat4f renderer::s_matrix_PVM = mat4f();
	mat3f renderer::s_matrix_N = mat3f();

	GLuint renderer::s_fbo;
	GLuint renderer::s_textures[renderer::GBUFFER_COUNT];
	GLuint renderer::s_depth_texture;

	const camera_transform<float>* renderer::s_camera = nullptr;

	geometry_null_shader* renderer::s_geometry_null_shader = nullptr;
	matrix_PVM_shader* renderer::s_matrix_PVM_shader = nullptr;
	gp_c_shader* renderer::s_gp_c_shader = nullptr;
	gp_cd_shader* renderer::s_gp_cd_shader = nullptr;
	gp_cds_shader* renderer::s_gp_cds_shader = nullptr;
	gp_cdn_shader* renderer::s_gp_cdn_shader = nullptr;

	gp_dc_shader* renderer::s_gp_dc_shader = nullptr;
	gp_dcsc_shader* renderer::s_gp_dcsc_shader = nullptr;
	gp_dt_shader* renderer::s_gp_dt_shader = nullptr;
	gp_dcdt_shader* renderer::s_gp_dcdt_shader = nullptr;

	geometry_pass_shader* renderer::s_geometry_pass_shader = nullptr;
	lp_null_shader* renderer::s_lp_null_shader = nullptr;
	lp_directional_shader* renderer::s_lp_directional_shader = nullptr;
	lp_point_shader* renderer::s_lp_point_shader = nullptr;
	directional_light_pass_shader* renderer::s_directional_light_pass_shader = nullptr;
	text_shader* renderer::s_text_shader = nullptr;

	opengl::unit_quad_P* renderer::s_unit_quad_P = nullptr;
	opengl::unit_sphere_P* renderer::s_unit_sphere_P = nullptr;

	boost::signals2::signal<void(wchar_t)> renderer::char_typed;
	boost::signals2::signal<void(double, double)> renderer::mouse_scrolled;
	boost::signals2::signal<void(double, double)> renderer::mouse_moved;
	boost::signals2::signal<void(int, int, int, int)> renderer::key_pressed;
	boost::signals2::signal<void(int, int, int)> renderer::mouse_pressed;
	boost::signals2::signal<void(int, int)> renderer::window_resized;
	boost::signals2::signal<void(void)> renderer::mouse_entered;
	boost::signals2::signal<void(void)> renderer::mouse_left;
	boost::signals2::signal<void(void)> renderer::about_to_close;

	BOOST_CONSTEXPR GLenum gbuffer_tex_internal[renderer::GBUFFER_COUNT] = 
	{
		GL_RGBA32F, // GBUFFER_POSITION
		GL_RGBA,    // GBUFFER_DIFFUSE
		GL_RGBA,    // GBUFFER_SPECULAR
		GL_RGBA16F, // GBUFFER_NORMAL
		GL_RGB      // GBUFFER_FINAL_COLOR
	};
	BOOST_CONSTEXPR GLenum gbuffer_tex_format[renderer::GBUFFER_COUNT] = 
	{
		GL_RGBA, // GBUFFER_POSITION
		GL_RGBA, // GBUFFER_DIFFUSE
		GL_RGBA, // GBUFFER_SPECULAR
		GL_RGBA, // GBUFFER_NORMAL
		GL_RGB   // GBUFFER_FINAL_COLOR
	};

	#ifdef ENABLE_DEBUG_TRACE
	fontstream& renderer::cerr() { return *debug_stream; }
	#endif

	void renderer::init(const char* title, const camera_transform<float>& cam, const bool fullscreen, const int width, const int height)
	{
		bool was_already_initialized;
		if (is_initialized())
		{
			was_already_initialized = true;
			release();
		}
		else
		{
			was_already_initialized = false;
			if (SDL_Init(SDL_INIT_EVERYTHING) != 0) 
			{
				std::clog << SDL_GetError() << '\n';
				return;
			}
		}
		if (was_already_initialized == false) std::atexit(SDL_Quit);

		s_camera = &cam;
		s_fullscreen = fullscreen;
		s_width = width;
		s_height = height;
		s_aspect_ratio = (float) s_width / (float) s_height;

		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
		if (s_fullscreen) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		s_window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		// We start by trying to obtain an OpenGL 4.1 context.
		SDL_GL_ResetAttributes();
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		s_context = SDL_GL_CreateContext(s_window);
		if (!s_context)
		{
			// Failed to get an OpenGL 4.1 context. Let's try an OpenGL 3.3 context.
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
			s_context = SDL_GL_CreateContext(s_window);
			if (!s_context)
			{
				// The user should update their drivers at this point...
				release();
				throw exception("Failed to obtain OpenGL 3.3 context.");
			}
		}
		const auto glad_error = gladLoadGL();
		// glGetError(); // clear any GL errors from GLEW
		if (glad_error != 1)
		{
			std::clog << "Could not initialize OpenGL.\n";
			release();
			return;
		}

		SDL_GetKeyboardState(&s_key_state_count);
		s_key_prev_state = new Uint8[s_key_state_count];
		std::memset(s_key_prev_state, 0, sizeof(Uint8) * s_key_state_count);
		s_key_state = SDL_GetKeyboardState(nullptr);
		
		if (was_already_initialized == false) std::atexit(renderer::release);
		
		vsync(true);
		
		s_start_time = clock_type::now();

		#ifdef BOOST_MSVC
		CoInitialize(nullptr); // initialize COM
		#endif

		//
		// Initialize framebuffers
		//
		glGenFramebuffers(1, &s_fbo); 
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, s_fbo);
		glGenTextures(GBUFFER_COUNT, s_textures);
		glGenTextures(1, &s_depth_texture);
		for (unsigned int i = 0 ; i < GBUFFER_COUNT; ++i) 
		{
			glBindTexture(GL_TEXTURE_2D, s_textures[i]);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, gbuffer_tex_internal[i], s_width, s_height, 0, gbuffer_tex_format[i], GL_FLOAT, nullptr);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, s_textures[i], 0);
		}
		glBindTexture(GL_TEXTURE_2D, s_depth_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, s_width, s_height, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, nullptr);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, s_depth_texture, 0);
		const GLenum framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (framebuffer_status != GL_FRAMEBUFFER_COMPLETE)
		{
			throw exception("Frame buffer status was not complete: " + std::to_string(framebuffer_status));
		}
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		//
		// Initialize shaders
		//
		try
		{
			s_geometry_null_shader = new geometry_null_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load geometry null shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_matrix_PVM_shader = new matrix_PVM_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load matrix_PVM_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_gp_c_shader = new gp_c_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load gp_c_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_gp_cd_shader = new gp_cd_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load gp_cd_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_gp_cds_shader = new gp_cds_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load gp_cds_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_gp_cds_shader = new gp_cds_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load gp_cdn_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_gp_dc_shader = new gp_dc_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load gp_dc_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_gp_dcsc_shader = new gp_dcsc_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load gp_dcsc_shader: ");
			e.prepend(name());
		}
		try
		{
			s_gp_dt_shader = new gp_dt_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load gp_dt_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_gp_dcdt_shader = new gp_dcdt_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load gp_dcdt_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_geometry_pass_shader = new geometry_pass_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load geometry shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_lp_null_shader = new lp_null_shader();	
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load lp_null_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_lp_directional_shader = new lp_directional_shader();	
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load lp_directional_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_lp_point_shader = new lp_point_shader();	
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load lp_point_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_directional_light_pass_shader = new directional_light_pass_shader();	
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load directional light pass shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_text_shader = new text_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load text shader: ");
			e.prepend(name());
			throw;
		}

		//
		// Initialize basic shapes
		//
		s_unit_quad_P = new opengl::unit_quad_P();
		s_unit_sphere_P = new opengl::unit_sphere_P(64, 64);

		//
		// Initialize debug variables
		//
		#ifdef ENABLE_DEBUG_TRACE
		font::init();
		debug_font = new font::flyweight("../resources/Inconsolata-Regular.ttf", 20);
		debug_stream = new fontstream();
		debug_stream->open(*debug_font);
		#endif
	}

	void renderer::resize(const int width, const int height)
	{
		//
		// resize viewport
		//
		s_width = width;
		s_height = height;
		s_aspect_ratio = (float)s_width / (float)s_height;
		glViewport(0, 0, s_width, s_height);

		//
		// resize framebuffers
		//
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, s_fbo);

		for (unsigned int i = 0 ; i < GBUFFER_COUNT; ++i) 
		{
			glBindTexture(GL_TEXTURE_2D, s_textures[i]);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, gbuffer_tex_internal[i], s_width, s_height, 0, gbuffer_tex_format[i], GL_FLOAT, nullptr);
		}
		glBindTexture(GL_TEXTURE_2D, s_depth_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, s_width, s_height, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, nullptr);
		const GLenum framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (framebuffer_status != GL_FRAMEBUFFER_COMPLETE)
		{
			throw exception("Frame buffer status was not complete: " + std::to_string(framebuffer_status));
		}
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	bool renderer::is_initialized() BOOST_NOEXCEPT_OR_NOTHROW
	{
		return s_window != nullptr;
	}

	void renderer::focus_context() BOOST_NOEXCEPT_OR_NOTHROW
	{ 
		SDL_GL_MakeCurrent(s_window, s_context); 
	}

	void renderer::set_cursor_position(const double x, const double y) BOOST_NOEXCEPT_OR_NOTHROW
	{
		SDL_WarpMouseInWindow(s_window, (int)x, (int)y);
		SDL_FlushEvent(SDL_MOUSEMOTION);
	}

	void renderer::set_freeform_cursor(const bool b)
	{
		SDL_SetRelativeMouseMode(b? SDL_TRUE : SDL_FALSE);
	}

	void renderer::disable_cursor() BOOST_NOEXCEPT_OR_NOTHROW
	{ 
		SDL_ShowCursor(0); 
	}

	void renderer::enable_cursor() BOOST_NOEXCEPT_OR_NOTHROW
	{ 
		SDL_ShowCursor(1); 
	}

	void renderer::center_cursor() BOOST_NOEXCEPT_OR_NOTHROW
	{
		set_cursor_position(s_width / 2, s_height / 2);
	}

	void renderer::vsync(const bool b) 
	{ 
		SDL_GL_SetSwapInterval(b? 1 : 0);
	}

	void renderer::show() BOOST_NOEXCEPT_OR_NOTHROW 
	{ 
		SDL_ShowWindow(s_window); 
	}

	void renderer::hide() BOOST_NOEXCEPT_OR_NOTHROW 
	{ 
		SDL_HideWindow(s_window); 
	}

	void renderer::close() BOOST_NOEXCEPT_OR_NOTHROW
	{
		s_should_close = true;
		about_to_close();
	}

	bool renderer::should_close() BOOST_NOEXCEPT_OR_NOTHROW
	{ 
		return s_should_close; 
	}

	float renderer::aspect_ratio() BOOST_NOEXCEPT_OR_NOTHROW
	{ 
		return s_aspect_ratio; 
	}

	bool renderer::key(const int keycode) BOOST_NOEXCEPT_OR_NOTHROW
	{
		return s_key_state[keycode] != 0;
	}

	bool renderer::key_prev(const int keycode) BOOST_NOEXCEPT_OR_NOTHROW
	{
		return s_key_prev_state[keycode] != 0;
	}

	bool renderer::key_toggle_press(const int keycode) BOOST_NOEXCEPT_OR_NOTHROW
	{
		return key(keycode) && !key_prev(keycode);
	}

	bool renderer::key_toggle_release(const int keycode) BOOST_NOEXCEPT_OR_NOTHROW
	{
		return !key(keycode) && key_prev(keycode);
	}

	bool renderer::mousebutton(const int buttoncode) BOOST_NOEXCEPT_OR_NOTHROW
	{
		return 0 != (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(buttoncode));
	}

	void renderer::release()
	{
		if (s_context) // deletes shaders, textures, framebuffers, etc.
		{
			SDL_GL_DeleteContext(s_context);
			s_context = nullptr;
		}
		if (s_window)
		{
			SDL_DestroyWindow(s_window);
			s_window = nullptr;
		}
	}

	void renderer::update() BOOST_NOEXCEPT_OR_NOTHROW
	{
		#ifdef ENABLE_DEBUG_TRACE
		get_text_shader()->activate();
		get_text_shader()->set_color(vec3f(1.0f, 1.0f, 1.0f));
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		debug_stream->close();
		#endif

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, s_fbo);
		glReadBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_FINAL_COLOR);
		glBlitFramebuffer(0, 0, s_width, s_height, 0, 0, s_width, s_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		
		SDL_GL_SwapWindow(s_window);

		#ifdef ENABLE_DEBUG_TRACE
		debug_stream->open(*debug_font);
		#endif
		
		s_prev_elapsed_time = s_elapsed_time;
		s_elapsed_time = clock_type::now() - s_start_time;
		s_delta_time = s_elapsed_time - s_prev_elapsed_time;

		std::memcpy(s_key_prev_state, s_key_state, sizeof(Uint8) * s_key_state_count);
		s_key_state = SDL_GetKeyboardState(nullptr);

		s_mouse_delta[0] = s_mouse_delta[1] = 0.0f;
		while (SDL_PollEvent(&s_event))
		{
			switch (s_event.type)
			{
			case SDL_WINDOWEVENT:
				switch (s_event.window.event)
				{
					case SDL_WINDOWEVENT_ENTER:
						mouse_entered();
						break;
					case SDL_WINDOWEVENT_LEAVE:
						mouse_left();
						break;
					case SDL_WINDOWEVENT_RESIZED:
						resize((int)s_event.window.data1, (int)s_event.window.data2);
						window_resized(s_width, s_height);
						break;
				}
				break;
			case SDL_KEYDOWN:
				break;
			case SDL_KEYUP:
				break;
			case SDL_MOUSEMOTION:
				s_mouse_delta[0] = (float)s_event.motion.xrel;
				s_mouse_delta[1] = (float)s_event.motion.yrel;
				mouse_moved(s_mouse_delta[0], s_mouse_delta[1]);
				break;
			case SDL_QUIT:
				close();
				break;
			default:
				break;
			}
		}
	}

	void renderer::begin_geometry_pass()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, s_fbo);
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_FINAL_COLOR);
		glClear(GL_COLOR_BUFFER_BIT);
		BOOST_CONSTEXPR GLenum DrawBuffers[4] = 
		{
			GL_COLOR_ATTACHMENT0 + GBUFFER_POSITION, 
			GL_COLOR_ATTACHMENT0 + GBUFFER_DIFFUSE, 
			GL_COLOR_ATTACHMENT0 + GBUFFER_SPECULAR, 
			GL_COLOR_ATTACHMENT0 + GBUFFER_NORMAL
		}; 
		glDrawBuffers(4, DrawBuffers);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	void renderer::begin_stencil_pass()
	{
		glDrawBuffer(GL_NONE);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glClear(GL_STENCIL_BUFFER_BIT);
		glStencilFunc(GL_ALWAYS, 0, 0);
		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
		glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
	}

	void renderer::begin_light_pass()
	{
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_FINAL_COLOR);
		for (unsigned int i = 0; i < GBUFFER_FINAL_COLOR; ++i) 
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, s_textures[i]);
		}
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);
		// glClear(GL_COLOR_BUFFER_BIT);
	}

	void renderer::set_read_buffer(const enum GBUFFER type)
	{
		glReadBuffer(GL_COLOR_ATTACHMENT0 + type);
	}

	void renderer::blit_drawbuffers_to_screen()
	{
		// Take s_fbo as the active framebuffer.
		// We blit the geometry stuff into yet another color attachment.
		// This final color attachment gets blitted to the screen in the update()
		// method which is called at the end of the main render loop.
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, s_fbo);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, s_fbo);
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_FINAL_COLOR);

		const GLsizei halfwidth = (GLsizei)(width() / 2.0f);
		const GLsizei halfheight = (GLsizei)(height() / 2.0f);

		set_read_buffer(GBUFFER_POSITION);
		glBlitFramebuffer(0, 0, width(), height(), 0, 0, halfwidth, halfheight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		set_read_buffer(GBUFFER_NORMAL);
		glBlitFramebuffer(0, 0, width(), height(), 0, halfheight, halfwidth, height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
		set_read_buffer(GBUFFER_DIFFUSE);
		glBlitFramebuffer(0, 0, width(), height(), halfwidth, halfheight, width(), height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
		set_read_buffer(GBUFFER_SPECULAR);
		glBlitFramebuffer(0, 0, width(), height(), halfwidth, 0, width(), halfheight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	}

	void renderer::blit_drawbuffers_to_screen(fontstream& stream)
	{
		// Take s_fbo as the active framebuffer.
		// We blit the geometry stuff into yet another color attachment.
		// This final color attachment gets blitted to the screen in the update()
		// method which is called at the end of the main render loop.
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, s_fbo);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, s_fbo);
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_FINAL_COLOR);

		const GLsizei halfwidth = (GLsizei)(width() / 2.0f);
		const GLsizei halfheight = (GLsizei)(height() / 2.0f);		
		
		set_read_buffer(GBUFFER_POSITION);
		glBlitFramebuffer(0, 0, width(), height(), 0, 0, halfwidth, halfheight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		
		set_read_buffer(GBUFFER_NORMAL);
		glBlitFramebuffer(0, 0, width(), height(), 0, halfheight, halfwidth, height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
		
		set_read_buffer(GBUFFER_DIFFUSE);
		glBlitFramebuffer(0, 0, width(), height(), halfwidth, halfheight, width(), height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
		
		set_read_buffer(GBUFFER_SPECULAR);
		glBlitFramebuffer(0, 0, width(), height(), halfwidth, 0, width(), halfheight, GL_COLOR_BUFFER_BIT, GL_LINEAR);


		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		get_text_shader()->activate();
		get_text_shader()->set_color(vec3f(1.0f, 1.0f, 1.0f));

		stream << "GBUFFER_NORMAL" << std::endl;
		stream->position[0] += 1.0f;
		stream << "GBUFFER_DIFFUSE" << std::endl;
		stream->position[1] -= 1.0f;
		stream << "GBUFFER_SPECULAR" << std::endl;
		stream->position[0] -= 1.0f;
		stream << "GBUFFER_POSITION" << std::endl;

		glEnable(GL_CULL_FACE);
	}

	void renderer::null_light_pass() BOOST_NOEXCEPT_OR_NOTHROW
	{
		const auto& s = get_lp_null_shader();
		s.activate();
		s.set_gbuffer_diffuse(GBUFFER_DIFFUSE);
		s.set_viewport_size(vec2f(static_cast<float>(width()), static_cast<float>(height())));
		get_unit_quad_P().draw();
	}

	void renderer::update_matrix_VM()
	{
		if (s_matrix_VM_dirty)
		{
			s_matrix_VM = camera().matrix_V() * s_matrix_M;
			s_matrix_VM_dirty = false;
		}
	}

	void renderer::update_matrix_PVM()
	{
		update_matrix_VM();
		if (s_matrix_PVM_dirty)
		{
			s_matrix_PVM = camera().matrix_P(width(), height()) * s_matrix_VM;
			s_matrix_PVM_dirty = false;
		}
	}

	void renderer::update_matrix_N()
	{
		update_matrix_VM();
		if (s_matrix_N_dirty)
		{
			s_matrix_N = s_matrix_VM.upper_left_33().invert().transpose();
			s_matrix_N_dirty = false;
		}
	}

	#define RENDERER_NOT_INITIALIZED "Renderer is not initialized."

	const char* renderer::name()
	{
		if (!is_initialized()) throw std::logic_error(RENDERER_NOT_INITIALIZED);
		else return reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	}

	const char* renderer::version()
	{
		if (!is_initialized()) throw std::logic_error(RENDERER_NOT_INITIALIZED);
		else return reinterpret_cast<const char*>(glGetString(GL_VERSION));
	}

} // namespace gintonic