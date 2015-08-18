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
		class unit_cube_P;
		class unit_cube_P_flipped;
		class unit_sphere_P;
		class unit_cone_P;
	}

	class matrix_PVM_shader;

	class gp_shader;
	class gp_d_shader;
	class gp_s_shader;
	class gp_n_shader;
	class gp_ds_shader;
	class gp_dn_shader;
	class gp_sn_shader;
	class gp_dsn_shader;
	
	class gpi_shader;
	class gpi_d_shader;
	class gpi_s_shader;
	class gpi_n_shader;
	class gpi_ds_shader;
	class gpi_dn_shader;
	class gpi_sn_shader;
	class gpi_dsn_shader;

	class lp_ambient_shader;
	class lp_directional_shader;
	class lp_point_shader;
	class lp_spot_shader;

	class skybox_shader;

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

		inline static const mat4f& matrix_P() BOOST_NOEXCEPT_OR_NOTHROW
		{
			update_matrix_P();
			return s_matrix_P;
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

		inline static const matrix_PVM_shader& get_null_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_matrix_PVM_shader;
		}
	
		inline static const gp_shader& get_gp_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_gp_shader;
		}

		inline static const gp_d_shader& get_gp_d_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_gp_d_shader;
		}

		inline static const gp_s_shader& get_gp_s_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_gp_s_shader;
		}

		inline static const gp_n_shader& get_gp_n_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_gp_n_shader;
		}

		inline static const gp_ds_shader& get_gp_ds_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_gp_ds_shader;
		}

		inline static const gp_dn_shader& get_gp_dn_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_gp_dn_shader;
		}

		inline static const gp_sn_shader& get_gp_sn_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_gp_sn_shader;
		}

		inline static const gp_dsn_shader& get_gp_dsn_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_gp_dsn_shader;
		}

		inline static const gpi_shader& get_gpi_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_gpi_shader;
		}

		inline static const gpi_d_shader& get_gpi_d_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_gpi_d_shader;
		}

		inline static const gpi_s_shader& get_gpi_s_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_gpi_s_shader;
		}

		inline static const gpi_n_shader& get_gpi_n_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_gpi_n_shader;
		}

		inline static const gpi_ds_shader& get_gpi_ds_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_gpi_ds_shader;
		}

		inline static const gpi_dn_shader& get_gpi_dn_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_gpi_dn_shader;
		}

		inline static const gpi_sn_shader& get_gpi_sn_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_gpi_sn_shader;
		}

		inline static const gpi_dsn_shader& get_gpi_dsn_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_gpi_dsn_shader;
		}

		inline static const lp_ambient_shader& get_lp_ambient_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_lp_ambient_shader;
		}

		inline static const lp_directional_shader& get_lp_directional_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_lp_directional_shader;
		}

		inline static const lp_point_shader& get_lp_point_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_lp_point_shader;
		}

		inline static const lp_spot_shader& get_lp_spot_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_lp_spot_shader;
		}

		inline static const skybox_shader& get_skybox_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_skybox_shader;
		}

		inline static text_shader* get_text_shader() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return s_text_shader;
		}

		inline static const opengl::unit_quad_P& get_unit_quad_P() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_unit_quad_P;
		}

		inline static const opengl::unit_cube_P& get_unit_cube_P() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_unit_cube_P;
		}

		inline static const opengl::unit_cube_P_flipped& get_unit_cube_P_flipped() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_unit_cube_P_flipped;
		}

		inline static const opengl::unit_sphere_P& get_unit_sphere_P() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_unit_sphere_P;
		}

		inline static const opengl::unit_cone_P& get_unit_cone_P() BOOST_NOEXCEPT_OR_NOTHROW
		{
			return *s_unit_cone_P;
		}

	private:

		static void update_matrix_P();
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

		static bool s_matrix_P_dirty;
		static bool s_matrix_VM_dirty;
		static bool s_matrix_PVM_dirty;
		static bool s_matrix_N_dirty;

		static mat4f s_matrix_P;
		static mat4f s_matrix_M;
		static mat4f s_matrix_VM;
		static mat4f s_matrix_PVM;
		static mat3f s_matrix_N;

		static GLuint s_fbo;
		static GLuint s_textures[GBUFFER_COUNT];
		static GLuint s_depth_texture;

		static const camera_transform<float>* s_camera;

		static matrix_PVM_shader* s_matrix_PVM_shader;

		static gp_shader* s_gp_shader;
		static gp_d_shader* s_gp_d_shader;
		static gp_s_shader* s_gp_s_shader;
		static gp_n_shader* s_gp_n_shader;
		static gp_ds_shader* s_gp_ds_shader;
		static gp_dn_shader* s_gp_dn_shader;
		static gp_sn_shader* s_gp_sn_shader;
		static gp_dsn_shader* s_gp_dsn_shader;
		
		static gpi_shader* s_gpi_shader;
		static gpi_d_shader* s_gpi_d_shader;
		static gpi_s_shader* s_gpi_s_shader;
		static gpi_n_shader* s_gpi_n_shader;
		static gpi_ds_shader* s_gpi_ds_shader;
		static gpi_dn_shader* s_gpi_dn_shader;
		static gpi_sn_shader* s_gpi_sn_shader;
		static gpi_dsn_shader* s_gpi_dsn_shader;

		static lp_ambient_shader* s_lp_ambient_shader;
		static lp_directional_shader* s_lp_directional_shader;
		static lp_point_shader* s_lp_point_shader;
		static lp_spot_shader* s_lp_spot_shader;

		static skybox_shader* s_skybox_shader;

		static text_shader* s_text_shader;

		static opengl::unit_quad_P* s_unit_quad_P;
		static opengl::unit_cube_P* s_unit_cube_P;
		static opengl::unit_cube_P_flipped* s_unit_cube_P_flipped;
		static opengl::unit_sphere_P* s_unit_sphere_P;
		static opengl::unit_cone_P* s_unit_cone_P;
	};
} // namespace gintonic

#endif