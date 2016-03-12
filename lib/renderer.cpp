#include "renderer.hpp"
#include "basic_shapes.hpp"
#include "shaders.hpp"
#include "fonts.hpp"
#include "exception.hpp"
#include "vec4f.hpp"
#include "camera.hpp"
#include "proj_info.hpp"
#include "materials.hpp"
#include "mesh.hpp"
#include "lights.hpp"
#include "entity.hpp"
#include <iostream>
#include <SDL.h>

#ifdef BOOST_MSVC
	#include <Objbase.h> // for CoInitializeEx
#endif

namespace gintonic {

	// ALL the global variables.

	#ifdef ENABLE_DEBUG_TRACE
	std::shared_ptr<Font> sDebugFont = nullptr;
	FontStream* sDebugStream = nullptr;
	#endif

	SDL_Window* sWindow = nullptr;
	SDL_GLContext sContext;
	SDL_Event sEvent = SDL_Event();
	
	bool renderer::s_should_close = false;
	bool renderer::s_fullscreen = false;
	bool renderer::sRenderInWireframeMode = false;
	int renderer::s_width = 800;
	int renderer::s_height = 640;
	float renderer::s_aspectratio = (float)renderer::s_width / (float)renderer::s_height;

	renderer::time_point_type renderer::s_start_time;
	renderer::duration_type renderer::s_delta_time = renderer::duration_type();
	renderer::duration_type renderer::s_prev_elapsed_time = renderer::duration_type();
	renderer::duration_type renderer::s_elapsed_time = renderer::duration_type();
	boost::circular_buffer<renderer::duration_type> renderer::s_circle_buffer = boost::circular_buffer<renderer::duration_type>();
	vec2f renderer::s_mouse_delta = vec2f(0,0);

	Uint8* s_key_prev_state = nullptr;
	const Uint8* s_key_state = nullptr;
	int s_key_state_count;

	bool renderer::s_matrix_P_dirty = true;
	bool renderer::s_matrix_VM_dirty = true;
	bool renderer::s_matrix_PVM_dirty = true;
	bool renderer::s_matrix_N_dirty = true;

	mat4f renderer::s_matrix_P = mat4f();
	mat4f renderer::s_matrix_V = mat4f();
	mat4f renderer::s_matrix_M = mat4f();
	mat4f renderer::s_matrix_VM = mat4f();
	mat4f renderer::s_matrix_PVM = mat4f();
	mat3f renderer::s_matrix_N = mat3f();

	GLuint renderer::s_fbo;
	GLuint renderer::s_textures[renderer::GBUFFER_COUNT];
	GLuint renderer::s_depth_texture;
	GLuint renderer::s_shadow_texture;

	std::shared_ptr<entity> renderer::sCameraEntity = std::shared_ptr<entity>(nullptr);

	write_lock renderer::sEntityQueueLock;
	std::vector<std::shared_ptr<entity>> renderer::sFutureQueue;
	std::vector<std::shared_ptr<entity>> renderer::sCurrentQueue;

	matrix_PVM_shader* renderer::s_matrix_PVM_shader = nullptr;

	gp_shader* renderer::s_gp_shader = nullptr;
	gp_d_shader* renderer::s_gp_d_shader = nullptr;
	gp_s_shader* renderer::s_gp_s_shader = nullptr;
	gp_n_shader* renderer::s_gp_n_shader = nullptr;
	gp_ds_shader* renderer::s_gp_ds_shader = nullptr;
	gp_dn_shader* renderer::s_gp_dn_shader = nullptr;
	gp_sn_shader* renderer::s_gp_sn_shader = nullptr;
	gp_dsn_shader* renderer::s_gp_dsn_shader = nullptr;
	
	gpi_shader* renderer::s_gpi_shader = nullptr;
	gpi_d_shader* renderer::s_gpi_d_shader = nullptr;
	gpi_s_shader* renderer::s_gpi_s_shader = nullptr;
	gpi_n_shader* renderer::s_gpi_n_shader = nullptr;
	gpi_ds_shader* renderer::s_gpi_ds_shader = nullptr;
	gpi_dn_shader* renderer::s_gpi_dn_shader = nullptr;
	gpi_sn_shader* renderer::s_gpi_sn_shader = nullptr;
	gpi_dsn_shader* renderer::s_gpi_dsn_shader = nullptr;

	lp_ambient_shader* renderer::s_lp_ambient_shader = nullptr;
	lp_directional_shader* renderer::s_lp_directional_shader = nullptr;
	lp_point_shader* renderer::s_lp_point_shader = nullptr;
	lp_spot_shader* renderer::s_lp_spot_shader = nullptr;

	sp_directional_shader* renderer::s_sp_directional_shader = nullptr;

	skybox_shader* renderer::s_skybox_shader = nullptr;

	text_shader* renderer::s_text_shader = nullptr;

	unit_quad_P* renderer::s_unit_quad_P = nullptr;
	unit_cube_P* renderer::s_unit_cube_P = nullptr;
	unit_cube_P_flipped* renderer::s_unit_cube_P_flipped = nullptr;
	unit_sphere_P* renderer::s_unit_sphere_P = nullptr;
	unit_cone_P* renderer::s_unit_cone_P = nullptr;

	boost::signals2::signal<void(wchar_t)> renderer::char_typed;
	boost::signals2::signal<void(double, double)> renderer::mouse_scrolled;
	boost::signals2::signal<void(double, double)> renderer::mouse_moved;
	boost::signals2::signal<void(int, int, int, int)> renderer::key_pressed;
	boost::signals2::signal<void(int, int, int)> renderer::mouse_pressed;
	boost::signals2::signal<void(int, int)> renderer::window_resized;
	boost::signals2::signal<void(void)> renderer::mouse_entered;
	boost::signals2::signal<void(void)> renderer::mouse_left;
	boost::signals2::signal<void(void)> renderer::about_to_close;

	constexpr GLenum gbuffer_tex_internal[renderer::GBUFFER_COUNT] = 
	{
		GL_RGBA32F, // GBUFFER_POSITION
		GL_RGBA,    // GBUFFER_DIFFUSE
		GL_RGBA,    // GBUFFER_SPECULAR
		GL_RGBA16F, // GBUFFER_NORMAL
		GL_RGB      // GBUFFER_FINAL_COLOR
	};
	constexpr GLenum gbuffer_tex_format[renderer::GBUFFER_COUNT] = 
	{
		GL_RGBA, // GBUFFER_POSITION
		GL_RGBA, // GBUFFER_DIFFUSE
		GL_RGBA, // GBUFFER_SPECULAR
		GL_RGBA, // GBUFFER_NORMAL
		GL_RGB   // GBUFFER_FINAL_COLOR
	};

	#ifdef ENABLE_DEBUG_TRACE
	FontStream& renderer::cerr() { return *sDebugStream; }
	#endif

	void renderer::getElapsedAndDeltaTime(double& elapsedTime, double& deltaTime)
	{
		using std::chrono::duration_cast;
		using std::chrono::nanoseconds;
		elapsedTime = static_cast<double>(duration_cast<nanoseconds>(elapsed_time()).count()) / double(1e9);
		deltaTime = static_cast<double>(duration_cast<nanoseconds>(delta_time()).count()) / double(1e9);
	}

	void renderer::init(const char* title, std::shared_ptr<entity> cameraEntity, const bool fullscreen, const int width, const int height)
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

		sCameraEntity = std::move(cameraEntity);
		s_fullscreen = fullscreen;
		s_width = width;
		s_height = height;
		s_aspectratio = (float) s_width / (float) s_height;
		if (sCameraEntity->camera)
		{
			sCameraEntity->camera->setWidth(static_cast<float>(s_width));
			sCameraEntity->camera->setHeight(static_cast<float>(s_height));
			// sCameraEntity->camera->setNearPlane(0.01f);
			// sCameraEntity->camera->setFarPlane(100.0f);
			// sCameraEntity->camera->setProjection(camera::kPerspectiveProjection);
		}
		else
		{
			throw std::runtime_error("Expected a valid entity with a camera component.");
		}

		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
		if (s_fullscreen) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		sWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		// We start by trying to obtain an OpenGL 4.1 context.
		SDL_GL_ResetAttributes();
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		sContext = SDL_GL_CreateContext(sWindow);
		if (!sContext)
		{
			// Failed to get an OpenGL 4.1 context. Let's try an OpenGL 3.3 context.
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
			sContext = SDL_GL_CreateContext(sWindow);
			if (!sContext)
			{
				// The user should update their drivers at this point...
				release();
				throw exception("Failed to obtain OpenGL 3.3 context.");
			}
		}
		const auto glad_error = gladLoadGL();
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
		glClearColor(0.03, 0.03, 0.03, 0.0);
		
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
		glGenTextures(1, &s_shadow_texture);
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
		// Initializer shaders
		//
		init_shaders();

		//
		// Initialize basic shapes
		//
		s_unit_quad_P = new unit_quad_P();
		s_unit_cube_P = new unit_cube_P();
		s_unit_cube_P_flipped = new unit_cube_P_flipped();
		s_unit_sphere_P = new unit_sphere_P(64, 64);
		s_unit_cone_P = new unit_cone_P(16);

		//
		// Initialize debug variables
		//
		#ifdef ENABLE_DEBUG_TRACE
		sDebugFont = std::make_shared<Font>("../resources/Inconsolata-Regular.ttf");
		sDebugStream = new FontStream();
		sDebugStream->open(sDebugFont);
		#endif
	}

	void renderer::init_dummy(const bool construct_shaders)
	{
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
		sWindow = SDL_CreateWindow("dummy context", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 20, 20, flags);

		// We start by trying to obtain an OpenGL 4.1 context.
		SDL_GL_ResetAttributes();
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		sContext = SDL_GL_CreateContext(sWindow);
		if (!sContext)
		{
			// Failed to get an OpenGL 4.1 context. Let's try an OpenGL 3.3 context.
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
			sContext = SDL_GL_CreateContext(sWindow);
			if (!sContext)
			{
				// The user should update their drivers at this point...
				release();
				throw exception("Failed to obtain OpenGL 3.3 context.");
			}
		}
		const auto glad_error = gladLoadGL();
		if (glad_error != 1)
		{
			std::clog << "Could not initialize OpenGL.\n";
			release();
			return;
		}

		if (construct_shaders) init_shaders();
	}

	void renderer::resize(const int width, const int height)
	{
		//
		// resize viewport
		//
		s_width = width;
		s_height = height;
		s_aspectratio = (float)s_width / (float)s_height;
		s_matrix_P_dirty = true;
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

		//
		// Update projection matrix
		//
		sCameraEntity->camera->setWidth(static_cast<float>(s_width));
		sCameraEntity->camera->setHeight(static_cast<float>(s_height));
	}

	bool renderer::is_initialized() noexcept
	{
		return sWindow != nullptr;
	}

	void renderer::setCameraEntity(std::shared_ptr<entity> cameraEntity)
	{
		sCameraEntity = std::move(cameraEntity);
		if (sCameraEntity->camera)
		{
			sCameraEntity->camera->setWidth(static_cast<float>(s_width));
			sCameraEntity->camera->setHeight(static_cast<float>(s_height));
		}
		else
		{
			throw std::runtime_error("entity is missing a camera component.");
		}
	}

	void renderer::focus_context() noexcept
	{ 
		SDL_GL_MakeCurrent(sWindow, sContext); 
	}

	void renderer::set_cursor_position(const double x, const double y) noexcept
	{
		SDL_WarpMouseInWindow(sWindow, (int)x, (int)y);
		SDL_FlushEvent(SDL_MOUSEMOTION);
	}

	void renderer::set_freeform_cursor(const bool b)
	{
		SDL_SetRelativeMouseMode(b? SDL_TRUE : SDL_FALSE);
	}

	void renderer::disable_cursor() noexcept
	{ 
		SDL_ShowCursor(0); 
	}

	void renderer::enable_cursor() noexcept
	{ 
		SDL_ShowCursor(1); 
	}

	void renderer::center_cursor() noexcept
	{
		set_cursor_position(s_width / 2, s_height / 2);
	}

	void renderer::vsync(const bool b) 
	{ 
		SDL_GL_SetSwapInterval(b? 1 : 0);
	}

	void renderer::setWireframeMode(const bool yesOrNo) noexcept
	{
		sRenderInWireframeMode = yesOrNo;
	}

	void renderer::show() noexcept 
	{ 
		SDL_ShowWindow(sWindow); 
	}

	void renderer::hide() noexcept 
	{ 
		SDL_HideWindow(sWindow); 
	}

	void renderer::close() noexcept
	{
		s_should_close = true;
		about_to_close();
	}

	bool renderer::should_close() noexcept
	{ 
		return s_should_close; 
	}

	vec2f renderer::viewport_size() noexcept
	{
		return vec2f(static_cast<float>(s_width), static_cast<float>(s_height));
	}

	bool renderer::key(const int keycode) noexcept
	{
		return s_key_state[keycode] != 0;
	}

	bool renderer::key_prev(const int keycode) noexcept
	{
		return s_key_prev_state[keycode] != 0;
	}

	bool renderer::key_toggle_press(const int keycode) noexcept
	{
		return key(keycode) && !key_prev(keycode);
	}

	bool renderer::key_toggle_release(const int keycode) noexcept
	{
		return !key(keycode) && key_prev(keycode);
	}

	bool renderer::mousebutton(const int buttoncode) noexcept
	{
		return 0 != (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(buttoncode));
	}

	void renderer::release()
	{
		if (sContext) // deletes shaders, textures, framebuffers, etc.
		{
			SDL_GL_DeleteContext(sContext);
			sContext = nullptr;
		}
		if (sWindow)
		{
			SDL_DestroyWindow(sWindow);
			sWindow = nullptr;
		}
	}

	void renderer::update() noexcept
	{
		sEntityQueueLock.obtain();
		sCurrentQueue.swap(sFutureQueue);
		sFutureQueue.clear();
		sEntityQueueLock.release();

		std::vector<entity*> lShadowCastingGeometryEntities;
		std::vector<entity*> lNonShadowCastingGeometryEntities;
		std::vector<entity*> lShadowCastingLightEntities;
		std::vector<entity*> lNonShadowCastingLightEntities;

		for (auto lEntity : sCurrentQueue)
		{
			if (lEntity->castsShadow)
			{
				if (lEntity->mesh && lEntity->material)
				{
					lShadowCastingGeometryEntities.push_back(lEntity.get());
				}
				if (lEntity->light)
				{
					lShadowCastingLightEntities.push_back(lEntity.get());
					if (!lEntity->shadowBuffer) lEntity->light->initializeShadowBuffer(lEntity);
				}
			}
			else
			{
				if (lEntity->mesh && lEntity->material)
				{
					lNonShadowCastingGeometryEntities.push_back(lEntity.get());
				}
				if (lEntity->light)
				{
					lNonShadowCastingLightEntities.push_back(lEntity.get());
				}
			}
		}

		// Geometry collection phase.
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, s_fbo);
		// glDrawBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_FINAL_COLOR);
		// glClear(GL_COLOR_BUFFER_BIT);
		constexpr GLenum DrawBuffers[4] = 
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

		if (sRenderInWireframeMode)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		for (auto* lEntity : lShadowCastingGeometryEntities)
		{
			set_model_matrix(lEntity->global_transform());
			lEntity->material->bind();
			lEntity->mesh->draw();
		}
		for (auto* lEntity : lNonShadowCastingGeometryEntities)
		{
			set_model_matrix(lEntity->global_transform());
			lEntity->material->bind();
			lEntity->mesh->draw();
		}

		// Shadow collection phase.
		// for (auto lEntity : sCurrentQueue)
		// {
		// 	if (lEntity->castsShadow)
		// 	{
		// 		if (lEntity->light)
		// 		{
		// 			if (!lEntity->shadowBuffer)
		// 			{
		// 				lEntity->light->setupShadowBuffer(lEntity);
		// 			}
		// 			lEntity->light->prepareShadowPass(lEntity);
		// 			for (auto lGeometryEntity : sCurrentQueue)
		// 			{
		// 				if (lGeometryEntity->castsShadow)
		// 				{
		// 					set_model_matrix(lEntity.global_transform());
		// 					lGeometryEntity->mesh->draw();
		// 					lEntity->light->renderShadow(lEntity, lGeometryEntity);
		// 				}
		// 			}
		// 		}
		// 	}
		// }

		// Illumination phase.
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		for (unsigned int i = 0; i < GBUFFER_FINAL_COLOR; ++i) 
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, s_textures[i]);
		}
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);
		glClear(GL_COLOR_BUFFER_BIT);

		// Ambient lighting
		s_lp_ambient_shader->activate();
		s_lp_ambient_shader->set_gbuffer_diffuse(GBUFFER_DIFFUSE);
		s_lp_ambient_shader->set_viewport_size(viewport_size());
		s_lp_ambient_shader->set_light_intensity(vec4f(1.0f, 1.0f, 1.0f, 1.0f));
		s_unit_quad_P->draw();
		
		for (auto* lEntity : lShadowCastingLightEntities)
		{
			lEntity->light->shine(*lEntity);
		}
		for (auto* lEntity : lNonShadowCastingLightEntities)
		{
			lEntity->light->shine(*lEntity);
		}

		// #ifdef ENABLE_DEBUG_TRACE
		// get_text_shader()->activate();
		// get_text_shader()->set_color(vec3f(1.0f, 1.0f, 1.0f));
		// glDisable(GL_CULL_FACE);
		// glEnable(GL_BLEND);
		// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// sDebugStream->close();
		// #endif

		// glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		// glBindFramebuffer(GL_READ_FRAMEBUFFER, s_fbo);
		// glReadBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_FINAL_COLOR);
		// glBlitFramebuffer(0, 0, s_width, s_height, 0, 0, s_width, s_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		// glClear(GL_COLOR_BUFFER_BIT);
		
		SDL_GL_SwapWindow(sWindow);

		// #ifdef ENABLE_DEBUG_TRACE
		// sDebugStream->open(*sDebugFont);
		// #endif
		
		s_prev_elapsed_time = s_elapsed_time;
		s_elapsed_time = clock_type::now() - s_start_time;
		s_delta_time = s_elapsed_time - s_prev_elapsed_time;

		std::memcpy(s_key_prev_state, s_key_state, sizeof(Uint8) * s_key_state_count);
		s_key_state = SDL_GetKeyboardState(nullptr);

		s_mouse_delta = 0.0f;
		while (SDL_PollEvent(&sEvent))
		{
			switch (sEvent.type)
			{
			case SDL_WINDOWEVENT:
				switch (sEvent.window.event)
				{
					case SDL_WINDOWEVENT_ENTER:
						mouse_entered();
						break;
					case SDL_WINDOWEVENT_LEAVE:
						mouse_left();
						break;
					case SDL_WINDOWEVENT_RESIZED:
						resize((int)sEvent.window.data1, (int)sEvent.window.data2);
						window_resized(s_width, s_height);
						break;
				}
				break;
			case SDL_KEYDOWN:
				break;
			case SDL_KEYUP:
				break;
			case SDL_MOUSEMOTION:
				s_mouse_delta.x += (float)sEvent.motion.xrel;
				s_mouse_delta.y += (float)sEvent.motion.yrel;
				break;
			case SDL_QUIT:
				close();
				break;
			default:
				break;
			}
		}
		if (s_mouse_delta.x != 0.0f || s_mouse_delta.y != 0.0f)
		{
			mouse_moved(s_mouse_delta.x, s_mouse_delta.y);
		}

		// Update the WORLD->VIEW matrix.
		sCameraEntity->get_view_matrix(s_matrix_V);
	}

	void renderer::begin_geometry_pass()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, s_fbo);
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_FINAL_COLOR);
		glClear(GL_COLOR_BUFFER_BIT);
		constexpr GLenum DrawBuffers[4] = 
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
		// method which is (or rather should be) called at the end of the main render loop.
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

	void renderer::blit_drawbuffers_to_screen(FontStream& stream)
	{
		// Take s_fbo as the active framebuffer.
		// We blit the geometry stuff into yet another color attachment.
		// This final color attachment gets blitted to the screen in the update()
		// method which is (or rather should be) called at the end of the main render loop.
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
		stream->position.x += 1.0f;
		stream << "GBUFFER_DIFFUSE" << std::endl;
		stream->position.y -= 1.0f;
		stream << "GBUFFER_SPECULAR" << std::endl;
		stream->position.x -= 1.0f;
		stream << "GBUFFER_POSITION" << std::endl;

		glEnable(GL_CULL_FACE);
	}

	void renderer::ambient_light_pass() noexcept
	{
		const auto& s = get_lp_ambient_shader();
		s.activate();
		s.set_gbuffer_diffuse(GBUFFER_DIFFUSE);
		s.set_viewport_size(viewport_size());
		s.set_light_intensity(vec4f(1.0f, 1.0f, 1.0f, 1.0f));
		get_unit_quad_P().draw();
	}

	void renderer::update_matrix_P()
	{
		if (s_matrix_P_dirty)
		{
			s_matrix_P = sCameraEntity->camera->projectionMatrix();
			s_matrix_P_dirty = false;
		}
	}

	void renderer::update_matrix_VM()
	{
		if (s_matrix_VM_dirty)
		{
			s_matrix_VM = s_matrix_V * s_matrix_M;
			s_matrix_VM_dirty = false;
		}
	}

	void renderer::update_matrix_PVM()
	{
		update_matrix_P();
		update_matrix_VM();
		if (s_matrix_PVM_dirty)
		{
			s_matrix_PVM = s_matrix_P * s_matrix_VM;
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

	void renderer::init_shaders()
	{
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
			s_gp_shader = new gp_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load gp_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_gp_d_shader = new gp_d_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load gp_d_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_gp_s_shader = new gp_s_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load gp_s_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_gp_n_shader = new gp_n_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load gp_n_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_gp_ds_shader = new gp_ds_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load gp_ds_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_gp_dn_shader = new gp_dn_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load gp_dn_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_gp_sn_shader = new gp_sn_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load gp_sn_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_gp_dsn_shader = new gp_dsn_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load gp_dsn_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_gpi_shader = new gpi_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load gpi_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_gpi_d_shader = new gpi_d_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load gpi_d_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_gpi_s_shader = new gpi_s_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load gpi_s_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_gpi_n_shader = new gpi_n_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load gpi_n_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_gpi_ds_shader = new gpi_ds_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load gpi_ds_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_gpi_dn_shader = new gpi_dn_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load gpi_dn_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_gpi_sn_shader = new gpi_sn_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load gpi_sn_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_gpi_dsn_shader = new gpi_dsn_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load gpi_dsn_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_lp_ambient_shader = new lp_ambient_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load lp_ambient_shader: ");
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
			s_lp_spot_shader = new lp_spot_shader();	
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load lp_spot_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_sp_directional_shader = new sp_directional_shader();
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load sp_directional_shader: ");
			e.prepend(name());
			throw;
		}
		try
		{
			s_skybox_shader = new skybox_shader();	
		}
		catch (exception& e)
		{
			e.prepend(": Failed to load skybox_shader: ");
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

} // namespace gintonic