#include "Graphics/Renderer.hpp"

#include "Graphics/GUI/Base.hpp"

#include "Foundation/exception.hpp"
#include "Foundation/Octree.hpp"
#include "Math/vec4f.hpp"
#include "Math/MatrixPipeline.hpp"

#include "Graphics/AnimationClip.hpp"
#include "Graphics/GeometryBuffer.hpp"
#include "Graphics/Mesh.hpp"
#include "Graphics/ShaderPrograms.hpp"
#include "Graphics/Material.hpp"
#include "Graphics/Light.hpp"
#include "Graphics/PointLight.hpp"
#include "Graphics/SpotLight.hpp"
#include "Graphics/Skeleton.hpp"
#include "Graphics/ShadowBuffer.hpp"

#include "Camera.hpp"
#include "Entity.hpp"
#include "EntityVisitor.hpp"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif // __clang__
#include "SDL.h"
#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__

#include <iostream>

#ifdef BOOST_MSVC
	#include <Objbase.h> // for CoInitializeEx
#endif

#define NUM_SUBDIVISIONS 2
#define PREFERRED_LINE_WIDTH 4

#define FONT_FILE_LOCATION "assets/fonts/Inconsolata-Regular.ttf"

#define HAS_DIFFUSE_TEXTURE         1
#define HAS_SPECULAR_TEXTURE        2
#define HAS_NORMAL_TEXTURE          4
#define HAS_TANGENTS_AND_BITANGENTS 8
#define MESH_HAS_JOINTS             16
#define INSTANCED_RENDERING         32

#define GBUFFER_TEX_DIFFUSE 0
#define GBUFFER_TEX_SPECULAR 1
#define GBUFFER_TEX_NORMAL 2

#define DEPTH_TEXTURE_UNIT 4

#define DEBUG_DRAW_SKELETONS

namespace gintonic {

// Anybody want to make the Renderer a proper singleton class?

GUI::Base* sGUIRoot = nullptr;

GeometryBuffer* sGeometryBuffer = nullptr;

std::shared_ptr<Camera> sDefaultCamera = Camera::create("DefaultCamera");

std::vector<std::shared_ptr<Entity>> sShadowCastingLightEntities;
std::vector<std::shared_ptr<Entity>> sShadowCastingPointLightEntities;
std::vector<std::shared_ptr<Entity>> sShadowCastingGeometryEntities;
std::vector<std::shared_ptr<Entity>> sNonShadowCastingLightEntities;
std::vector<std::shared_ptr<Entity>> sNonShadowCastingGeometryEntities;

WriteLock sEntitiesLock;

MaterialShaderProgram* sMaterialShaderProgram = nullptr;

class EntitySorter : public EntityVisitor
{
public:
	EntitySorter(std::shared_ptr<Entity> root) 
	: EntityVisitor(root)
	{
		sEntitiesLock.obtain();
	}

	virtual ~EntitySorter()
	{
		sEntitiesLock.release();
	}

	virtual bool onVisit(std::shared_ptr<Entity> entity)
	{
		if (entity->castShadow)
		{
			if (entity->light)
			{
				// Catch SpotLight before PointLight. This is needed
				// because SpotLight inherits from PointLight. Otherwise
				// every SpotLight would also match the dynamic cast to a PointLight.
				// That is not what we want.
				if (dynamic_cast<SpotLight*>(entity->light.get()))
				{
					sShadowCastingLightEntities.push_back(entity);
					if (!entity->shadowBuffer)
					{
						entity->light->initializeShadowBuffer(*entity);
					}
					assert(entity->shadowBuffer);
				}
				// Point lights need to be treated separately.
				else if (dynamic_cast<PointLight*>(entity->light.get()))
				{
					sShadowCastingPointLightEntities.push_back(entity);
				}
				// Treat all other light types as if they apply the
				// shadow map algorithm.
				else
				{
					sShadowCastingLightEntities.push_back(entity);
					if (!entity->shadowBuffer)
					{
						entity->light->initializeShadowBuffer(*entity);
					}
					assert(entity->shadowBuffer);
				}
			}
			if (entity->material && entity->mesh)
			{
				sShadowCastingGeometryEntities.push_back(entity);
			}
		}
		else // non-shadow casting entity
		{
			if (entity->light)
			{
				sNonShadowCastingLightEntities.push_back(entity);
				if (entity->shadowBuffer)
				{
					// Non-shadow casting light entity
					// But it has a shadow buffer. We can remove that.
					entity->shadowBuffer.reset();
					assert(!entity->shadowBuffer);
				}
			}
			if (entity->material && entity->mesh)
			{
				sNonShadowCastingGeometryEntities.push_back(entity);
			}
		}
		return true;
	}
};

// ALL the global variables.

std::shared_ptr<Font> sDebugFont = nullptr;
FontStream* sDebugErrorStream = nullptr;
FontStream* sDebugLogStream = nullptr;

SDL_Window* sWindow = nullptr;
SDL_GLContext sContext;
SDL_Event sEvent = SDL_Event();

bool Renderer::sShouldClose = false;
bool Renderer::sFullscreen = false;
bool Renderer::sRenderInWireframeMode = false;
bool Renderer::sViewGeometryBuffers = false;
bool Renderer::sViewCameraDepthBuffer = false;
int Renderer::sWidth = 800;
int Renderer::sHeight = 640;
float Renderer::sAspectRatio = (float)Renderer::sWidth / (float)Renderer::sHeight;

Renderer::time_point_type Renderer::sStartTime;
Renderer::duration_type Renderer::sDeltaTime = Renderer::duration_type();
Renderer::duration_type Renderer::sPrevElapsedTime = Renderer::duration_type();
Renderer::duration_type Renderer::sElapsedTime = Renderer::duration_type();
boost::circular_buffer<Renderer::duration_type> Renderer::s_circle_buffer = boost::circular_buffer<Renderer::duration_type>();
vec2f Renderer::sMouseDelta = vec2f(0.0f, 0.0f);
vec2f Renderer::sMouseWheel = vec2f(0.0f, 0.0f);
vec4f Renderer::sFingerMotion = vec4f(0.0f, 0.0f, 0.0f, 0.0f);

Uint8* sKeyPrevState = nullptr;
const Uint8* sKeyState = nullptr;
int sKeyStateCount;

MatrixPipeline sMatrixPipeline;

bool Renderer::sMatrixPDirty = true;
bool Renderer::sMatrixVMDirty = true;
bool Renderer::sMatrixPVMDirty = true;
bool Renderer::sMatrixNDirty = true;

mat4f Renderer::sMatrixP = mat4f(1.0f);
mat4f Renderer::sMatrixV = mat4f(1.0f);
mat4f Renderer::sMatrixM = mat4f(1.0f);
mat4f Renderer::sMatrixVM = mat4f(1.0f);
mat4f Renderer::sMatrixPVM = mat4f(1.0f);
mat3f Renderer::sMatrixN = mat3f(1.0f);

std::vector<mat4f, allocator<mat4f>> Renderer::sMatrix44Array = std::vector<mat4f, allocator<mat4f>>(1 * GT_SKELETON_MAX_JOINTS);
std::vector<vec4f, allocator<vec4f>> Renderer::sMatrix33Array = std::vector<vec4f, allocator<vec4f>>(3 * GT_SKELETON_MAX_JOINTS);

OpenGL::BufferObject* Renderer::sMatrix44UniformBuffer = nullptr;
OpenGL::BufferObject* Renderer::sMatrix33UniformBuffer = nullptr;

std::shared_ptr<Entity> Renderer::sCameraEntity = std::shared_ptr<Entity>(nullptr);
std::shared_ptr<Entity> Renderer::sDebugShadowBufferEntity = std::shared_ptr<Entity>(nullptr);
const Octree* Renderer::sOctreeRoot = nullptr;
vec3f Renderer::sCameraPosition = vec3f(0.0f, 0.0f, 0.0f);

std::shared_ptr<Mesh> Renderer::sUnitQuadPUN        = nullptr;
std::shared_ptr<Mesh> Renderer::sUnitCubePUN        = nullptr;
std::shared_ptr<Mesh> Renderer::sUnitCubePUNTB      = nullptr;
std::shared_ptr<Mesh> Renderer::sFlippedUnitCubePUN = nullptr;
std::shared_ptr<Mesh> Renderer::sUnitSpherePUN      = nullptr;
std::shared_ptr<Mesh> Renderer::sUnitConePUN        = nullptr;
std::shared_ptr<Mesh> Renderer::sUnitCylinderPUN    = nullptr;
GLuint sPackedQuadVAO = 0;
GLuint sPackedQuadVBO = 0;

boost::signals2::signal<void(wchar_t)> Renderer::onCharTyped;
boost::signals2::signal<void(double, double)> Renderer::onMouseScroll;
boost::signals2::signal<void(const vec2f&)> Renderer::onMouseMove;
boost::signals2::signal<void(const vec2f&)> Renderer::onMouseWheel;
boost::signals2::signal<void(const vec4f&)> Renderer::onFingerMotion;
boost::signals2::signal<void(int, int, int)> Renderer::onKeyPress;
boost::signals2::signal<void(int, int, int)> Renderer::onKeyRelease;
boost::signals2::signal<void(int, int, int)> Renderer::onMousePressed;
boost::signals2::signal<void(int, int)> Renderer::onWindowResize;
boost::signals2::signal<void(void)> Renderer::onMouseEnter;
boost::signals2::signal<void(void)> Renderer::onMouseLeave;
boost::signals2::signal<void(char[32])> Renderer::onTextInput;
boost::signals2::signal<void(char[32], int, int)> Renderer::onTextEdit;
boost::signals2::signal<void(void)> Renderer::onAboutToClose;

bool bindUniformBlock(const OpenGL::ShaderProgram& program, const OpenGL::BufferObject& buffer, const char* uniformBlockName)
{
	const auto lIndex = glGetUniformBlockIndex(program, uniformBlockName);
	if (lIndex == GL_INVALID_INDEX) return false;
	glUniformBlockBinding(program, lIndex, buffer);
	return true;
}

FontStream& Renderer::cerr() { return *sDebugErrorStream; }
FontStream& Renderer::cout() { return *sDebugLogStream;   }

void Renderer::getElapsedAndDeltaTime(double& t, double& dt)
{
	using std::chrono::duration_cast;
	using std::chrono::nanoseconds;
	t = static_cast<double>(duration_cast<nanoseconds>(elapsedTime()).count()) / double(1e9);
	dt = static_cast<double>(duration_cast<nanoseconds>(deltaTime()).count()) / double(1e9);
}

void Renderer::initialize(
		const char* windowTitle, 
		std::shared_ptr<Entity> cameraEntity, 
		const bool fullscreen, 
		const int preferredWidth, 
		const int preferredHeight,
		const bool initializeGeometryBuffer,
		const bool initializeShaderPrograms,
		const bool initializeBasicMeshes)
{
	bool lWasAlreadyInitialized;
	if (isInitialized())
	{
		lWasAlreadyInitialized = true;
		release();
	}
	else
	{
		lWasAlreadyInitialized = false;
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0) 
		{
			throw InitException(SDL_GetError());
		}
	}

	sFullscreen = fullscreen;

	if (sFullscreen)
	{
		const auto lNumDisplays = SDL_GetNumVideoDisplays();
		if (lNumDisplays <= 0)
		{
			throw NoDisplaysException();
		}
		SDL_Rect lRect;
		SDL_GetDisplayBounds(0, &lRect);
		sWidth = lRect.w;
		sHeight = lRect.h;
	}
	else
	{
		sWidth = preferredWidth;
		sHeight = preferredHeight;
	}

	sAspectRatio = (float) sWidth / (float) sHeight;

	if (lWasAlreadyInitialized == false) std::atexit(SDL_Quit);

	Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
	if (sFullscreen) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	sWindow = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, sWidth, sHeight, flags);

	//
	// We want an OpenGL 3.3 core profile context.
	//
	SDL_GL_ResetAttributes();
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	sContext = SDL_GL_CreateContext(sWindow);
	if (!sContext)
	{
		release();
		throw NoContextAvailableException(3, 3);
	}

	if (gladLoadGL() != 1)
	{
		release();
		throw FunctionLoadException();
	}

	SDL_GetKeyboardState(&sKeyStateCount);
	sKeyPrevState = new Uint8[sKeyStateCount];
	if (!sKeyPrevState)
	{
		throw std::bad_alloc();
	}
	std::memset(sKeyPrevState, 0, sizeof(Uint8) * sKeyStateCount);
	sKeyState = SDL_GetKeyboardState(nullptr);
	
	if (lWasAlreadyInitialized == false) std::atexit(Renderer::release);
	
	vsync(false);
	glClearColor(0.03f, 0.03f, 0.03f, 0.0f);
	glClearDepth(1.0f);
	
	sStartTime = clock_type::now();

	#ifdef BOOST_MSVC
	CoInitialize(nullptr); // initialize COM
	#endif

	//
	// Initialize framebuffers
	//
	if (initializeGeometryBuffer)
	{
		sGeometryBuffer = new GeometryBuffer(sWidth, sHeight);
	}
	else
	{
		if (sGeometryBuffer) delete sGeometryBuffer;
		sGeometryBuffer = nullptr;
	}

	//
	// Initialize default camera and camera entity
	//
	sDefaultCamera->name = "DefaultCamera";
	sDefaultCamera->setWidth(static_cast<float>(sWidth));
	sDefaultCamera->setHeight(static_cast<float>(sHeight));
	sDefaultCamera->setNearPlane(0.5f);
	sDefaultCamera->setFarPlane(100.0f);
	sDefaultCamera->setProjectionType(Camera::kPerspective);
	if (!cameraEntity) cameraEntity = Entity::create("DefaultCamera");
	setCameraEntity(std::move(cameraEntity));

	//
	// Initializer shaders
	//
	if (initializeShaderPrograms)
	{
		init_shaders();

		sMatrix44UniformBuffer = new OpenGL::BufferObject(GL_UNIFORM_BUFFER, sizeof(mat4f) * 1 * GT_SKELETON_MAX_JOINTS, nullptr, GL_STREAM_DRAW);
		sMatrix33UniformBuffer = new OpenGL::BufferObject(GL_UNIFORM_BUFFER, sizeof(vec4f) * 3 * GT_SKELETON_MAX_JOINTS, nullptr, GL_STREAM_DRAW);

		// MaterialShaderProgram::get().::gintonic::Uniform::Block::Joint44::bindBuffer(*sMatrix44UniformBuffer);
		// MaterialShaderProgram::get().::gintonic::Uniform::Block::Joint33::bindBuffer(*sMatrix33UniformBuffer);
	}	

	//
	// Initialize basic shapes
	//
	if (initializeBasicMeshes) initializeBasicShapes();

	//
	// Initialize debug variables
	//
	// Determine the point size. We take 1/80 of the width
	// const unsigned int lPointSize = static_cast<unsigned int>((float)sWidth / 80.0f);
	const unsigned int lPointSize = 24;

	// This may throw a Font::InitException when the font is not present.
	sDebugFont = Font::create(FONT_FILE_LOCATION, lPointSize);
	sDebugErrorStream = (FontStream*) _mm_malloc(sizeof(FontStream), 16); // we want this on a 16-byte boundary.
	sDebugLogStream   = (FontStream*) _mm_malloc(sizeof(FontStream), 16); // we want this on a 16-byte boundary.
	new (sDebugErrorStream) FontStream(); // placement new
	new (sDebugLogStream)   FontStream(); // placement new
	sDebugErrorStream->open(sDebugFont);
	sDebugLogStream->open(sDebugFont);
	// #endif

	//
	// Check various values.
	//
	GLint lMaxVertexAttribs;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &lMaxVertexAttribs);
	if (lMaxVertexAttribs < 16)
	{
		throw std::runtime_error("Context does not support at least 16 vertex attributes.");
	}
	GLint lLineWidthDimensions[2];
	glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE, lLineWidthDimensions);
	lLineWidthDimensions[1] = std::min(PREFERRED_LINE_WIDTH, lLineWidthDimensions[1]);
	glLineWidth(static_cast<GLfloat>(lLineWidthDimensions[1]));
}

void Renderer::initDummy(const bool construct_shaders)
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
			throw NoContextAvailableException(3, 3);
		}
	}
	if (gladLoadGL() != 1)
	{
		release();
		throw FunctionLoadException();
	}

	if (construct_shaders) init_shaders();
}

void Renderer::resize(const int width, const int height)
{
	//
	// resize viewport
	//
	sWidth = width;
	sHeight = height;
	sAspectRatio = (float)sWidth / (float)sHeight;
	sMatrixPDirty = true;
	glViewport(0, 0, sWidth, sHeight);

	//
	// resize framebuffers
	//
	try
	{
		sGeometryBuffer->resize(sWidth, sHeight);	
	}
	catch (const OpenGL::Framebuffer::Exception& framebufferException)
	{
		exception lException(name());
		lException.append(": GeometryBuffer failed to resize: ");
		lException.append(framebufferException.what());
		throw ;
	}
	
	//
	// Update default camera
	//
	sDefaultCamera->setWidth(static_cast<float>(sWidth));
	sDefaultCamera->setHeight(static_cast<float>(sHeight));
}

bool Renderer::isInitialized() noexcept
{
	return sWindow != nullptr;
}

void Renderer::setCameraEntity(std::shared_ptr<Entity> cameraEntity)
{
	sCameraEntity = std::move(cameraEntity);
	if (!sCameraEntity->camera)
	{
		sCameraEntity->camera = sDefaultCamera;
	}
}

void Renderer::focusContext() noexcept
{ 
	SDL_GL_MakeCurrent(sWindow, sContext); 
}

void Renderer::setCursorPosition(const double x, const double y) noexcept
{
	SDL_WarpMouseInWindow(sWindow, (int)x, (int)y);
	SDL_FlushEvent(SDL_MOUSEMOTION);
}

void Renderer::setFreeformCursor(const bool b)
{
	SDL_SetRelativeMouseMode(b? SDL_TRUE : SDL_FALSE);
}

void Renderer::disableCursor() noexcept
{ 
	SDL_ShowCursor(0); 
}

void Renderer::enableCursor() noexcept
{ 
	SDL_ShowCursor(1); 
}

void Renderer::centerCursor() noexcept
{
	setCursorPosition(sWidth / 2, sHeight / 2);
}

void Renderer::vsync(const bool b) 
{ 
	SDL_GL_SetSwapInterval(b? 1 : 0);
}

void Renderer::beginTextInput() noexcept
{
	SDL_StartTextInput();
}

void Renderer::endTextInput() noexcept
{
	SDL_StopTextInput();
}

void Renderer::beginStencilPass() noexcept
{
	sGeometryBuffer->beginStencilPass();
}

void Renderer::endStencilPass() noexcept
{
	sGeometryBuffer->endStencilPass();
}

void Renderer::setWireframeMode(const bool yesOrNo) noexcept
{
	sRenderInWireframeMode = yesOrNo;
}

void Renderer::setGUIRoot(GUI::Base* root) noexcept
{
	sGUIRoot = root;
}

GUI::Base* Renderer::getGUIRoot() noexcept
{
	return sGUIRoot;
}

void Renderer::setViewGeometryBuffers(const bool yesOrNo) noexcept
{
	sViewGeometryBuffers = yesOrNo;
	if (sViewGeometryBuffers) sViewCameraDepthBuffer = false;
}

void Renderer::setViewCameraDepthBuffer(const bool yesOrNo) noexcept
{
	sViewCameraDepthBuffer = yesOrNo;
	if (sViewCameraDepthBuffer) sViewGeometryBuffers = false;
}

void Renderer::show() noexcept 
{ 
	SDL_ShowWindow(sWindow); 
}

void Renderer::hide() noexcept 
{ 
	SDL_HideWindow(sWindow); 
}

void Renderer::close() noexcept
{
	sShouldClose = true;
	onAboutToClose();
}

bool Renderer::shouldClose() noexcept
{ 
	return sShouldClose; 
}

vec2f Renderer::viewportSize() noexcept
{
	return vec2f(static_cast<float>(sWidth), static_cast<float>(sHeight));
}

bool Renderer::key(const int keycode) noexcept
{
	return sKeyState[keycode] != 0;
}

bool Renderer::keyPrev(const int keycode) noexcept
{
	return sKeyPrevState[keycode] != 0;
}

bool Renderer::keyTogglePress(const int keycode) noexcept
{
	return key(keycode) && !keyPrev(keycode);
}

bool Renderer::keyToggleRelease(const int keycode) noexcept
{
	return !key(keycode) && keyPrev(keycode);
}

bool Renderer::mouseButton(const int buttoncode) noexcept
{
	return 0 != (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(buttoncode));
}

void Renderer::release()
{
	if (sMatrix33UniformBuffer)
	{
		delete sMatrix33UniformBuffer;
		sMatrix33UniformBuffer = nullptr;
	}
	if (sMatrix44UniformBuffer)
	{
		delete sMatrix44UniformBuffer;
		sMatrix44UniformBuffer = nullptr;
	}
	if (sPackedQuadVBO)
	{
		glDeleteBuffers(1, &sPackedQuadVBO);
	}
	if (sPackedQuadVAO)
	{
		glDeleteVertexArrays(1, &sPackedQuadVAO);
	}
	if (sGeometryBuffer)
	{
		delete sGeometryBuffer;
		sGeometryBuffer = nullptr;
	}
	if (sDebugErrorStream)
	{
		try
		{
			// Explicitly call destructor.
			sDebugErrorStream->~FontStream();	
		}
		catch (...)
		{
			// Absorb all exception.
		}
		_mm_free(sDebugErrorStream);
		sDebugErrorStream = nullptr;
	}
	if (sDebugLogStream)
	{
		try
		{
			// Explicitly call destructor.
			sDebugLogStream->~FontStream();	
		}
		catch (...)
		{
			// Absorb all exception.
		}
		_mm_free(sDebugLogStream);
		sDebugErrorStream = nullptr;
	}
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

void Renderer::update() noexcept
{
	processEvents();
	prepareRendering();
	sGeometryBuffer->prepareGeometryPhase();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glCullFace(GL_BACK);

	if (sViewGeometryBuffers) // <--- debug path
	{
		renderGeometry();
		sGeometryBuffer->blitDrawbuffersToScreen(sWidth, sHeight);
		cerr() << "GEOMETRY BUFFERS\n";
	}
	else if (sViewCameraDepthBuffer) // <--- debug path
	{
		renderGeometry();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_DEPTH_TEST);
		sGeometryBuffer->bindDepthTexture(DEPTH_TEXTURE_UNIT);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glViewport(0, 0, sWidth, sHeight);
		const auto& lProgram = DepthBufferShaderProgram::get();
		lProgram.activate();
		lProgram.setViewportSize(viewportSize());
		lProgram.setDepthTexture(DEPTH_TEXTURE_UNIT);
		lProgram.setFarPlane(sCameraEntity->camera->farPlane() / 10.0f);
		sUnitQuadPUN->draw();
		cerr() << "CAMERA DEPTH BUFFER\n";
	}
	else if (sDebugShadowBufferEntity) // <--- debug path
	{
		assert(sDebugShadowBufferEntity->shadowBuffer);

		renderShadows();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_DEPTH_TEST);
		sDebugShadowBufferEntity->shadowBuffer->bindDepthTextures();
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glViewport(0, 0, sWidth, sHeight);
		const auto& lProgram = DepthBufferShaderProgram::get();
		lProgram.activate();
		lProgram.setViewportSize(viewportSize());
		lProgram.setDepthTexture(DEPTH_TEXTURE_UNIT);
		lProgram.setFarPlane(25.0f);
		sUnitQuadPUN->draw();
		cerr() << sDebugShadowBufferEntity->name << " SHADOW BUFFER\n";
	}
	else // <--- This is the "default" path.
	{
		renderGeometry();
		
		renderShadows();

		sGeometryBuffer->prepareLightingPhase();
		glViewport(0, 0, sWidth, sHeight);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);
		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_CLAMP);
		glEnable(GL_STENCIL_TEST);
		glEnable(GL_DEPTH_CLAMP);
		glDepthMask(GL_FALSE);
		renderPointLights();
		glDepthMask(GL_TRUE);
		glDisable(GL_DEPTH_CLAMP);
		glDisable(GL_STENCIL_TEST);
		glDisable(GL_DEPTH_CLAMP);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		renderLights();
		
		sGeometryBuffer->finalize(sWidth, sHeight);
	}

	if (sOctreeRoot)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glLineWidth(1.0f);
		const auto& lProgram = OctreeDebugShaderProgram::get();
		lProgram.activate();
		lProgram.setColor(vec3f(0.0f, 0.0f, 1.0f));
		sOctreeRoot->forEachNode( [&lProgram] (const Octree* node)
		{
			SQT lTransform;
			const auto lBBox = node->bounds();
			lTransform.rotation = quatf(1.0f, 0.0f, 0.0f, 0.0f);
			lTransform.translation = 0.5f * (lBBox.minCorner + lBBox.maxCorner); // Center of the box
			lTransform.scale = 0.5f * (lBBox.minCorner - lBBox.maxCorner);
			setModelMatrix(lTransform);
			lProgram.setMatrixPVM(matrix_PVM());
			sUnitCubePUN->draw();
		});
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
	glDisable(GL_CULL_FACE); // for the text
	renderGUI();

	finalizeRendering();
}

void Renderer::prepareRendering() noexcept
{
	sEntitiesLock.obtain();

	if (sRenderInWireframeMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void Renderer::renderGeometry() noexcept
{
	const auto& lMaterialShaderProgram = MaterialShaderProgram::get();
	lMaterialShaderProgram.activate();
	lMaterialShaderProgram.setMaterialDiffuseTexture(GBUFFER_TEX_DIFFUSE);
	lMaterialShaderProgram.setMaterialSpecularTexture(GBUFFER_TEX_SPECULAR);
	lMaterialShaderProgram.setMaterialNormalTexture(GBUFFER_TEX_NORMAL);

	std::vector<mat4f, allocator<mat4f>> matrixBs(GT_SKELETON_MAX_JOINTS);
	std::vector<mat3f> matrixBNs(GT_SKELETON_MAX_JOINTS);

	renderGeometry(sShadowCastingGeometryEntities, matrixBs, matrixBNs);
	renderGeometry(sNonShadowCastingGeometryEntities, matrixBs, matrixBNs);
}

void Renderer::renderGeometry(
	const std::vector<std::shared_ptr<Entity>>& geometries, 
	std::vector<mat4f, allocator<mat4f>>& matrixBs, 
	std::vector<mat3f>& matrixBNs) noexcept
{
	const auto& lMaterialShaderProgram = MaterialShaderProgram::get();
	const auto lElapsedTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime()).count()) / float(1e3);

	for (const auto& lEntity : geometries)
	{
		GLint lMaterialFlag       = 0;
		// GLint lHasTangentsAndBitangents = 0;
		const auto lMaterial      = lEntity->material.get();
		const auto lMesh          = lEntity->mesh.get();
		const auto lAnimationClip = lEntity->activeAnimationClip;

		if (lMaterial->diffuseTexture)
		{
			lMaterialFlag |= HAS_DIFFUSE_TEXTURE;
			lMaterial->diffuseTexture->bind(GBUFFER_TEX_DIFFUSE);
		}
		if (lMaterial->specularTexture)
		{
			lMaterialFlag |= HAS_SPECULAR_TEXTURE;
			lMaterial->specularTexture->bind(GBUFFER_TEX_SPECULAR);
		}
		if (lMaterial->normalTexture)
		{
			lMaterialFlag |= HAS_NORMAL_TEXTURE;
			lMaterial->normalTexture->bind(GBUFFER_TEX_NORMAL);
		}
		if (lMesh->hasTangentsAndBitangents())
		{
			lMaterialFlag |= HAS_TANGENTS_AND_BITANGENTS;
			// lHasTangentsAndBitangents = 1;
		}
		if (lAnimationClip && lMesh->hasSkinning())
		{
			lMaterialFlag |= MESH_HAS_JOINTS;

			lAnimationClip->isLooping = false;

			cerr() << lEntity->name << " --> " << lAnimationClip->name << '\n';
			const auto lStart = lEntity->activeAnimationStartTime;
			for (uint8_t j = 0; j < lAnimationClip->jointCount(); ++j)
			{
				matrixBs[j] = lAnimationClip->evaluate(j, lStart, lElapsedTime);
				matrixBNs[j] = matrixBs[j].upperLeft33().invert().transpose();

				// sMatrix44Array[j] = matrixBs[j];
				// sMatrix33Array[j] = matrixBNs[j];
				// sMatrix44Array[j] = lAnimationClip->evaluate(j, lStart, lElapsedTime);
				// const auto lTempNormalMatrix = sMatrix44Array[j].upperLeft33().invert().transpose();
				// sMatrix33Array[3 * j + 0] = vec4f(lTempNormalMatrix.data[0], lTempNormalMatrix.data[1], lTempNormalMatrix.data[2], 0.0f);
				// sMatrix33Array[3 * j + 1] = vec4f(lTempNormalMatrix.data[3], lTempNormalMatrix.data[4], lTempNormalMatrix.data[5], 0.0f);
				// sMatrix33Array[3 * j + 2] = vec4f(lTempNormalMatrix.data[6], lTempNormalMatrix.data[7], lTempNormalMatrix.data[8], 0.0f);
				// sMatrix33Array[j] = sMatrix44Array[j].upperLeft33().invert().transpose();
			}
			
			lMaterialShaderProgram.setMatrixB(matrixBs);
			lMaterialShaderProgram.setMatrixBN(matrixBNs);

			// glBindBuffer(GL_UNIFORM_BUFFER, *sMatrix44UniformBuffer);
			// gtBufferSubData(GL_UNIFORM_BUFFER, 0, sMatrix44Array.size(), sMatrix44Array);
			// glBindBufferBase(GL_UNIFORM_BUFFER, lMaterialShaderProgram.getJoint44(), *sMatrix44UniformBuffer);
			// glBindBuffer(GL_UNIFORM_BUFFER, *sMatrix33UniformBuffer);
			// gtBufferSubData(GL_UNIFORM_BUFFER, 0, sMatrix33Array.size(), sMatrix33Array);
			// glBindBufferBase(GL_UNIFORM_BUFFER, lMaterialShaderProgram.getJoint33(), *sMatrix33UniformBuffer);
		}

		setModelMatrix(lEntity->globalTransform());

		// const auto lJointBlockIndex       = glGetUniformBlockIndex(lMaterialShaderProgram, "JointBlock44");
		// const auto lNormalJointBlockIndex = glGetUniformBlockIndex(lMaterialShaderProgram, "JointBlock33");

		// if (lJointBlockIndex == GL_INVALID_INDEX)
		// {
		// 	cerr() << "GL_INVALID_INDEX for joint block.\n";
		// }
		// if (lNormalJointBlockIndex == GL_INVALID_INDEX)
		// {
		// 	cerr() << "GL_INVALID_INDEX for normal joint block.\n";
		// }

		lMaterialShaderProgram.setMaterialDiffuseColor(lMaterial->diffuseColor);
		lMaterialShaderProgram.setMaterialSpecularColor(lMaterial->specularColor);
		lMaterialShaderProgram.setMaterialFlag(lMaterialFlag);
		// lMaterialShaderProgram.setHasTangentsAndBitangents(lHasTangentsAndBitangents);
		lMaterialShaderProgram.setMatrixPVM(matrix_PVM());
		lMaterialShaderProgram.setMatrixVM(matrix_VM());
		lMaterialShaderProgram.setMatrixN(matrix_N());

		lMesh->draw();
	}
}

void Renderer::renderShadows() noexcept
{
	// ShadowShaderProgram::get().activate();
	// ShadowShaderProgram::get().setInstancedRendering(0);
	for (auto lEntity : sShadowCastingLightEntities)
	{
		lEntity->shadowBuffer->collect(*lEntity, sShadowCastingGeometryEntities);
	}
}

void Renderer::renderPointLights() noexcept
{
	for (const auto lEntity : sShadowCastingPointLightEntities)
	{
		lEntity->light->shine(*lEntity, sShadowCastingGeometryEntities);
	}
}

void Renderer::renderLights() noexcept
{
	// Ambient lighting
	const auto& lAmbientLightShaderProgram = AmbientLightShaderProgram::get();
	lAmbientLightShaderProgram.activate();
	lAmbientLightShaderProgram.setGeometryBufferDiffuseTexture(GBUFFER_DIFFUSE);
	lAmbientLightShaderProgram.setViewportSize(viewportSize());
	lAmbientLightShaderProgram.setLightIntensity(vec4f(1.0f, 1.0f, 1.0f, 1.0f));
	sUnitQuadPUN->draw();
	
	for (auto lEntity : sShadowCastingLightEntities)
	{
		lEntity->light->shine(*lEntity, sShadowCastingGeometryEntities);
	}
	for (auto lEntity : sNonShadowCastingLightEntities)
	{
		lEntity->light->shine(*lEntity, sShadowCastingGeometryEntities);
	}
}

void Renderer::renderGUI() noexcept
{
	if (sGUIRoot)
	{
		const auto& lProgram = GUIShaderProgram::get();
		lProgram.activate();
		sGUIRoot->draw();
	}
}

void Renderer::finalizeRendering() noexcept
{
	sShadowCastingLightEntities.clear();
	sShadowCastingPointLightEntities.clear();
	sShadowCastingGeometryEntities.clear();
	sNonShadowCastingLightEntities.clear();
	sNonShadowCastingGeometryEntities.clear();
	sEntitiesLock.release();

	const auto& lTextProgram = FlatTextShaderProgram::get();
	lTextProgram.activate();
	lTextProgram.setColor(vec3f(1.0f, 0.0f, 0.0f));
	sDebugErrorStream->close();
	lTextProgram.setColor(vec3f(0.8f, 0.8f, 0.8f));
	sDebugLogStream->close();

	SDL_GL_SwapWindow(sWindow);

	sDebugErrorStream->open(sDebugFont);
	sDebugLogStream->open(sDebugFont);
}

void Renderer::processEvents() noexcept
{
	sPrevElapsedTime = sElapsedTime;
	sElapsedTime = clock_type::now() - sStartTime;
	sDeltaTime = sElapsedTime - sPrevElapsedTime;

	std::memcpy(sKeyPrevState, sKeyState, sizeof(Uint8) * sKeyStateCount);
	sKeyState = SDL_GetKeyboardState(nullptr); // Maybe unnecessary

	sMouseDelta = 0.0f;
	sMouseWheel = 0.0f;
	sFingerMotion = 0.0f;
	while (SDL_PollEvent(&sEvent))
	{
		switch (sEvent.type)
		{
		case SDL_WINDOWEVENT:
			switch (sEvent.window.event)
			{
				case SDL_WINDOWEVENT_ENTER:
					onMouseEnter();
					break;
				case SDL_WINDOWEVENT_LEAVE:
					onMouseLeave();
					break;
				case SDL_WINDOWEVENT_RESIZED:
					resize((int)sEvent.window.data1, (int)sEvent.window.data2);
					onWindowResize(sWidth, sHeight);
					break;
			}
			break;
		case SDL_MOUSEMOTION:
		{
			int lMouseXPosition;
			int lMouseYPosition;
			vec2f lMousePosition;
			SDL_GetMouseState(&lMouseXPosition, &lMouseYPosition);
			// Make the position relative
			lMousePosition.x = 2.0f * ((float)lMouseXPosition / sWidth) - 1.0f;
			lMousePosition.y = 2.0f * (1.0f - ((float)lMouseYPosition / sHeight)) - 1.0f;
			onMouseMove(lMousePosition);
			sMouseDelta.x += (float)sEvent.motion.xrel;
			sMouseDelta.y += (float)sEvent.motion.yrel;
			break;
		}
		case SDL_MOUSEWHEEL:
			sMouseWheel.x += (float)sEvent.wheel.x;
			sMouseWheel.y += (float)sEvent.wheel.y;
			break;
		case SDL_FINGERMOTION:
			sFingerMotion.x += sEvent.tfinger.x;
			sFingerMotion.y += sEvent.tfinger.y;
			sFingerMotion.z += sEvent.tfinger.dx;
			sFingerMotion.w += sEvent.tfinger.dy;
			break;
		case SDL_KEYDOWN:
			onKeyPress(sEvent.key.keysym.scancode, sEvent.key.keysym.sym, sEvent.key.keysym.mod);
			break;
		case SDL_KEYUP:
			onKeyRelease(sEvent.key.keysym.scancode, sEvent.key.keysym.sym, sEvent.key.keysym.mod);
			break;
		case SDL_TEXTINPUT:
			/* Add new text onto the end of our text */
			// strcat(text, sEvent.text.text);
			onTextInput(sEvent.text.text);
			break;
		case SDL_TEXTEDITING:
			/*
			Update the composition text.
			Update the cursor position.
			Update the selection length (if any).
			*/
			// composition = sEvent.edit.text;
			// cursor = sEvent.edit.start;
			// selection_len = sEvent.edit.length;
			onTextEdit(sEvent.edit.text, sEvent.edit.start, sEvent.edit.length);
			break;
		case SDL_QUIT:
			close();
			break;
		default:
			break;
		}
	}

	if (sFingerMotion.x != 0.0f || sFingerMotion.y != 0.0f || sFingerMotion.z != 0.0f || sFingerMotion.w != 0.0f)
	{
		onFingerMotion(sFingerMotion);
	}

	// Update the WORLD->VIEW matrix.
	sCameraEntity->updateViewMatrix(sMatrixV);
	sCameraPosition = vec3f((sCameraEntity->globalTransform() * vec4f(0.0f, 0.0f, 0.0f, 1.0f)).data);
}

void Renderer::updateMatrixP()
{
	if (sMatrixPDirty)
	{
		sMatrixP = sCameraEntity->camera->projectionMatrix();
		sMatrixPDirty = false;
	}
}

void Renderer::updateMatrixVM()
{
	if (sMatrixVMDirty)
	{
		sMatrixVM = sMatrixV * sMatrixM;
		// sMatrix44Array[0] = sMatrixVM;
		sMatrixVMDirty = false;
	}
}

void Renderer::updateMatrixPVM()
{
	updateMatrixP();
	updateMatrixVM();
	if (sMatrixPVMDirty)
	{
		sMatrixPVM = sCameraEntity->camera->projectionMatrix()  * sMatrixVM;
		// sMatrix44Array[1] = sMatrixPVM;
		sMatrixPVMDirty = false;
	}
}

void Renderer::updateMatrixN()
{
	updateMatrixVM();
	if (sMatrixNDirty)
	{
		sMatrixN = sMatrixVM.upperLeft33().invert().transpose();
		// sMatrix33Array[0] = sMatrixN;
		sMatrixNDirty = false;
	}
}

#define RENDERER_NOT_INITIALIZED "Renderer is not initialized."

const char* Renderer::name()
{
	if (!isInitialized()) throw std::logic_error(RENDERER_NOT_INITIALIZED);
	else return reinterpret_cast<const char*>(glGetString(GL_RENDERER));
}

const char* Renderer::version()
{
	if (!isInitialized()) throw std::logic_error(RENDERER_NOT_INITIALIZED);
	else return reinterpret_cast<const char*>(glGetString(GL_VERSION));
}

void Renderer::submitEntityRecursive(std::shared_ptr<Entity> current)
{
	EntitySorter lEntitySorter(current);
	lEntitySorter.execute();

	// sEntityQueueLock.obtain();
	// submitEntityRecursiveHelper(current);
	// sEntityQueueLock.release();
}

std::shared_ptr<Entity> Renderer::createGizmo()
{
	static Entity::WeakPtr sGizmoCache;

	if (auto lCachedGizmo = sGizmoCache.lock())
	{
		return lCachedGizmo;
	}

	auto lRedMaterial = Material::create();
	lRedMaterial->name = "Red";
	lRedMaterial->diffuseColor = vec4f(1.0f, 0.0f, 0.0f, 0.0f);
	lRedMaterial->specularColor = vec4f(0.0f, 0.0f, 0.0f, 0.0f);

	auto lGreenMaterial = Material::create();
	lGreenMaterial->name = "Green";
	lGreenMaterial->diffuseColor = vec4f(0.0f, 1.0f, 0.0f, 0.0f);
	lGreenMaterial->specularColor = vec4f(0.0f, 0.0f, 0.0f, 0.0f);

	auto lBlueMaterial = Material::create();
	lBlueMaterial->name = "Blue";
	lBlueMaterial->diffuseColor = vec4f(0.0f, 0.0f, 1.0f, 0.0f);
	lBlueMaterial->specularColor = vec4f(0.0f, 0.0f, 0.0f, 0.0f);

	auto lRightArrowCone = Entity::create
	(
		"RightArrowCone",
		SQT
		(
			vec3f(5.0f, 5.0f, 0.2f),
			quatf(1.0f, 0.0f, 0.0f, 0.0f),
			vec3f(0.0f, 0.0f, -1.0f)
		)
	);
	lRightArrowCone->material = lRedMaterial;
	lRightArrowCone->mesh = getUnitCone();

	auto lRightArrowCylinder = Entity::create
	(
		"RightArrowCylinder",
		SQT
		(
			vec3f(1.0f, 0.02f, 0.02f), 
			quatf::axis_angle(vec3f(0.0f, 1.0f, 0.0f), -static_cast<float>(M_PI) * 0.5f),
			vec3f(1.0f, 0.0f, 0.0f)
		)
	);
	lRightArrowCylinder->material = lRedMaterial;
	lRightArrowCylinder->mesh = getUnitCylinder();
	lRightArrowCylinder->addChild(lRightArrowCone);

	auto lUpArrowCone = Entity::create
	(
		"UpArrowCone",
		SQT
		(
			vec3f(5.0f, 5.0f, 0.2f),
			quatf(1.0f, 0.0f, 0.0f, 0.0f),
			vec3f(0.0f, 0.0f, -1.0f)
		)
	);
	lUpArrowCone->material = lGreenMaterial;
	lUpArrowCone->mesh = getUnitCone();

	auto lUpArrowCylinder = Entity::create
	(
		"UpArrowCylinder",
		SQT
		(
			vec3f(0.02f, 1.0f, 0.02f), 
			quatf::axis_angle(vec3f(1.0f, 0.0f, 0.0f), static_cast<float>(M_PI) * 0.5f),
			vec3f(0.0f, 1.0f, 0.0f)
		)
	);
	lUpArrowCylinder->material = lGreenMaterial;
	lUpArrowCylinder->mesh = getUnitCylinder();
	lUpArrowCylinder->addChild(lUpArrowCone);

	auto lForwardArrowCone = Entity::create
	(
		"ForwardArrowCone",
		SQT
		(
			vec3f(5.0f, 5.0f, 0.2f),
			quatf(1.0f, 0.0f, 0.0f, 0.0f),
			vec3f(0.0f, 0.0f, -1.0f)
		)
	);
	lForwardArrowCone->material = lBlueMaterial;
	lForwardArrowCone->mesh = getUnitCone();

	auto lBackwardArrowCylinder = Entity::create
	(
		"ForwardArrowCylinder",
		SQT
		(
			vec3f(0.02f, 0.02f, 1.0f), 
			quatf::axis_angle(vec3f(1.0f, 0.0f, 0.0f), static_cast<float>(M_PI)),
			vec3f(0.0f, 0.0f, 1.0f)
		)
	);
	lBackwardArrowCylinder->material = lBlueMaterial;
	lBackwardArrowCylinder->mesh = getUnitCylinder();
	lBackwardArrowCylinder->addChild(lForwardArrowCone);

	auto lGizmoRoot = Entity::create
	(
		"Gizmo",
		SQT
		(
			vec3f(1.0f, 1.0f, 1.0f), 
			quatf(1.0f, 0.0f, 0.0f, 0.0f), 
			vec3f(0.0f, 0.0f, 0.0f)
		)
	);
	lGizmoRoot->addChild(lRightArrowCylinder);
	lGizmoRoot->addChild(lUpArrowCylinder);
	lGizmoRoot->addChild(lBackwardArrowCylinder);

	sGizmoCache = lGizmoRoot;

	return lGizmoRoot;
}

void Renderer::init_shaders()
{
	#define STRINGIFY(x) #x
	#define INIT_SHADER(className)                    \
	try                                               \
	{                                                 \
		className::initialize();                      \
	}                                                 \
	catch(exception& e)                               \
	{                                                 \
		e.prepend(": ");                              \
		e.prepend(STRINGIFY(className));              \
		e.prepend(": ");                              \
		e.prepend(name());                            \
		throw;                                        \
	}

	INIT_SHADER(DepthBufferShaderProgram);
	INIT_SHADER(ShadowShaderProgram);
	INIT_SHADER(MaterialShaderProgram);
	INIT_SHADER(AmbientLightShaderProgram);
	INIT_SHADER(DirectionalLightShaderProgram);
	INIT_SHADER(PointLightShaderProgram);
	INIT_SHADER(SpotLightShaderProgram);
	INIT_SHADER(FlatTextShaderProgram);
	INIT_SHADER(GUIShaderProgram);
	INIT_SHADER(SilhouetteShaderProgram);
	INIT_SHADER(ShadowVolumeShaderProgram);
	INIT_SHADER(OctreeDebugShaderProgram);
}


struct SphereFace
{
	virtual ~SphereFace() = default;
	virtual void get(
		const float s, 
		const float t, 
		gintonic::vec3f& pos, 
		gintonic::vec2f& uv) const noexcept = 0;
};

struct SphereFrontFace : public SphereFace
{
	virtual ~SphereFrontFace() = default;
	inline virtual void get(
		const float s, 
		const float t, 
		gintonic::vec3f& pos, 
		gintonic::vec2f& uv) const noexcept final
	{
		pos.x = 2.0f * s - 1.0f;
		pos.y = 2.0f * t - 1.0f;
		pos.z = 1.0f;
		uv.x = s;
		uv.y = t;
		pos.normalize();
	}
};

struct SphereBackFace : public SphereFace
{
	virtual ~SphereBackFace() = default;
	inline virtual void get(
		const float s, 
		const float t, 
		gintonic::vec3f& pos, 
		gintonic::vec2f& uv) const noexcept final
	{
		pos.x = 2.0f * s - 1.0f;
		pos.y = -(2.0f * t - 1.0f);
		pos.z = -1.0f;
		uv.x = s;
		uv.y = t;
		pos.normalize();
	}
};

struct SphereTopFace : public SphereFace
{
	virtual ~SphereTopFace() = default;
	inline virtual void get(
		const float s, 
		const float t, 
		gintonic::vec3f& pos, 
		gintonic::vec2f& uv) const noexcept final
	{
		pos.x = 2.0f * s - 1.0f;
		pos.y = 1.0f;
		pos.z = -(2.0f * t - 1.0f);
		uv.x = s;
		uv.y = t;
		pos.normalize();
	}
};

struct SphereBottomFace : public SphereFace
{
	virtual ~SphereBottomFace() = default;
	inline virtual void get(
		const float s, 
		const float t, 
		gintonic::vec3f& pos, 
		gintonic::vec2f& uv) const noexcept final
	{
		pos.x = 2.0f * s - 1.0f;
		pos.y = -1.0f;
		pos.z = 2.0f * t - 1.0f;
		uv.x = s;
		uv.y = t;
		pos.normalize();
	}
};

struct SphereLeftFace : public SphereFace
{
	virtual ~SphereLeftFace() = default;
	inline virtual void get(
		const float s, 
		const float t, 
		gintonic::vec3f& pos, 
		gintonic::vec2f& uv) const noexcept final
	{
		pos.x = -1.0f;
		pos.y = 2.0f * s - 1.0f;
		pos.z = -(2.0f * t - 1.0f);
		uv.x = s;
		uv.y = t;
		pos.normalize();
	}
};

struct SphereRightFace : public SphereFace
{
	virtual ~SphereRightFace() = default;
	inline virtual void get(
		const float s, 
		const float t, 
		gintonic::vec3f& pos, 
		gintonic::vec2f& uv) const noexcept final
	{
		pos.x = 1.0f;
		pos.y = 2.0f * s - 1.0f;
		pos.z = 2.0f * t - 1.0f;
		uv.x = s;
		uv.y = t;
		pos.normalize();
	}
};

void Renderer::drawPackedUnitQuad() noexcept
{
	glBindVertexArray(sPackedQuadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, sPackedQuadVBO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Renderer::initializeBasicShapes()
{
	/* Set up the packed unit quad */

	glGenVertexArrays(1, &sPackedQuadVAO);
	glGenBuffers(1, &sPackedQuadVBO);
	glBindVertexArray(sPackedQuadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, sPackedQuadVBO);
	std::array<float, 16> lPackedQuadArray{{-1.0f,-1.0f,0.0f,1.0f,  1.0f,-1.0f,1.0f,1.0f,  1.0f,1.0f,1.0f,0.0f, -1.0f,1.0f,0.0f,0.0f}};
	glBufferData
	(
		GL_ARRAY_BUFFER, // target
		4 * 4 * sizeof(float), // size
		(const GLvoid*)lPackedQuadArray.data(), // data
		GL_STATIC_DRAW // usage
	);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
	

	/* Set up the unit quad */

	sUnitQuadPUN = Mesh::create();
	sUnitQuadPUN->name = "UnitQuad";
	sUnitQuadPUN->set(
		{
			0,1,2,2,1,3 // indices
		},
		{
			{-1.0f, -1.0f, 0.0f,  0.0f}, // position_XYZ_texcoords_X
			{ 1.0f, -1.0f, 0.0f,  1.0f},
			{-1.0f,  1.0f, 0.0f,  0.0f},
			{ 1.0f,  1.0f, 0.0f,  1.0f}
		},
		{
			{ 0.0f,  0.0f, 1.0f,  1.0f}, // normal_XYZ_texcoords_Y
			{ 0.0f,  0.0f, 1.0f,  1.0f},
			{ 0.0f,  0.0f, 1.0f,  0.0f},
			{ 0.0f,  0.0f, 1.0f,  0.0f}
		}
	);

	/* Set up the unit cube */

	sUnitCubePUN = Mesh::create();
	sUnitCubePUN->name = "UnitCube";
	sUnitCubePUN->set(
		{
			// front
			 0,  1,  2, // indices
			 2,  3,  0,
			// top
			 4,  5,  6,
			 6,  7,  4,
			// back
			 8,  9, 10,
			10, 11,  8,
			// bottom
			12, 13, 14,
			14, 15, 12,
			// left
			16, 17, 18,
			18, 19, 16,
			// right
			20, 21, 22,
			22, 23, 20
		},
		{
			// front
			{-1.0f, -1.0f,  1.0f,  0.0f}, // position_XYZ_texcoords_X
			{ 1.0f, -1.0f,  1.0f,  1.0f},
			{ 1.0f,  1.0f,  1.0f,  1.0f},
			{-1.0f,  1.0f,  1.0f,  0.0f},
			// top
			{-1.0f,  1.0f,  1.0f,  0.0f},
			{ 1.0f,  1.0f,  1.0f,  1.0f},
			{ 1.0f,  1.0f, -1.0f,  1.0f},
			{-1.0f,  1.0f, -1.0f,  0.0f},
			// back
			{ 1.0f, -1.0f, -1.0f,  0.0f},
			{-1.0f, -1.0f, -1.0f,  1.0f},
			{-1.0f,  1.0f, -1.0f,  1.0f},
			{ 1.0f,  1.0f, -1.0f,  0.0f},
			// bottom
			{-1.0f, -1.0f, -1.0f,  0.0f},
			{ 1.0f, -1.0f, -1.0f,  1.0f},
			{ 1.0f, -1.0f,  1.0f,  1.0f},
			{-1.0f, -1.0f,  1.0f,  0.0f},
			// left
			{-1.0f, -1.0f, -1.0f,  0.0f},
			{-1.0f, -1.0f,  1.0f,  1.0f},
			{-1.0f,  1.0f,  1.0f,  1.0f},
			{-1.0f,  1.0f, -1.0f,  0.0f},
			// right
			{ 1.0f, -1.0f,  1.0f,  0.0f},
			{ 1.0f, -1.0f, -1.0f,  1.0f},
			{ 1.0f,  1.0f, -1.0f,  1.0f},
			{ 1.0f,  1.0f,  1.0f,  0.0f}
		},
		{
			// front
			{ 0.0f,  0.0f,  1.0f,  1.0f}, // normal_XYZ_texcoords_Y
			{ 0.0f,  0.0f,  1.0f,  1.0f},
			{ 0.0f,  0.0f,  1.0f,  0.0f},
			{ 0.0f,  0.0f,  1.0f,  0.0f},
			// top
			{ 0.0f,  1.0f,  0.0f,  1.0f},
			{ 0.0f,  1.0f,  0.0f,  1.0f},
			{ 0.0f,  1.0f,  0.0f,  0.0f},
			{ 0.0f,  1.0f,  0.0f,  0.0f},
			// back
			{ 0.0f,  0.0f, -1.0f,  1.0f},
			{ 0.0f,  0.0f, -1.0f,  1.0f},
			{ 0.0f,  0.0f, -1.0f,  0.0f},
			{ 0.0f,  0.0f, -1.0f,  0.0f},
			// bottom
			{ 0.0f, -1.0f,  0.0f,  1.0f},
			{ 0.0f, -1.0f,  0.0f,  1.0f},
			{ 0.0f, -1.0f,  0.0f,  0.0f},
			{ 0.0f, -1.0f,  0.0f,  0.0f},
			// left
			{-1.0f,  0.0f,  0.0f,  1.0f},
			{-1.0f,  0.0f,  0.0f,  1.0f},
			{-1.0f,  0.0f,  0.0f,  0.0f},
			{-1.0f,  0.0f,  0.0f,  0.0f},
			// right
			{ 1.0f,  0.0f,  0.0f,  1.0f},
			{ 1.0f,  0.0f,  0.0f,  1.0f},
			{ 1.0f,  0.0f,  0.0f,  0.0f},
			{ 1.0f,  0.0f,  0.0f,  0.0f}
		}
	);

	/* Set up the unit cube with tangents (and bitangents) */

	sUnitCubePUNTB = Mesh::create();
	sUnitCubePUNTB->name = "UnitCubeWithTangents";
	sUnitCubePUNTB->set(
		{
			// front
			 0,  1,  2, // indices
			 2,  3,  0,
			// top
			 4,  5,  6,
			 6,  7,  4,
			// back
			 8,  9, 10,
			10, 11,  8,
			// bottom
			12, 13, 14,
			14, 15, 12,
			// left
			16, 17, 18,
			18, 19, 16,
			// right
			20, 21, 22,
			22, 23, 20
		},
		{
			// front
			{-1.0f, -1.0f,  1.0f,  0.0f}, // position_XYZ_texcoords_X
			{ 1.0f, -1.0f,  1.0f,  1.0f},
			{ 1.0f,  1.0f,  1.0f,  1.0f},
			{-1.0f,  1.0f,  1.0f,  0.0f},
			// top
			{-1.0f,  1.0f,  1.0f,  0.0f},
			{ 1.0f,  1.0f,  1.0f,  1.0f},
			{ 1.0f,  1.0f, -1.0f,  1.0f},
			{-1.0f,  1.0f, -1.0f,  0.0f},
			// back
			{ 1.0f, -1.0f, -1.0f,  0.0f},
			{-1.0f, -1.0f, -1.0f,  1.0f},
			{-1.0f,  1.0f, -1.0f,  1.0f},
			{ 1.0f,  1.0f, -1.0f,  0.0f},
			// bottom
			{-1.0f, -1.0f, -1.0f,  0.0f},
			{ 1.0f, -1.0f, -1.0f,  1.0f},
			{ 1.0f, -1.0f,  1.0f,  1.0f},
			{-1.0f, -1.0f,  1.0f,  0.0f},
			// left
			{-1.0f, -1.0f, -1.0f,  0.0f},
			{-1.0f, -1.0f,  1.0f,  1.0f},
			{-1.0f,  1.0f,  1.0f,  1.0f},
			{-1.0f,  1.0f, -1.0f,  0.0f},
			// right
			{ 1.0f, -1.0f,  1.0f,  0.0f},
			{ 1.0f, -1.0f, -1.0f,  1.0f},
			{ 1.0f,  1.0f, -1.0f,  1.0f},
			{ 1.0f,  1.0f,  1.0f,  0.0f}
		},
		{
			// front
			{ 0.0f,  0.0f,  1.0f,  1.0f}, // normal_XYZ_texcoords_Y
			{ 0.0f,  0.0f,  1.0f,  1.0f},
			{ 0.0f,  0.0f,  1.0f,  0.0f},
			{ 0.0f,  0.0f,  1.0f,  0.0f},
			// top
			{ 0.0f,  1.0f,  0.0f,  1.0f},
			{ 0.0f,  1.0f,  0.0f,  1.0f},
			{ 0.0f,  1.0f,  0.0f,  0.0f},
			{ 0.0f,  1.0f,  0.0f,  0.0f},
			// back
			{ 0.0f,  0.0f, -1.0f,  1.0f},
			{ 0.0f,  0.0f, -1.0f,  1.0f},
			{ 0.0f,  0.0f, -1.0f,  0.0f},
			{ 0.0f,  0.0f, -1.0f,  0.0f},
			// bottom
			{ 0.0f, -1.0f,  0.0f,  1.0f},
			{ 0.0f, -1.0f,  0.0f,  1.0f},
			{ 0.0f, -1.0f,  0.0f,  0.0f},
			{ 0.0f, -1.0f,  0.0f,  0.0f},
			// left
			{-1.0f,  0.0f,  0.0f,  1.0f},
			{-1.0f,  0.0f,  0.0f,  1.0f},
			{-1.0f,  0.0f,  0.0f,  0.0f},
			{-1.0f,  0.0f,  0.0f,  0.0f},
			// right
			{ 1.0f,  0.0f,  0.0f,  1.0f},
			{ 1.0f,  0.0f,  0.0f,  1.0f},
			{ 1.0f,  0.0f,  0.0f,  0.0f},
			{ 1.0f,  0.0f,  0.0f,  0.0f}
		},
		{
			// front
			{1.0f, 0.0f, 0.0f, 1.0f}, // tangent_XYZ_handedness
			{1.0f, 0.0f, 0.0f, 1.0f},
			{1.0f, 0.0f, 0.0f, 1.0f},
			{1.0f, 0.0f, 0.0f, 1.0f},
			// top
			{1.0f, 0.0f, 0.0f,  1.0f},
			{1.0f, 0.0f, 0.0f,  1.0f},
			{1.0f, 0.0f, 0.0f,  1.0f},
			{1.0f, 0.0f, 0.0f,  1.0f},
			// back
			{-1.0f, 0.0f, 0.0f, 1.0f},
			{-1.0f, 0.0f, 0.0f, 1.0f},
			{-1.0f, 0.0f, 0.0f, 1.0f},
			{-1.0f, 0.0f, 0.0f, 1.0f},
			// bottom
			{-1.0f, 0.0f, 0.0f, 1.0f},
			{-1.0f, 0.0f, 0.0f, 1.0f},
			{-1.0f, 0.0f, 0.0f, 1.0f},
			{-1.0f, 0.0f, 0.0f, 1.0f},
			// left
			{0.0f, 0.0f, 1.0f, 1.0f},
			{0.0f, 0.0f, 1.0f, 1.0f},
			{0.0f, 0.0f, 1.0f, 1.0f},
			{0.0f, 0.0f, 1.0f, 1.0f},
			// right
			{0.0f, 0.0f, -1.0f, 1.0f},
			{0.0f, 0.0f, -1.0f, 1.0f},
			{0.0f, 0.0f, -1.0f, 1.0f},
			{0.0f, 0.0f, -1.0f, 1.0f}
		}
	);

	/* Set up the flipped unit cube */

	sFlippedUnitCubePUN = Mesh::create();
	sFlippedUnitCubePUN->name = "FlippedUnitCube";
	sFlippedUnitCubePUN->set(
		{
			// front
			2,  1,  0, // indices
			0,  3,  2,
			// top
			6,  5,  4,
			4,  7,  6,
			// back
			10,  9,  8,
			8, 11, 10,
			// bottom
			14, 13, 12,
			12, 15, 14,
			// left
			18, 17, 16,
			16, 19, 18,
			// right
			22, 21, 20,
			20, 23, 22
		},
		{
			// front
			{-1.0f, -1.0f,  1.0f,  0.0f}, // position_XYZ_texcoords_X
			{ 1.0f, -1.0f,  1.0f,  1.0f},
			{ 1.0f,  1.0f,  1.0f,  1.0f},
			{-1.0f,  1.0f,  1.0f,  0.0f},
			// top
			{-1.0f,  1.0f,  1.0f,  0.0f},
			{ 1.0f,  1.0f,  1.0f,  1.0f},
			{ 1.0f,  1.0f, -1.0f,  1.0f},
			{-1.0f,  1.0f, -1.0f,  0.0f},
			// back
			{ 1.0f, -1.0f, -1.0f,  0.0f},
			{-1.0f, -1.0f, -1.0f,  1.0f},
			{-1.0f,  1.0f, -1.0f,  1.0f},
			{ 1.0f,  1.0f, -1.0f,  0.0f},
			// bottom
			{-1.0f, -1.0f, -1.0f,  0.0f},
			{ 1.0f, -1.0f, -1.0f,  1.0f},
			{ 1.0f, -1.0f,  1.0f,  1.0f},
			{-1.0f, -1.0f,  1.0f,  0.0f},
			// left
			{-1.0f, -1.0f, -1.0f,  0.0f},
			{-1.0f, -1.0f,  1.0f,  1.0f},
			{-1.0f,  1.0f,  1.0f,  1.0f},
			{-1.0f,  1.0f, -1.0f,  0.0f},
			// right
			{ 1.0f, -1.0f,  1.0f,  0.0f},
			{ 1.0f, -1.0f, -1.0f,  1.0f},
			{ 1.0f,  1.0f, -1.0f,  1.0f},
			{ 1.0f,  1.0f,  1.0f,  0.0f}
		},
		{
			// front
			{ 0.0f,  0.0f, -1.0f,  1.0f}, // normal_XYZ_texcoords_Y
			{ 0.0f,  0.0f, -1.0f,  1.0f},
			{ 0.0f,  0.0f, -1.0f,  0.0f},
			{ 0.0f,  0.0f, -1.0f,  0.0f},
			// top
			{ 0.0f, -1.0f,  0.0f,  1.0f},
			{ 0.0f, -1.0f,  0.0f,  1.0f},
			{ 0.0f, -1.0f,  0.0f,  0.0f},
			{ 0.0f, -1.0f,  0.0f,  0.0f},
			// back
			{ 0.0f,  0.0f,  1.0f,  1.0f},
			{ 0.0f,  0.0f,  1.0f,  1.0f},
			{ 0.0f,  0.0f,  1.0f,  0.0f},
			{ 0.0f,  0.0f,  1.0f,  0.0f},
			// bottom
			{ 0.0f,  1.0f,  0.0f,  1.0f},
			{ 0.0f,  1.0f,  0.0f,  1.0f},
			{ 0.0f,  1.0f,  0.0f,  0.0f},
			{ 0.0f,  1.0f,  0.0f,  0.0f},
			// left
			{ 1.0f,  0.0f,  0.0f,  1.0f},
			{ 1.0f,  0.0f,  0.0f,  1.0f},
			{ 1.0f,  0.0f,  0.0f,  0.0f},
			{ 1.0f,  0.0f,  0.0f,  0.0f},
			// right
			{-1.0f,  0.0f,  0.0f,  1.0f},
			{-1.0f,  0.0f,  0.0f,  1.0f},
			{-1.0f,  0.0f,  0.0f,  0.0f},
			{-1.0f,  0.0f,  0.0f,  0.0f}
		}
	);

	/* Set up the unit sphere */

	SphereFace* lSphereFace[6] =
	{
		new SphereFrontFace(),
		new SphereBackFace(),
		new SphereTopFace(),
		new SphereBottomFace(),
		new SphereLeftFace(),
		new SphereRightFace()
	};

	GLuint lCounter(0);
	GLfloat s, t;
	GLsizei lSubdivisions = NUM_SUBDIVISIONS;
	GLfloat lSubdivs = static_cast<GLfloat>(lSubdivisions);
	gintonic::vec3f lPosition;
	gintonic::vec2f lTexCoord;
	std::vector<Mesh::vec4f> lPosition_XYZ_TexCoord_X;
	std::vector<Mesh::vec4f> lNormal_XYZ_TexCoord_Y;
	std::vector<GLuint> lIndices;

	for (std::size_t f = 0; f < 6; ++f)
	{
		for (GLsizei i = 0; i < lSubdivisions; ++i)
		{
			for (GLsizei j = 0; j < lSubdivisions; ++j)
			{
				s = static_cast<GLfloat>(i) / lSubdivs;
				t = static_cast<GLfloat>(j) / lSubdivs;
				lSphereFace[f]->get(s, t, lPosition, lTexCoord);
				lPosition_XYZ_TexCoord_X.emplace_back(lPosition.x, lPosition.y, lPosition.z, lTexCoord.x);
				lNormal_XYZ_TexCoord_Y.emplace_back(lPosition.x, lPosition.y, lPosition.z, lTexCoord.y);

				s = static_cast<GLfloat>(i+1) / lSubdivs;
				lSphereFace[f]->get(s, t, lPosition, lTexCoord);
				lPosition_XYZ_TexCoord_X.emplace_back(lPosition.x, lPosition.y, lPosition.z, lTexCoord.x);
				lNormal_XYZ_TexCoord_Y.emplace_back(lPosition.x, lPosition.y, lPosition.z, lTexCoord.y);

				t = static_cast<GLfloat>(j+1) / lSubdivs;
				lSphereFace[f]->get(s, t, lPosition, lTexCoord);
				lPosition_XYZ_TexCoord_X.emplace_back(lPosition.x, lPosition.y, lPosition.z, lTexCoord.x);
				lNormal_XYZ_TexCoord_Y.emplace_back(lPosition.x, lPosition.y, lPosition.z, lTexCoord.y);

				s = static_cast<GLfloat>(i) / lSubdivs;
				lSphereFace[f]->get(s, t, lPosition, lTexCoord);
				lPosition_XYZ_TexCoord_X.emplace_back(lPosition.x, lPosition.y, lPosition.z, lTexCoord.x);
				lNormal_XYZ_TexCoord_Y.emplace_back(lPosition.x, lPosition.y, lPosition.z, lTexCoord.y);

				lIndices.push_back(lCounter + 0);
				lIndices.push_back(lCounter + 1);
				lIndices.push_back(lCounter + 2);
				lIndices.push_back(lCounter + 2);
				lIndices.push_back(lCounter + 3);
				lIndices.push_back(lCounter + 0);

				lCounter += 4;
			}
		}
		delete lSphereFace[f];
	}

	sUnitSpherePUN = Mesh::create();
	sUnitSpherePUN->name = "UnitSphere";
	sUnitSpherePUN->set(lIndices, lPosition_XYZ_TexCoord_X, lNormal_XYZ_TexCoord_Y);

	/* Set up the unit cone */

	lSubdivisions *= 4; // HACKY!
	const GLfloat lAngle = 2.0f * static_cast<float>(M_PI) / static_cast<float>(lSubdivisions);
	GLfloat lSin, lCos, lNextSin(0.0f), lNextCos(1.0f);

	lPosition_XYZ_TexCoord_X.clear();
	lNormal_XYZ_TexCoord_Y.clear();
	lIndices.clear();
	lPosition_XYZ_TexCoord_X.emplace_back(0.0f, 0.0f, 0.0f, 0.5f);
	lNormal_XYZ_TexCoord_Y.emplace_back(0.0f, 0.0f, 1.0f, 0.5f);
	lCounter = 1;
	for (GLsizei lDiv = 0; lDiv < lSubdivisions; ++lDiv) // base
	{
		lCos = lNextCos;
		lSin = lNextSin;
		lNextCos = (lDiv == lSubdivisions - 1) ? 1.0f : std::cos(static_cast<float>(lDiv + 1) * lAngle);
		lNextSin = (lDiv == lSubdivisions - 1) ? 0.0f : std::sin(static_cast<float>(lDiv + 1) * lAngle);

		const gintonic::vec3f P0(    0.0f,      0.0f, -1.0f);
		const gintonic::vec3f P1(    lCos,      lSin,  0.0f);
		const gintonic::vec3f P2(lNextCos,  lNextSin,  0.0f);
		const auto N = cross(P2 - P0, P1 - P0).normalize();

		lPosition_XYZ_TexCoord_X.emplace_back(    lCos,     lSin, 0.0f,            (1.0f + lCos) * 0.5f);
		lPosition_XYZ_TexCoord_X.emplace_back(lNextCos, lNextSin, 0.0f,        (1.0f + lNextCos) * 0.5f);
		lNormal_XYZ_TexCoord_Y.emplace_back(      0.0f,     0.0f, 1.0f,     1.0f - (1.0f + lSin) * 0.5f);
		lNormal_XYZ_TexCoord_Y.emplace_back(      0.0f,     0.0f, 1.0f, 1.0f - (1.0f + lNextSin) * 0.5f);

		lPosition_XYZ_TexCoord_X.emplace_back(    0.0f,      0.0f, -1.0f,                     0.5f);
		lPosition_XYZ_TexCoord_X.emplace_back(lNextCos,  lNextSin,  0.0f, (1.0f + lNextCos) * 0.5f);
		lPosition_XYZ_TexCoord_X.emplace_back(    lCos,      lSin,  0.0f,     (1.0f + lCos) * 0.5f);
		lNormal_XYZ_TexCoord_Y.emplace_back(       N.x,      N.y,    N.z,                     0.5f);
		lNormal_XYZ_TexCoord_Y.emplace_back(       N.x,      N.y,    N.z, (1.0f + lNextSin) * 0.5f);
		lNormal_XYZ_TexCoord_Y.emplace_back(       N.x,      N.y,    N.z,     (1.0f + lSin) * 0.5f);
		

		lIndices.push_back(0);
		lIndices.push_back(lCounter);
		lIndices.push_back(lCounter + 1);
		lIndices.push_back(lCounter + 2);
		lIndices.push_back(lCounter + 3);
		lIndices.push_back(lCounter + 4);
		lCounter += 5;
	}

	sUnitConePUN = Mesh::create();
	sUnitConePUN->name = "UnitCone";
	sUnitConePUN->set(lIndices, lPosition_XYZ_TexCoord_X, lNormal_XYZ_TexCoord_Y);

	/* Set up the unit cylinder */

	lNextSin = 0.0f;
	lNextCos = 1.0f;
	lPosition_XYZ_TexCoord_X.clear();
	lNormal_XYZ_TexCoord_Y.clear();
	lIndices.clear();
	lPosition_XYZ_TexCoord_X.emplace_back(0.0f, 0.0f, 1.0f, 0.5f);
	lPosition_XYZ_TexCoord_X.emplace_back(0.0f, 0.0f, -1.0f, 0.5f);
	lNormal_XYZ_TexCoord_Y.emplace_back(0.0f, 0.0f, 1.0f, 0.5f);
	lNormal_XYZ_TexCoord_Y.emplace_back(0.0f, 0.0f, -1.0f, 0.5f);
	lCounter = 2;
	for (GLsizei lDiv = 0; lDiv < lSubdivisions; ++lDiv) // front
	{
		lCos = lNextCos;
		lSin = lNextSin;
		lNextCos = (lDiv == lSubdivisions - 1) ? 1.0f : std::cos(static_cast<float>(lDiv + 1) * lAngle);
		lNextSin = (lDiv == lSubdivisions - 1) ? 0.0f : std::sin(static_cast<float>(lDiv + 1) * lAngle);

		lPosition_XYZ_TexCoord_X.emplace_back(    lCos,     lSin, 1.0f,            (1.0f + lCos) * 0.5f);
		lPosition_XYZ_TexCoord_X.emplace_back(lNextCos, lNextSin, 1.0f,        (1.0f + lNextCos) * 0.5f);
		lNormal_XYZ_TexCoord_Y.emplace_back(      0.0f,     0.0f, 1.0f,     1.0f - (1.0f + lSin) * 0.5f);
		lNormal_XYZ_TexCoord_Y.emplace_back(      0.0f,     0.0f, 1.0f, 1.0f - (1.0f + lNextSin) * 0.5f);

		lPosition_XYZ_TexCoord_X.emplace_back(    lCos,     lSin, -1.0f,     (1.0f + lCos) * 0.5f);
		lPosition_XYZ_TexCoord_X.emplace_back(lNextCos, lNextSin, -1.0f, (1.0f + lNextCos) * 0.5f);
		lNormal_XYZ_TexCoord_Y.emplace_back(      0.0f,     0.0f, -1.0f,     (1.0f + lSin) * 0.5f);
		lNormal_XYZ_TexCoord_Y.emplace_back(      0.0f,     0.0f, -1.0f, (1.0f + lNextSin) * 0.5f);

		lIndices.push_back(0);
		lIndices.push_back(lCounter + 0);
		lIndices.push_back(lCounter + 1);

		lIndices.push_back(lCounter + 3);
		lIndices.push_back(lCounter + 1);
		lIndices.push_back(lCounter + 0);
		
		lIndices.push_back(lCounter + 3);
		lIndices.push_back(lCounter + 0);
		lIndices.push_back(lCounter + 2);
		
		lIndices.push_back(1);
		lIndices.push_back(lCounter + 3);
		lIndices.push_back(lCounter + 2);

		lCounter += 4;
	}
	
	sUnitCylinderPUN = Mesh::create();
	sUnitCylinderPUN->name = "UnitCylinder";
	sUnitCylinderPUN->set(lIndices, lPosition_XYZ_TexCoord_X, lNormal_XYZ_TexCoord_Y);
}

} // namespace gintonic