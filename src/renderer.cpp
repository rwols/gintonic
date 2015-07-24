#include "renderer.hpp"
#include <iostream>

#ifndef DEBUG_PRINT
#define DEBUG_PRINT_START std::cerr << __FILE__ << '(' << __LINE__ << ')'
#define DEBUG_PRINT DEBUG_PRINT_START << '\n'
#define PRINT_VAR(var) DEBUG_PRINT_START << ": " << #var << " == " << var << '\n'
#endif

namespace gintonic {

	SDL_Window* renderer::s_window = nullptr;
	SDL_GLContext renderer::s_context;
	SDL_Event renderer::s_event = SDL_Event();
	
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

	Uint8* renderer::s_key_prev_state = nullptr;
	const Uint8* renderer::s_key_state = nullptr;
	int renderer::s_key_state_count;

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
	gp_c_shader* renderer::s_gp_c_shader = nullptr;
	gp_cd_shader* renderer::s_gp_cd_shader = nullptr;
	geometry_pass_shader* renderer::s_geometry_pass_shader = nullptr;
	directional_light_pass_shader* renderer::s_directional_light_pass_shader = nullptr;
	text_shader* renderer::s_text_shader = nullptr;

	boost::signals2::signal<void(wchar_t)> renderer::char_typed;
	boost::signals2::signal<void(double, double)> renderer::mouse_scrolled;
	boost::signals2::signal<void(double, double)> renderer::mouse_moved;
	boost::signals2::signal<void(int, int, int, int)> renderer::key_pressed;
	boost::signals2::signal<void(int, int, int)> renderer::mouse_pressed;
	boost::signals2::signal<void(int, int)> renderer::window_resized;
	boost::signals2::signal<void(void)> renderer::mouse_entered;
	boost::signals2::signal<void(void)> renderer::mouse_left;
	boost::signals2::signal<void(void)> renderer::about_to_close;

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
		SDL_GL_ResetAttributes();
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		s_context = SDL_GL_CreateContext(s_window);
		const auto glad_error = gladLoadGL();
		glGetError(); // clear any GL errors from GLEW
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
		BOOST_CONSTEXPR GLenum tex_internal[GBUFFER_COUNT] = { GL_RGB32F, GL_RGBA, GL_RGB32F, GL_RG32F };
		BOOST_CONSTEXPR GLenum tex_format[GBUFFER_COUNT] = { GL_RGB, GL_RGBA, GL_RGB, GL_RG };
		for (unsigned int i = 0 ; i < GBUFFER_COUNT; ++i) 
		{
			glBindTexture(GL_TEXTURE_2D, s_textures[i]);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, tex_internal[i], s_width, s_height, 0, tex_format[i], GL_FLOAT, nullptr);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, s_textures[i], 0);
		}
		glBindTexture(GL_TEXTURE_2D, s_depth_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, s_width, s_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, s_depth_texture, 0);
		const GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 }; 
		glDrawBuffers(4, DrawBuffers);
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
		BOOST_CONSTEXPR GLenum tex_internal[GBUFFER_COUNT] = { GL_RGB32F, GL_RGBA, GL_RGB32F, GL_RG32F };
		BOOST_CONSTEXPR GLenum tex_format[GBUFFER_COUNT] = { GL_RGB, GL_RGBA, GL_RGB, GL_RG };
		for (unsigned int i = 0 ; i < GBUFFER_COUNT; ++i) 
		{
			glBindTexture(GL_TEXTURE_2D, s_textures[i]);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, tex_internal[i], s_width, s_height, 0, tex_format[i], GL_FLOAT, nullptr);
			// glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, s_textures[i], 0);
		}
		glBindTexture(GL_TEXTURE_2D, s_depth_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, s_width, s_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		// glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, s_depth_texture, 0);
		// GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 }; 
		// glDrawBuffers(4, DrawBuffers);
		GLenum framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (framebuffer_status != GL_FRAMEBUFFER_COMPLETE)
		{
			throw exception("Frame buffer status was not complete: " + std::to_string(framebuffer_status));
		}
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	void renderer::release()
	{
		// if (s_directional_light_pass_shader)
		// {
		// 	delete s_directional_light_pass_shader;
		// 	s_directional_light_pass_shader = nullptr;
		// }
		// if (s_geometry_pass_shader)
		// {
		// 	delete s_geometry_pass_shader;
		// 	s_geometry_pass_shader = nullptr;
		// }
		// if (s_geometry_null_shader)
		// {
		// 	delete s_geometry_null_shader;
		// 	s_geometry_null_shader = nullptr;
		// }
		// if (s_key_prev_state)
		// {
		// 	delete [] s_key_prev_state;
		// 	s_key_prev_state = nullptr;
		// }
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
		SDL_GL_SwapWindow(s_window);
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

	void renderer::bind_for_writing()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, s_fbo);
		// glDepthMask(GL_TRUE);
		// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// glEnable(GL_DEPTH_TEST);
		// glDisable(GL_BLEND);
	}

	void renderer::bind_for_reading()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, s_fbo);
		// for (unsigned int i = 0; i < GBUFFER_COUNT; ++i) 
		// {
		// 	glActiveTexture(GL_TEXTURE0 + i);
		// 	glBindTexture(GL_TEXTURE_2D, s_textures[i]);
		// }
		// glDepthMask(GL_FALSE);
		// glDisable(GL_DEPTH_TEST);
		// glEnable(GL_BLEND);
		// glBlendEquation(GL_FUNC_ADD);
		// glBlendFunc(GL_ONE, GL_ONE);
		// glClear(GL_COLOR_BUFFER_BIT);
	}

	void renderer::set_read_buffer(const enum GBUFFER type)
	{
		glReadBuffer(GL_COLOR_ATTACHMENT0 + type);
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