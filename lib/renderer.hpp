#ifndef gintonic_renderer_linux_hpp
#define gintonic_renderer_linux_hpp

#include <chrono>
#include <boost/circular_buffer.hpp>
#include <boost/signals2.hpp>
#include "opengl.hpp"
#include "config.hpp"

#ifdef ENABLE_DEBUG_TRACE
	#include "fonts.hpp"
#endif

namespace gintonic 
{
	namespace opengl
	{
		class unit_quad_P;
		class unit_sphere_P;
	}

	class geometry_null_shader;
	class matrix_PVM_shader;
	class gp_c_shader;
	class gp_cd_shader;
	class gp_cds_shader;
	class gp_cdn_shader;

	class gp_dc_shader;
	class gp_dcsc_shader;
	class gp_dt_shader;
	class gp_dcdt_shader;

	class geometry_pass_shader;
	class lp_null_shader;
	class lp_directional_shader;
	class lp_point_shader;
	class directional_light_pass_shader;
	class text_shader;

	class renderer
	{
	public:
		typedef std::chrono::high_resolution_clock clock_type;
		typedef clock_type::time_point time_point_type;
		typedef clock_type::duration duration_type;

		static void init(const char* title, const camera_transform<float>& cam, const bool fullscreen, const int width, const int height);
		inline static duration_type delta_time() BOOST_NOEXCEPT_OR_NOTHROW { return s_delta_time; }
		inline static duration_type elapsed_time() BOOST_NOEXCEPT_OR_NOTHROW { return s_elapsed_time; }
		static bool is_initialized() BOOST_NOEXCEPT_OR_NOTHROW;
		static void set_camera(const camera_transform<float>& c);
		inline static const camera_transform<float>& camera() BOOST_NOEXCEPT_OR_NOTHROW { return *s_camera; }
		inline static int width() BOOST_NOEXCEPT_OR_NOTHROW { return s_width; }
		inline static int height() BOOST_NOEXCEPT_OR_NOTHROW { return s_height; }
		void static resize(const int width, const int height);
		static void focus_context() BOOST_NOEXCEPT_OR_NOTHROW;
		static void set_cursor_position(const double x, const double y) BOOST_NOEXCEPT_OR_NOTHROW;
		static void set_freeform_cursor(const bool b);
		static void disable_cursor() BOOST_NOEXCEPT_OR_NOTHROW;
		static void enable_cursor() BOOST_NOEXCEPT_OR_NOTHROW;
		static void center_cursor() BOOST_NOEXCEPT_OR_NOTHROW;
		static void vsync(const bool b);
		static void show() BOOST_NOEXCEPT_OR_NOTHROW;
		static void hide() BOOST_NOEXCEPT_OR_NOTHROW;
		static void close() BOOST_NOEXCEPT_OR_NOTHROW;
		static bool should_close() BOOST_NOEXCEPT_OR_NOTHROW;
		static float aspect_ratio() BOOST_NOEXCEPT_OR_NOTHROW;
		static bool key(const int keycode) BOOST_NOEXCEPT_OR_NOTHROW;
		static bool key_prev(const int keycode) BOOST_NOEXCEPT_OR_NOTHROW;
		static bool key_toggle_press(const int keycode) BOOST_NOEXCEPT_OR_NOTHROW;
		static bool key_toggle_release(const int keycode) BOOST_NOEXCEPT_OR_NOTHROW;
		static bool mousebutton(const int buttoncode) BOOST_NOEXCEPT_OR_NOTHROW;
		inline static const vec2f& mouse_delta() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return s_mouse_delta;
		}

		inline static const mat4f& matrix_M() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return s_matrix_M;
		}

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

		#ifdef ENABLE_DEBUG_TRACE
		static fontstream& cerr();
		#endif

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

		enum GBUFFER
		{
			GBUFFER_POSITION = 0,
			GBUFFER_DIFFUSE,
			GBUFFER_SPECULAR,
			GBUFFER_NORMAL,
			GBUFFER_FINAL_COLOR,
			GBUFFER_COUNT
		};

		enum GBUFFER_IN
		{
			GBUFFER_TEX_DIFFUSE = 0,
			GBUFFER_TEX_SPECULAR,
			GBUFFER_TEX_NORMAL
		};

		static void begin_geometry_pass();
		static void begin_stencil_pass();
		static void begin_light_pass();
		
		static void set_read_buffer(const enum GBUFFER type);
		static void blit_drawbuffers_to_screen();
		static void blit_drawbuffers_to_screen(fontstream&);
		static void null_light_pass() BOOST_NOEXCEPT_OR_NOTHROW;

		inline static geometry_null_shader* get_geometry_null_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return s_geometry_null_shader;
		}
		inline static const matrix_PVM_shader& get_null_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_matrix_PVM_shader;
		}
		inline static gp_c_shader* get_gp_c_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return s_gp_c_shader;
		}

		inline static const gp_cd_shader& get_gp_cd_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_gp_cd_shader;
		}

		inline static const gp_cds_shader& get_gp_cds_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_gp_cds_shader;
		}

		inline static const gp_cdn_shader& get_gp_cdn_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_gp_cdn_shader;
		}

		inline static const gp_dc_shader& get_gp_dc_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_gp_dc_shader;
		}

		inline static const gp_dcsc_shader& get_gp_dcsc_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_gp_dcsc_shader;
		}

		inline static const gp_dt_shader& get_gp_dt_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_gp_dt_shader;
		}

		inline static const gp_dcdt_shader& get_gp_dcdt_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_gp_dcdt_shader;
		}

		inline static geometry_pass_shader* get_geometry_pass_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return s_geometry_pass_shader;
		}

		inline static const lp_null_shader& get_lp_null_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_lp_null_shader;
		}

		inline static const lp_directional_shader& get_lp_directional_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_lp_directional_shader;
		}

		inline static const lp_point_shader& get_lp_point_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_lp_point_shader;
		}

		inline static directional_light_pass_shader* get_directional_light_pass_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return s_directional_light_pass_shader;
		}

		inline static text_shader* get_text_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return s_text_shader;
		}

		inline static const opengl::unit_quad_P& get_unit_quad_P() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_unit_quad_P;
		}

		inline static const opengl::unit_sphere_P& get_unit_sphere_P() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_unit_sphere_P;
		}

	private:

		static void update_matrix_VM();
		static void update_matrix_PVM();
		static void update_matrix_N();

		static void release();
		
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

		static bool s_matrix_VM_dirty;
		static bool s_matrix_PVM_dirty;
		static bool s_matrix_N_dirty;

		static mat4f s_matrix_M;
		static mat4f s_matrix_VM;
		static mat4f s_matrix_PVM;
		static mat3f s_matrix_N;

		static GLuint s_fbo;
		static GLuint s_textures[GBUFFER_COUNT];
		static GLuint s_depth_texture;

		static const camera_transform<float>* s_camera;

		static geometry_null_shader* s_geometry_null_shader;
		static matrix_PVM_shader* s_matrix_PVM_shader;
		static gp_c_shader* s_gp_c_shader;
		static gp_cd_shader* s_gp_cd_shader;
		static gp_cds_shader* s_gp_cds_shader;
		static gp_cdn_shader* s_gp_cdn_shader;

		static gp_dc_shader* s_gp_dc_shader;
		static gp_dcsc_shader* s_gp_dcsc_shader;
		static gp_dt_shader* s_gp_dt_shader;
		static gp_dcdt_shader* s_gp_dcdt_shader;

		static geometry_pass_shader* s_geometry_pass_shader;
		static lp_null_shader* s_lp_null_shader;
		static lp_directional_shader* s_lp_directional_shader;
		static lp_point_shader* s_lp_point_shader;
		static directional_light_pass_shader* s_directional_light_pass_shader;
		static text_shader* s_text_shader;

		static opengl::unit_quad_P* s_unit_quad_P;
		static opengl::unit_sphere_P* s_unit_sphere_P;
	};
} // namespace gintonic

#endif