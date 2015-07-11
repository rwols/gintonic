#ifndef gintonic_renderer_linux_hpp
#define gintonic_renderer_linux_hpp


#include <chrono>
#include <boost/circular_buffer.hpp>
#include <boost/signals2.hpp>
#include "opengl.hpp"

namespace gintonic 
{
	class renderer
	{
	public:
		typedef std::chrono::high_resolution_clock clock_type;
		typedef clock_type::time_point time_point_type;
		typedef clock_type::duration duration_type;

		static void init(const char* title, const camera_transform<float>& cam, const bool fullscreen, const int width, const int height);
		inline static duration_type delta_time() BOOST_NOEXCEPT_OR_NOTHROW { return s_delta_time; }
		inline static duration_type elapsed_time() BOOST_NOEXCEPT_OR_NOTHROW { return s_elapsed_time; }
		static bool is_initialized() BOOST_NOEXCEPT_OR_NOTHROW{ return s_window != nullptr; }
		static void set_camera(const camera_transform<float>& c);
		inline static const camera_transform<float>& camera() BOOST_NOEXCEPT_OR_NOTHROW { return *s_camera; }
		inline static int width() BOOST_NOEXCEPT_OR_NOTHROW { return s_width; }
		inline static int height() BOOST_NOEXCEPT_OR_NOTHROW{ return s_height; }
		void static resize(const int width, const int height);
		inline static void focus_context() BOOST_NOEXCEPT_OR_NOTHROW{ SDL_GL_MakeCurrent(s_window, s_context); }
		inline static void set_cursor_position(const double x, const double y) BOOST_NOEXCEPT_OR_NOTHROW
		{
			SDL_WarpMouseInWindow(s_window, (int)x, (int)y);
			SDL_FlushEvent(SDL_MOUSEMOTION);
		}
		inline static void set_freeform_cursor(const bool b)
		{
			SDL_SetRelativeMouseMode(b? SDL_TRUE : SDL_FALSE);
		}
		inline static void disable_cursor() BOOST_NOEXCEPT_OR_NOTHROW{ SDL_ShowCursor(0); }
		inline static void enable_cursor() BOOST_NOEXCEPT_OR_NOTHROW{ SDL_ShowCursor(1); }
		inline static void center_cursor() BOOST_NOEXCEPT_OR_NOTHROW
		{
			set_cursor_position(s_width / 2, s_height / 2);
		}
		inline static void vsync(const bool b) { SDL_GL_SetSwapInterval(b? 1 : 0); }
		inline static void show() BOOST_NOEXCEPT_OR_NOTHROW { SDL_ShowWindow(s_window); }
		inline static void hide() BOOST_NOEXCEPT_OR_NOTHROW { SDL_HideWindow(s_window); }
		inline static void close() BOOST_NOEXCEPT_OR_NOTHROW
		{
			s_should_close = true;
			about_to_close();
		}
		inline static bool should_close() BOOST_NOEXCEPT_OR_NOTHROW{ return s_should_close; }
		inline static float aspect_ratio() BOOST_NOEXCEPT_OR_NOTHROW{ return s_aspect_ratio; }
		inline static bool key(const int keycode) BOOST_NOEXCEPT_OR_NOTHROW
		{
			return static_cast<bool>(s_key_state[keycode]);
		}
		inline static bool key_prev(const int keycode) BOOST_NOEXCEPT_OR_NOTHROW
		{
			return static_cast<bool>(s_key_prev_state[keycode]);
		}
		inline static bool key_toggle_press(const int keycode) BOOST_NOEXCEPT_OR_NOTHROW
		{
			return key(keycode) && !key_prev(keycode);
		}
		inline static bool key_toggle_release(const int keycode) BOOST_NOEXCEPT_OR_NOTHROW
		{
			return !key(keycode) && key_prev(keycode);
		}
		inline static bool mousebutton(const int buttoncode) BOOST_NOEXCEPT_OR_NOTHROW
		{
			return (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(buttoncode));
		}
		inline static const vec2f& mouse_delta() BOOST_NOEXCEPT_OR_NOTHROW{ return s_mouse_delta; }
		inline static const mat4f& matrix_M() BOOST_NOEXCEPT_OR_NOTHROW{ return s_matrix_M; }
		inline static const mat4f& matrix_VM() BOOST_NOEXCEPT_OR_NOTHROW
		{
			update_matrix_VM();
			return s_matrix_VM;
		}
		inline static const mat4f& matrix_PVM() BOOST_NOEXCEPT_OR_NOTHROW
		{
			update_matrix_PVM();
			return s_matrix_PVM;
		}
		inline static const mat3f& matrix_N() BOOST_NOEXCEPT_OR_NOTHROW
		{
			update_matrix_N();
			return s_matrix_N;
		}
		template <class ...Args> static void set_model_matrix(Args&&... args)
		{
			s_matrix_VM_dirty = true;
			s_matrix_PVM_dirty = true;
			s_matrix_N_dirty = true;
			s_matrix_M = mat4f(std::forward<Args>(args)...);
		}

		static void update() BOOST_NOEXCEPT_OR_NOTHROW;

		static void upload_matrix_PVM();
		static void upload_matrix_VM();
		static void upload_matrix_N();
		static void upload_all_matrices();

		static boost::signals2::signal<void(wchar_t)> char_typed;
		static boost::signals2::signal<void(double, double)> mouse_scrolled;
		static boost::signals2::signal<void(double, double)> mouse_moved;
		static boost::signals2::signal<void(int, int, int, int)> key_pressed;
		static boost::signals2::signal<void(int, int, int)> mouse_pressed;
		static boost::signals2::signal<void(int, int)> window_resized;
		static boost::signals2::signal<void(void)> mouse_entered;
		static boost::signals2::signal<void(void)> mouse_left;
		static boost::signals2::signal<void(void)> about_to_close;

		static const char* name();
		static const char* version();

		enum class TEXTURE_TYPE : char
		{
			position,
			diffuse,
			normal,
			texcoord,
			num_textures
		};

		static void bind_for_writing();
		static void bind_for_reading();

	private:

		static void update_matrix_VM();
		static void update_matrix_PVM();
		static void update_matrix_N();

		static void release();

		static SDL_Window* s_window;
		static SDL_GLContext s_context;
		static SDL_Event s_event;
		
		static bool s_should_close;
		static bool s_fullscreen;
		static int s_width;
		static int s_height;
		static float s_aspect_ratio;

		static time_point_type s_start_time;
		static duration_type s_delta_time;
		static duration_type s_prev_elapsed_time;
		static duration_type s_elapsed_time;
		static boost::circular_buffer<duration_type> s_circle_buffer;
		static vec2f s_mouse_delta;

		static const Uint8* s_key_state;
		static Uint8* s_key_prev_state;
		static int s_key_state_count;

		static bool s_matrix_VM_dirty;
		static bool s_matrix_PVM_dirty;
		static bool s_matrix_N_dirty;

		static mat4f s_matrix_M;
		static mat4f s_matrix_VM;
		static mat4f s_matrix_PVM;
		static mat3f s_matrix_N;

		GLuint s_fbo;
		GLuint s_textures[TEXTURE_TYPE::num_textures];
		GLuint s_depth_texture;

		static const camera_transform<float>* s_camera;
	};
} // namespace gintonic

#endif