/**
 * @file renderer.hpp
 * @brief Defines the renderer singleton class.
 * @author Raoul Wols
 */

#pragma once

#include "../Foundation/locks.hpp"

#include "../Math/mat3f.hpp"
#include "../Math/mat4f.hpp"

#include "OpenGL/khrplatform.hpp"

#include "fonts.hpp"

#include <chrono>
#include <boost/circular_buffer.hpp>
#include <boost/signals2.hpp>

namespace gintonic  {

class unit_quad_P; // Forward declaration.
class unit_cube_P; // Forward declaration.
class unit_cube_P_flipped; // Forward declaration.
class unit_sphere_P; // Forward declaration.
class unit_cone_P; // Forward declaration.

class Mesh; // Forward declaration.

class entity; // Forward declaration.

class matrix_PVM_shader; // Forward declaration.

class gp_shader; // Forward declaration.
class gp_d_shader; // Forward declaration.
class gp_s_shader; // Forward declaration.
class gp_n_shader; // Forward declaration.
class gp_ds_shader; // Forward declaration.
class gp_dn_shader; // Forward declaration.
class gp_sn_shader; // Forward declaration.
class gp_dsn_shader; // Forward declaration.

class gpi_shader; // Forward declaration.
class gpi_d_shader; // Forward declaration.
class gpi_s_shader; // Forward declaration.
class gpi_n_shader; // Forward declaration.
class gpi_ds_shader; // Forward declaration.
class gpi_dn_shader; // Forward declaration.
class gpi_sn_shader; // Forward declaration.
class gpi_dsn_shader; // Forward declaration.

class lp_ambient_shader; // Forward declaration.
class lp_directional_shader; // Forward declaration.
class lp_point_shader; // Forward declaration.
class lp_spot_shader; // Forward declaration.

class sp_directional_shader; // Forward declaration.

class skybox_shader; // Forward declaration.

class text_shader; // Forward declaration.

/**
 * @brief The renderer class. This is a singleton class.
 * @details The renderer has the following duties:
 * * Manages the OpenGL context.
 * * Manages the window.
 * * Manages the width and height of the window.
 * * Receives keyboard input and mouse input.
 * * Manages the geometry buffers.
 * * Manages the `MODEL->WORLD`, the `WORLD-VIEW`, the `VIEW->CLIP`, the
 *   `MODEL->VIEW` and `MODEL->CLIP` matrices.
 * * Records the delta time between frames.
 * * Manages the entity that acts as the camera.
 * * Manages all shader classes. Essentially just instantiates one instance
 *   of every shader class. You can obtain a reference to any shader class
 *   via the renderer singleton class.
 * * Manages a few basic shapes like a cube, a sphere and so on.
 *
 * This is quite a large list of duties so I am inclined to split up this
 * singleton class. However it works fine at the moment, and some might say
 * "keep it simple, stupid", abbreviated KISS.
 *
 * Note that *all* methods are static methods. To initialize the renderer,
 * you call the renderer::init function. You do not have to call any sort of
 * clean-up function. If you just need an OpenGL context and not all of the
 * fancy stuff that the renderer also initializes, you can use the function
 * renderer::init_dummy.
 */
class renderer
{
public:

	/// The clock type to record the delta time and elapsed time.
	typedef std::chrono::high_resolution_clock clock_type;

	/// The time point type to record the delta time and elapsed time.
	typedef clock_type::time_point time_point_type;

	/// The duration type to record the delta time and elapsed time.
	typedef clock_type::duration duration_type;

	/**
	 * @name Initialization and Shutdown Procedures
	 * 
	 * Functionality relating to initialization of the renderer and shutdown
	 * mechanisms of the renderer. Context focusing is also present here.
	 */
	
	///@{

	/**
	 * @brief Initialize the renderer.
	 * @param title The window title.
	 * @param camera The camera entity to start out with. It is perfectly
	 * possible to never change this.
	 * @param fullscreen Wether we should open a fullscreen window or not.
	 * @param width The preferred width of the window. Note that if the 
	 * fullscreen parameter is set to true, then the current resolution is
	 * taken instead and this parameter is ignored.
	 * @param height The preferred height of the window. Note that if the
	 * fullscreen parameter is set to true, then the current resolution is
	 * taken instead and this parameter is ignored.
	 */
	static void init(
		const char* title, 
		std::shared_ptr<entity> cameraEntity, 
		const bool fullscreen, 
		const int width, 
		const int height);

	/**
	 * @brief Only initialize the OpenGL context and optionally initialize
	 * the shaders too.
	 * @param construct_shaders Set this to true to initialize all the shaders
	 * too. If set to false, the shaders are not initialized. The default
	 * is false.
	 */
	static void init_dummy(const bool construct_shaders = false);

	/**
	 * @brief Check wether the renderer is initialized.
	 * @return True if the renderer is initialized, false if not.
	 */
	static bool is_initialized() noexcept;

	/**
	 * @brief Focus the OpenGL context on the current thread.
	 */
	static void focus_context() noexcept;

	/**
	 * @brief Show the window of the renderer.
	 * @details When you initialize the renderer with renderer::initialize,
	 * the renderer does not show its window after the initialization. You
	 * have to explicitly call the renderer::show function to show the
	 * window. This is because some after the renderer has initialized, it can
	 * be beneficial to do some more OpenGL initialization work that is not
	 * part of the initialization stage of the renderer. However, you cannot
	 * operate on OpenGL objects without an OpenGL context, and the renderer
	 * provides such an OpenGL context.
	 */
	static void show() noexcept;

	/**
	 * @brief Hide the window of the renderer.
	 */
	static void hide() noexcept;

	/**
	 * @brief Request to close the renderer.
	 * @details When this function is called, the renderer is not actually
	 * yet terminated. The renderer does some internal cleanup and then sets
	 * a boolean variable that indicates wether the renderer should close.
	 * You can check wether to stop the renderer with renderer::should_close.
	 * When renderer::should_close returns false, you can stop the render
	 * loop.
	 */
	static void close() noexcept;

	/**
	 * @brief Check wether you should stop the render loop.
	 * @return True if you should stop the render loop, false if not.
	 */
	static bool should_close() noexcept;

	///@}
	
	/**
	 * @name Time Management
	 * 
	 * Functionality to obtain the delta time and the elapsed time.
	 */

	///@{

	/**
	 * @brief Get the delta time between two frames.
	 * @return The delta time between two frames.
	 */
	inline static duration_type delta_time() noexcept 
	{
		return s_delta_time;
	}

	/**
	 * @brief Get the elapsed time since the renderer initialized.
	 * @return The elapsed time since the renderer initialized.
	 */
	inline static duration_type elapsed_time() noexcept
	{
		return s_elapsed_time;
	}

	/**
	 * @brief Get the elapsed and delta time in seconds.
	 * 
	 * @param [out] elapsedTime This variable is set to the current elapsed time.
	 * @param [out] deltaTime This variable is set to the delta time of the previous frame.
	 */
	static void getElapsedAndDeltaTime(double& elapsedTime, double& deltaTime);

	///@}
	
	/**
	 * @name Camera, Matrices and Viewport Management
	 * 
	 * Functionality relating to the camera entity, the global matrices
	 * and the viewport.
	 */
	
	///@{

	/**
	 * @brief Set the camera.
	 * @details The renderer will update the `WORLD->VIEW` matrix using the
	 * global transformation matrix of the given entity.
	 * @param e The entity that will act as a camera.
	 */
	static void setCameraEntity(std::shared_ptr<entity> cameraEntity);

	template <class ForwardIter>
	static void submitEntities(ForwardIter first, ForwardIter last)
	{
		sEntityQueueLock.obtain();
		std::copy(first, last, std::back_inserter(sFutureQueue));
		sEntityQueueLock.release();
	}
	
	/**
	 * @brief Get the current camera entity.
	 * @return A reference to the current camera entity.
	 */
	inline static std::shared_ptr<entity> getCameraEntity() noexcept 
	{ 
		return sCameraEntity; 
	}

	/**
	 * @brief Get the viewport width.
	 * @return The viewport width.
	 */
	inline static int width() noexcept 
	{ 
		return s_width; 
	}

	/**
	 * @brief Get the viewport height.
	 * @return The viewport height.
	 */
	inline static int height() noexcept 
	{ 
		return s_height; 
	}

	/**
	 * @brief Get the aspect ratio.
	 * @details The aspect ratio is the width divided by the height.
	 * @return The aspect ratio.
	 */
	inline static float aspectratio() noexcept 
	{ 
		return s_aspectratio; 
	}

	/**
	 * Get the viewport width and height as a vec2f.
	 * @return The viewport width and height as a vec2f.
	 */
	static vec2f viewport_size() noexcept;

	/**
	 * @brief Resize the viewport.
	 * @details Resizing the viewport has a consequence that various geometry
	 * buffers are also resized.
	 * @param width The new width of the viewport.
	 * @param height The new height of the viewport.
	 */
	static void resize(const int width, const int height);


	/**
	 * @brief Get the `VIEW->CLIP` matrix.
	 * @return A constant reference to the `VIEW->CLIP` matrix.
	 */
	inline static const mat4f& matrix_P() noexcept
	{
		update_matrix_P();
		return s_matrix_P;
	}

	/**
	 * @brief Get the `WORLD->VIEW` matrix.
	 * @return A constant reference to the `WORLD->VIEW` matrix.
	 */
	inline static const mat4f& matrix_V() noexcept
	{
		return s_matrix_V;
	}

	/**
	 * @brief Get the `MODEL->WORLD` matrix.
	 * @return A constant reference to the `MODEL->WORLD` matrix.
	 */
	inline static const mat4f& matrix_M() noexcept
	{
		return s_matrix_M;
	}

	/**
	 * @brief Get the `MODEL->VIEW` matrix.
	 * @return A constant reference to the `MODEL->VIEW` matrix.
	 */
	inline static const mat4f& matrix_VM() noexcept
	{
		update_matrix_VM();
		return s_matrix_VM;
	}

	/**
	 * @brief Get `MODEL->CLIP` matrix.
	 * @return A constant reference to the `MODEL->CLIP` matrix.
	 */
	inline static const mat4f& matrix_PVM() noexcept
	{
		update_matrix_PVM();
		return s_matrix_PVM;
	}

	/**
	 * @brief Get the `NORMAL` matrix.
	 * @return A constant reference to the `NORMAL` matrix.
	 */
	inline static const mat3f& matrix_N() noexcept
	{
		update_matrix_N();
		return s_matrix_N;
	}

	/**
	 * @brief Set the `MODEL->WORLD` matrix.
	 * @details All the other matrices get updated automatically when you
	 * set the `MODEL->WORLD` matrix.
	 * @tparam Args Template parameter pack.
	 * @param args Constructor arguments for a mat4f.
	 */
	template <class ...Args> static void set_model_matrix(Args&&... args)
	{
		s_matrix_VM_dirty = true;
		s_matrix_PVM_dirty = true;
		s_matrix_N_dirty = true;
		s_matrix_M = mat4f(std::forward<Args>(args)...);
	}

	///@}

	/**
	 * @name Keyboard and Mouse Management
	 * 
	 * Functionality relating to the keyboard and mouse input.
	 */

	///@{

	/**
	 * @brief Set the cursor position relative to the viewport.
	 * @details The dimensions are normalized to [-1,+1] x [-1,+1]. This means
	 * that this functionality is independent of the width and height of the
	 * viewport.
	 * @param x The X-coordinate.
	 * @param y The Y-coordinate.
	 */
	static void set_cursor_position(
		const double x, 
		const double y) noexcept;

	/**
	 * @brief Set wether the renderer should let the cursor act in a freeform
	 * way.
	 * @details Normally, the cursor can get stuck at the edges of the
	 * viewport. When we are in this situation, no more mouse delta is
	 * recorded in the respective dimension. This behaviour can be unwanted
	 * when we are, for example, modeling a "first-person" camera. You can
	 * use this function to set the cursor in "freeform" mode. In this mode,
	 * the cursor is not bounded by the viewport and any mouse movement is
	 * correctly recorded each frame in a mouse delta.
	 * @param b Set wether to enable the freeform cursor mode.
	 */
	static void set_freeform_cursor(const bool b);

	/**
	 * @brief Disable the cursor.
	 */
	static void disable_cursor() noexcept;

	/**
	 * @brief Enable the cursor.
	 */
	static void enable_cursor() noexcept;

	/**
	 * @brief Center the cursor to the center of the viewport.
	 */
	static void center_cursor() noexcept;

	/**
	 * @brief Check if a given key is pressed down.
	 * @param keycode The SDL scancode. See scancodes.hpp.
	 * @return True if the key is pressed down, false if not.
	 */
	static bool key(const int keycode) noexcept;

	/*
	 * @brief Check if a given key was pressed down in the previous frame.
	 * @param keycode The SDL scancode. See scancodes.hpp.
	 * @return True if the key was pressed down in the previous frame, false
	 * if not.
	 */
	static bool key_prev(const int keycode) noexcept;

	/**
	 * @brief Check if a given toggle key is pressed down.
	 * @param keycode The SDL scancode. See scancodes.hpp.
	 * @return True if the toggle key was pressed down. False if not.
	 */
	static bool key_toggle_press(const int keycode) noexcept;

	/**
	 * @brief Check if a given toggle key is released.
	 * @param keycode The SDL scancode. See scancodes.hpp.
	 * @return True if the toggle key was released. False if not.
	 */
	static bool key_toggle_release(const int keycode) 
		noexcept;

	/**
	 * @brief Check if a given mouse button is pressed.
	 * @param buttoncode The SDL scancode. See scancodes.hpp.
	 * @return True if the mouse button is pressed. False if not.
	 */
	static bool mousebutton(const int buttoncode) noexcept;
	
	/**
	 * @brief Get the previous frame's mouse delta.
	 * @return The previous frame's mouse delta.
	 */
	inline static const vec2f& mouse_delta() noexcept
	{
		return s_mouse_delta;
	}

	///@}

	/**
	 * @name Render state
	 * 
	 * Functions to change the state of the renderer. You will probably work
	 * with these functions most of the time.
	 */

	///@{

	/**
	 * @brief Enable or disable wireframe mode.
	 * 
	 * @param yesOrNo True if you want to render geometry in wireframe mode,
	 * false if you want geometry to be rendered normally.
	 */
	static void setWireframeMode(const bool yesOrNo) noexcept;

	/**
	 * @brief Query if the renderer is rendering in wireframe mode.
	 * @return True if the renderer is rendering in wireframe mode,
	 * false if the renderer is rendering normally.
	 */
	static bool getWireframeMode() noexcept
	{
		return sRenderInWireframeMode;
	}

	/**
	 * @brief Enable or disable virtual synchronization.
	 * @param b True to enable, false to disable.
	 */
	static void vsync(const bool b);

	#ifdef ENABLE_DEBUG_TRACE
	/**
	 * @brief Output to a debug stream on the renderer's viewport.
	 * @return A reference to a FontStream.
	 */
	static FontStream& cerr();
	#endif

	/**
	 * @brief Update the renderer.
	 * @details You need to call this method at the end of your render loop.
	 */
	static void update() noexcept;

	/**
	 * @deprecated
	 */
	static void upload_matrix_PVM();

	/**
	 * @deprecated
	 */
	static void upload_matrix_VM();

	/**
	 * @deprecated
	 */
	static void upload_matrix_N();

	/**
	 * @deprecated
	 */
	static void upload_all_matrices();


	/**
	 * @brief Get the name of the hardware (or software) renderer.
	 * @return The name of the hardware (or software) renderer.
	 */
	static const char* name();

	/**
	 * @brief Get the version of the renderer.
	 * @return The version of the renderer.
	 */
	static const char* version();

	/**
	 * @brief The geometry buffers.
	 */
	enum GBUFFER
	{
		GBUFFER_POSITION = 0,
		GBUFFER_DIFFUSE,
		GBUFFER_SPECULAR,
		GBUFFER_NORMAL,
		GBUFFER_FINAL_COLOR,
		GBUFFER_COUNT
	};

	/**
	 * @brief The texture image units that you should set when binding a
	 * texture in a material.
	 */
	enum GBUFFER_IN
	{
		GBUFFER_TEX_DIFFUSE = 0,
		GBUFFER_TEX_SPECULAR,
		GBUFFER_TEX_NORMAL
	};

	/**
	 * @brief Start the geometry pass.
	 */
	static void begin_geometry_pass();

	/**
	 * @brief Start a stencil pass.
	 */
	static void begin_stencil_pass();

	/**
	 * @brief Start the light pass.
	 */
	static void begin_light_pass();
	
	/**
	 * @brief Set the read buffer.
	 * @param type The read buffer.
	 */
	static void set_read_buffer(const enum GBUFFER type);

	/**
	 * @brief This will split the screen up into four parts that show
	 * the raw geometry buffers.
	 */
	static void blit_drawbuffers_to_screen();

	/**
	 * @brief This will split the screen up itno four parts that show the
	 * raw geometry buffers.
	 * @param f A FontStream to denote the names of the geometry buffers.
	 */
	static void blit_drawbuffers_to_screen(FontStream& f);

	/**
	 * @brief Do an ambient light pass. This should be called after the call
	 * to renderer::begin_light_pass.
	 */
	static void ambient_light_pass() noexcept;

	///@}

	/**
	 * @name Events
	 * 
	 * The events that the renderer may fire.
	 */

	///@{

	/// Event that fires when a character on the keyboard is typed.
	static boost::signals2::signal<void(wchar_t)> char_typed;

	/// Event that fires when the mouse has scrolled.
	static boost::signals2::signal<void(double, double)> mouse_scrolled;

	/// Event that fires when the mouse has moved.
	static boost::signals2::signal<void(double, double)> mouse_moved;

	/// Event that fires when a key is pressed.
	static boost::signals2::signal<void(int, int, int, int)> key_pressed;

	/// Event that fires when a mouse button is pressed.
	static boost::signals2::signal<void(int, int, int)> mouse_pressed;

	/// Event that fires when the window is resized.
	static boost::signals2::signal<void(int, int)> window_resized;

	/// Event that fires when the mouse has entered the window.
	static boost::signals2::signal<void(void)> mouse_entered;

	/// Event that fires when the mouse has left the window.
	static boost::signals2::signal<void(void)> mouse_left;

	/// Event that fires when the renderer is about to close.
	static boost::signals2::signal<void(void)> about_to_close;

	///@}

	/**
	 * @name Shader Access
	 * 
	 * The renderer houses all of the shaders.
	 */
	
	///@{

	/**
	 * @brief Get a shader that does nothing.
	 * @return A shader that does nothing.
	 */
	inline static const matrix_PVM_shader& get_null_shader() 
		noexcept
	{
		return *s_matrix_PVM_shader;
	}

	/**
	 * @brief Get gp_shader.
	 * @return gp_shader.
	 */
	inline static const gp_shader& get_gp_shader() noexcept
	{
		return *s_gp_shader;
	}

	/**
	 * @brief Get gp_d_shader.
	 * @return gp_d_shader.
	 */
	inline static const gp_d_shader& get_gp_d_shader() noexcept
	{
		return *s_gp_d_shader;
	}

	/**
	 * @brief Get gp_s_shader.
	 * @return gp_s_shader.
	 */
	inline static const gp_s_shader& get_gp_s_shader() noexcept
	{
		return *s_gp_s_shader;
	}

	/**
	 * @brief Get gp_n_shader.
	 * @return gp_n_shader.
	 */
	inline static const gp_n_shader& get_gp_n_shader() noexcept
	{
		return *s_gp_n_shader;
	}

	/**
	 * @brief Get gp_ds_shader.
	 * @return gp_ds_shader.
	 */
	inline static const gp_ds_shader& get_gp_ds_shader() noexcept
	{
		return *s_gp_ds_shader;
	}

	/**
	 * @brief Get gp_dn_shader.
	 * @return gp_dn_shader.
	 */
	inline static const gp_dn_shader& get_gp_dn_shader() noexcept
	{
		return *s_gp_dn_shader;
	}

	/**
	 * @brief Get gp_sn_shader.
	 * @return gp_sn_shader.
	 */
	inline static const gp_sn_shader& get_gp_sn_shader() noexcept
	{
		return *s_gp_sn_shader;
	}

	/**
	 * @brief Get gp_dsn_shader.
	 * @return gp_dsn_shader.
	 */
	inline static const gp_dsn_shader& get_gp_dsn_shader() noexcept
	{
		return *s_gp_dsn_shader;
	}

	/**
	 * @brief Get gpi_shader.
	 * @return gpi_shader.
	 */
	inline static const gpi_shader& get_gpi_shader() noexcept
	{
		return *s_gpi_shader;
	}

	/**
	 * @brief Get gpi_d_shader.
	 * @return gpi_d_shader.
	 */
	inline static const gpi_d_shader& get_gpi_d_shader() noexcept
	{
		return *s_gpi_d_shader;
	}

	/**
	 * @brief Get gpi_s_shader.
	 * @return gpi_s_shader.
	 */
	inline static const gpi_s_shader& get_gpi_s_shader() noexcept
	{
		return *s_gpi_s_shader;
	}

	/**
	 * @brief Get gpi_n_shader.
	 * @return gpi_n_shader.
	 */
	inline static const gpi_n_shader& get_gpi_n_shader() noexcept
	{
		return *s_gpi_n_shader;
	}

	/**
	 * @brief Get gpi_ds_shader.
	 * @return gpi_ds_shader.
	 */
	inline static const gpi_ds_shader& get_gpi_ds_shader() noexcept
	{
		return *s_gpi_ds_shader;
	}

	/**
	 * @brief Get gpi_dn_shader.
	 * @return gpi_dn_shader.
	 */
	inline static const gpi_dn_shader& get_gpi_dn_shader() noexcept
	{
		return *s_gpi_dn_shader;
	}

	/**
	 * @brief Get gpi_sn_shader.
	 * @return gpi_sn_shader.
	 */
	inline static const gpi_sn_shader& get_gpi_sn_shader() noexcept
	{
		return *s_gpi_sn_shader;
	}

	/**
	 * @brief Get gpi_dsn_shader.
	 * @return gpi_dsn_shader.
	 */
	inline static const gpi_dsn_shader& get_gpi_dsn_shader() noexcept
	{
		return *s_gpi_dsn_shader;
	}

	/**
	 * @brief Get lp_ambient_shader.
	 * @return lp_ambient_shader.
	 */
	inline static const lp_ambient_shader& get_lp_ambient_shader() noexcept
	{
		return *s_lp_ambient_shader;
	}

	/**
	 * @brief Get lp_directional_shader.
	 * @return lp_directional_shader.
	 */
	inline static const lp_directional_shader& get_lp_directional_shader() noexcept
	{
		return *s_lp_directional_shader;
	}

	/**
	 * @brief Get lp_point_shader.
	 * @return lp_point_shader.
	 */
	inline static const lp_point_shader& get_lp_point_shader() noexcept
	{
		return *s_lp_point_shader;
	}

	/**
	 * @brief Get lp_spot_shader.
	 * @return lp_spot_shader.
	 */
	inline static const lp_spot_shader& get_lp_spot_shader() noexcept
	{
		return *s_lp_spot_shader;
	}

	inline static const sp_directional_shader& get_sp_directional_shader() noexcept
	{
		return *s_sp_directional_shader;
	}

	/**
	 * @brief Get lp_skybox_shader.
	 * @return lp_skybox_shader.
	 */
	inline static const skybox_shader& get_skybox_shader() noexcept
	{
		return *s_skybox_shader;
	}

	/**
	 * @brief Get text_shader.
	 * @return text_shader.
	 */
	inline static text_shader* get_text_shader() noexcept
	{
		return s_text_shader;
	}

	///@}
	
	/**
	 * @name Basic Mesh Shape Access
	 * 
	 * The renderer houses some basic shapes. You can access them with these
	 * functions.
	 */
	
	///@{

	/**
	 * Get unit_quad_P.
	 * @return unit_quad_P
	 */
	inline static unit_quad_P& get_unit_quad_P() noexcept
	{
		return *s_unit_quad_P;
	}

	/**
	 * Get unit_cube_P.
	 * @return unit_cube_P
	 */
	inline static unit_cube_P& get_unit_cube_P() noexcept
	{
		return *s_unit_cube_P;
	}

	/**
	 * Get unit_cube_P_flipped.
	 * @return unit_cube_P_flipped
	 */
	inline static unit_cube_P_flipped& get_unit_cube_P_flipped() noexcept
	{
		return *s_unit_cube_P_flipped;
	}

	/**
	 * Get unit_sphere_P.
	 * @return unit_sphere_P
	 */
	inline static unit_sphere_P& get_unit_sphere_P() noexcept
	{
		return *s_unit_sphere_P;
	}

	/**
	 * Get unit_cone_P.
	 * @return unit_cone_P
	 */
	inline static unit_cone_P& get_unit_cone_P() noexcept
	{
		return *s_unit_cone_P;
	}

	inline static std::shared_ptr<Mesh> getUnitQuad() noexcept
	{
		return sUnitQuadPUN;
	}

	inline static std::shared_ptr<Mesh> getUnitCube() noexcept
	{
		return sUnitCubePUN;
	}

	inline static std::shared_ptr<Mesh> getUnitSphere() noexcept
	{
		return sUnitSpherePUN;
	}

	///@}

private:

	static void update_matrix_P();
	static void update_matrix_VM();
	static void update_matrix_PVM();
	static void update_matrix_N();

	static void init_shaders();
	static void initializeBasicShapes();

	static void release();
	
	static bool s_should_close;
	static bool s_fullscreen;
	static bool sRenderInWireframeMode;
	static int s_width;
	static int s_height;
	static float s_aspectratio;

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
	static mat4f s_matrix_V;
	static mat4f s_matrix_M;
	static mat4f s_matrix_VM;
	static mat4f s_matrix_PVM;
	static mat3f s_matrix_N;

	static GLuint s_fbo;
	static GLuint s_textures[GBUFFER_COUNT];
	static GLuint s_depth_texture;
	static GLuint s_shadow_texture;

	static std::shared_ptr<entity> sCameraEntity;

	static write_lock sEntityQueueLock;
	static std::vector<std::shared_ptr<entity>> sFutureQueue;
	static std::vector<std::shared_ptr<entity>> sCurrentQueue;

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

	static sp_directional_shader* s_sp_directional_shader;

	static skybox_shader* s_skybox_shader;

	static text_shader* s_text_shader;

	static unit_quad_P* s_unit_quad_P;
	static unit_cube_P* s_unit_cube_P;
	static unit_cube_P_flipped* s_unit_cube_P_flipped;
	static unit_sphere_P* s_unit_sphere_P;
	static unit_cone_P* s_unit_cone_P;

	static std::shared_ptr<Mesh> sUnitQuadPUN;
	static std::shared_ptr<Mesh> sUnitCubePUN;
	static std::shared_ptr<Mesh> sUnitSpherePUN;
};

} // namespace gintonic
