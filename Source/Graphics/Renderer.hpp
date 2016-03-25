/**
 * @file Renderer.hpp
 * @brief Defines the Renderer singleton class.
 * @author Raoul Wols
 */

#pragma once

#include "../ForwardDeclarations.hpp"

#include "../Foundation/WriteLock.hpp"

#include "../Math/vec3f.hpp"
#include "../Math/mat3f.hpp"
#include "../Math/mat4f.hpp"

#include "OpenGL/khrplatform.hpp"

#include "Font.hpp"

#include <chrono>
#include <boost/circular_buffer.hpp>
#include <boost/signals2.hpp>

namespace gintonic  {

/**
 * @brief The Renderer class. This is a singleton class.
 * @details The Renderer has the following duties:
 * * Manages the OpenGL context.
 * * Manages the window.
 * * Manages the width and height of the window.
 * * Receives keyboard input and mouse input.
 * * Manages the geometry buffers.
 * * Manages the `MODEL->WORLD`, the `WORLD-VIEW`, the `VIEW->CLIP`, the
 *   `MODEL->VIEW` and `MODEL->CLIP` matrices.
 * * Records the delta time between frames.
 * * Manages the Entity that acts as the camera.
 * * Manages all shader classes. Essentially just instantiates one instance
 *   of every shader class. You can obtain a reference to any shader class
 *   via the Renderer singleton class.
 * * Manages a few basic shapes like a cube, a sphere and so on.
 *
 * This is quite a large list of duties so I am inclined to split up this
 * singleton class. However it works fine at the moment, and some might say
 * "keep it simple, stupid", abbreviated KISS.
 *
 * Note that *all* methods are static methods. To initialize the Renderer,
 * you call the Renderer::init function. You do not have to call any sort of
 * clean-up function. If you just need an OpenGL context and not all of the
 * fancy stuff that the Renderer also initializes, you can use the function
 * Renderer::init_dummy.
 */
class Renderer
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
	 * Functionality relating to initialization of the Renderer and shutdown
	 * mechanisms of the Renderer. Context focusing is also present here.
	 */
	
	///@{

	/**
	 * @brief Initialize the Renderer.
	 * @param title The window title.
	 * @param camera The camera Entity to start out with. It is perfectly
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
		std::shared_ptr<Entity> cameraEntity, 
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
	static void initDummy(const bool construct_shaders = false);

	/**
	 * @brief Check wether the Renderer is initialized.
	 * @return True if the Renderer is initialized, false if not.
	 */
	static bool isInitialized() noexcept;

	/**
	 * @brief Focus the OpenGL context on the current thread.
	 */
	static void focusContext() noexcept;

	/**
	 * @brief Show the window of the Renderer.
	 * @details When you initialize the Renderer with Renderer::initialize,
	 * the Renderer does not show its window after the initialization. You
	 * have to explicitly call the Renderer::show function to show the
	 * window. This is because some after the Renderer has initialized, it can
	 * be beneficial to do some more OpenGL initialization work that is not
	 * part of the initialization stage of the Renderer. However, you cannot
	 * operate on OpenGL objects without an OpenGL context, and the Renderer
	 * provides such an OpenGL context.
	 */
	static void show() noexcept;

	/**
	 * @brief Hide the window of the Renderer.
	 */
	static void hide() noexcept;

	/**
	 * @brief Request to close the Renderer.
	 * @details When this function is called, the Renderer is not actually
	 * yet terminated. The Renderer does some internal cleanup and then sets
	 * a boolean variable that indicates wether the Renderer should close.
	 * You can check wether to stop the Renderer with Renderer::shouldClose.
	 * When Renderer::shouldClose returns false, you can stop the render
	 * loop.
	 */
	static void close() noexcept;

	/**
	 * @brief Check wether you should stop the render loop.
	 * @return True if you should stop the render loop, false if not.
	 */
	static bool shouldClose() noexcept;

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
	inline static duration_type deltaTime() noexcept 
	{
		return sDeltaTime;
	}

	/**
	 * @brief Get the elapsed time since the Renderer initialized.
	 * @return The elapsed time since the Renderer initialized.
	 */
	inline static duration_type elapsedTime() noexcept
	{
		return sElapsedTime;
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
	 * Functionality relating to the camera Entity, the global matrices
	 * and the viewport.
	 */
	
	///@{

	/**
	 * @brief Set the camera.
	 * @details The Renderer will update the `WORLD->VIEW` matrix using the
	 * global transformation matrix of the given Entity.
	 * @param e The Entity that will act as a camera.
	 */
	static void setCameraEntity(std::shared_ptr<Entity> cameraEntity);

	// template <class ForwardIter>
	// static void submitEntities(ForwardIter first, ForwardIter last)
	// {
	// 	sEntityQueueLock.obtain();
	// 	std::copy(first, last, std::back_inserter(sFutureQueue));
	// 	sEntityQueueLock.release();
	// }

	static void submitEntityRecursive(std::shared_ptr<Entity> toSubmit);
	
	/**
	 * @brief Get the current camera Entity.
	 * @return A reference to the current camera Entity.
	 */
	inline static std::shared_ptr<Entity> getCameraEntity() noexcept 
	{ 
		return sCameraEntity; 
	}

	/**
	 * @brief Get the current camera position in world coordinates.
	 * @return The camera position in world coordinates.
	 */
	inline static vec3f getCameraPosition() noexcept
	{
		return sCameraPosition;
	}

	/**
	 * @brief Get the viewport width.
	 * @return The viewport width.
	 */
	inline static int width() noexcept 
	{ 
		return sWidth; 
	}

	/**
	 * @brief Get the viewport height.
	 * @return The viewport height.
	 */
	inline static int height() noexcept 
	{ 
		return sHeight; 
	}

	/**
	 * @brief Get the aspect ratio.
	 * @details The aspect ratio is the width divided by the height.
	 * @return The aspect ratio.
	 */
	inline static float aspectRatio() noexcept 
	{ 
		return sAspectRatio; 
	}

	/**
	 * Get the viewport width and height as a vec2f.
	 * @return The viewport width and height as a vec2f.
	 */
	static vec2f viewportSize() noexcept;

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
	template <class ...Args> static void setModelMatrix(Args&&... args)
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
	static void setCursorPosition(
		const double x, 
		const double y) noexcept;

	/**
	 * @brief Set wether the Renderer should let the cursor act in a freeform
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
	static void setFreeformCursor(const bool b);

	/**
	 * @brief Disable the cursor.
	 */
	static void disableCursor() noexcept;

	/**
	 * @brief Enable the cursor.
	 */
	static void enableCursor() noexcept;

	/**
	 * @brief Center the cursor to the center of the viewport.
	 */
	static void centerCursor() noexcept;

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
	static bool keyPrev(const int keycode) noexcept;

	/**
	 * @brief Check if a given toggle key is pressed down.
	 * @param keycode The SDL scancode. See scancodes.hpp.
	 * @return True if the toggle key was pressed down. False if not.
	 */
	static bool keyTogglePress(const int keycode) noexcept;

	/**
	 * @brief Check if a given toggle key is released.
	 * @param keycode The SDL scancode. See scancodes.hpp.
	 * @return True if the toggle key was released. False if not.
	 */
	static bool keyToggleRelease(const int keycode) 
		noexcept;

	/**
	 * @brief Check if a given mouse button is pressed.
	 * @param buttoncode The SDL scancode. See scancodes.hpp.
	 * @return True if the mouse button is pressed. False if not.
	 */
	static bool mouseButton(const int buttoncode) noexcept;
	
	/**
	 * @brief Get the previous frame's mouse delta.
	 * @return The previous frame's mouse delta.
	 */
	inline static const vec2f& mouseDelta() noexcept
	{
		return sMouseDelta;
	}

	///@}

	/**
	 * @name Render state
	 * 
	 * Functions to change the state of the Renderer. You will probably work
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
	 * @brief Query if the Renderer is rendering in wireframe mode.
	 * @return True if the Renderer is rendering in wireframe mode,
	 * false if the Renderer is rendering normally.
	 */
	inline static bool getWireframeMode() noexcept
	{
		return sRenderInWireframeMode;
	}

	/**
	 * @brief Enable or disable viewing the raw geometry buffers.
	 * 
	 * @param yesOrNo True if you want to check the raw geometry buffers,
	 * false if you want things to be rendered normally.
	 */
	static void setViewGeometryBuffers(const bool yesOrNo) noexcept;

	/**
	 * @brief Query if the renderer is rendering the raw geometry buffers.
	 * @return True if the renderer is rendering the raw geometry buffers,
	 * false otherwise.
	 */
	inline static bool getViewGeometryBuffers() noexcept
	{
		return sViewGeometryBuffers;
	}

	static void setViewCameraDepthBuffer(const bool yesOrNo) noexcept;

	inline static bool getViewCameraDepthBuffer() noexcept
	{
		return sViewCameraDepthBuffer;
	}

	static void setEntityDebugShadowBuffer(std::shared_ptr<Entity> entityWithShadowBuffer) noexcept
	{
		sDebugShadowBufferEntity = std::move(entityWithShadowBuffer);
	}

	inline static std::shared_ptr<Entity> getEntityDebugShadowBuffer() noexcept
	{
		return sDebugShadowBufferEntity;
	}

	/**
	 * @brief Enable or disable virtual synchronization.
	 * @param b True to enable, false to disable.
	 */
	static void vsync(const bool b);

	/**
	 * @brief Output to a debug stream on the Renderer's viewport. The color is red.
	 * @return A reference to a FontStream.
	 */
	static FontStream& cerr();

	/**
	 * @brief Output to a debug stream on the Renderer's viewport. The color is white.
	 * @return A reference to a FontStream.
	 */
	static FontStream& cout();

	/**
	 * @brief Update the Renderer.
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
	 * @brief Get the name of the hardware (or software) Renderer.
	 * @return The name of the hardware (or software) Renderer.
	 */
	static const char* name();

	/**
	 * @brief Get the version of the Renderer.
	 * @return The version of the Renderer.
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
	// static void begin_geometry_pass();

	/**
	 * @brief Start a stencil pass.
	 */
	// static void begin_stencil_pass();

	/**
	 * @brief Start the light pass.
	 */
	// static void begin_light_pass();
	
	/**
	 * @brief Set the read buffer.
	 * @param type The read buffer.
	 */
	// static void set_read_buffer(const enum GBUFFER type);

	/**
	 * @brief This will split the screen up into four parts that show
	 * the raw geometry buffers.
	 */
	// static void blit_drawbuffers_to_screen();

	/**
	 * @brief This will split the screen up itno four parts that show the
	 * raw geometry buffers.
	 * @param f A FontStream to denote the names of the geometry buffers.
	 */
	// static void blit_drawbuffers_to_screen(FontStream& f);

	/**
	 * @brief Do an ambient light pass. This should be called after the call
	 * to Renderer::begin_light_pass.
	 */
	// static void ambient_light_pass() noexcept;

	///@}

	/**
	 * @name Events
	 * 
	 * The events that the Renderer may fire.
	 */

	///@{

	/// Event that fires when a character on the keyboard is typed.
	static boost::signals2::signal<void(wchar_t)> onCharTyped;

	/// Event that fires when the mouse has scrolled.
	static boost::signals2::signal<void(double, double)> onMouseScroll;

	/// Event that fires when the mouse has moved.
	static boost::signals2::signal<void(double, double)> onMouseMove;

	/// Event that fires when a key is pressed.
	static boost::signals2::signal<void(int, int, int, int)> onKeyPress;

	/// Event that fires when a mouse button is pressed.
	static boost::signals2::signal<void(int, int, int)> onMousePressed;

	/// Event that fires when the window is resized.
	static boost::signals2::signal<void(int, int)> onWindowResize;

	/// Event that fires when the mouse has entered the window.
	static boost::signals2::signal<void(void)> onMouseEnter;

	/// Event that fires when the mouse has left the window.
	static boost::signals2::signal<void(void)> onMouseLeave;

	/// Event that fires when the Renderer is about to close.
	static boost::signals2::signal<void(void)> onAboutToClose;

	///@}

	/**
	 * @name Basic Mesh Shape Access
	 * 
	 * The Renderer houses some basic shapes. You can access them with these
	 * functions.
	 */
	
	///@{

	inline static std::shared_ptr<Mesh> getUnitQuad() noexcept
	{
		return sUnitQuadPUN;
	}

	inline static std::shared_ptr<Mesh> getUnitCube() noexcept
	{
		return sUnitCubePUN;
	}

	inline static std::shared_ptr<Mesh> getInsideOutUnitCube() noexcept
	{
		return sFlippedUnitCubePUN;
	}

	inline static std::shared_ptr<Mesh> getUnitCubeWithTangents() noexcept
	{
		return sUnitCubePUNTB;
	}

	inline static std::shared_ptr<Mesh> getUnitSphere() noexcept
	{
		return sUnitSpherePUN;
	}

	inline static std::shared_ptr<Mesh> getUnitCone() noexcept
	{
		return sUnitConePUN;
	}

	inline static std::shared_ptr<Mesh> getUnitCylinder() noexcept
	{
		return sUnitCylinderPUN;
	}

	static std::shared_ptr<Entity> createGizmo();

	///@}

private:

	static void update_matrix_P();
	static void update_matrix_VM();
	static void update_matrix_PVM();
	static void update_matrix_N();

	static void init_shaders();
	static void initializeBasicShapes();

	static void release();
	
	static bool sShouldClose;
	static bool sFullscreen;
	static bool sRenderInWireframeMode;
	static bool sViewGeometryBuffers;
	static bool sViewCameraDepthBuffer;
	static int sWidth;
	static int sHeight;
	static float sAspectRatio;

	static time_point_type sStartTime;
	static duration_type sDeltaTime;
	static duration_type sPrevElapsedTime;
	static duration_type sElapsedTime;
	static boost::circular_buffer<duration_type> s_circle_buffer;
	static vec2f sMouseDelta;

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

	static std::shared_ptr<Entity> sCameraEntity;
	static std::shared_ptr<Entity> sDebugShadowBufferEntity;
	static vec3f sCameraPosition;

	static std::shared_ptr<Mesh> sUnitQuadPUN;
	static std::shared_ptr<Mesh> sUnitCubePUN;
	static std::shared_ptr<Mesh> sUnitCubePUNTB;
	static std::shared_ptr<Mesh> sFlippedUnitCubePUN;
	static std::shared_ptr<Mesh> sUnitSpherePUN;
	static std::shared_ptr<Mesh> sUnitConePUN;
	static std::shared_ptr<Mesh> sUnitCylinderPUN;

	static void prepareRendering() noexcept;
	static void renderGeometry() noexcept;
	static void renderShadows() noexcept;
	static void renderLights() noexcept;
	static void finalizeRendering() noexcept;
	static void processEvents() noexcept;

};

} // namespace gintonic
