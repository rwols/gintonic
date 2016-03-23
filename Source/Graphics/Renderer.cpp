#include "Renderer.hpp"

#include "../Foundation/exception.hpp"
#include "../Math/vec4f.hpp"

#include "GeometryBuffer.hpp"
#include "Mesh.hpp"
#include "basic_shapes.hpp"
#include "ShaderPrograms.hpp"
#include "Material.hpp"
#include "Light.hpp"

#include "../Camera.hpp"
#include "../Entity.hpp"
#include "../EntityVisitor.hpp"

#include <iostream>
#include <SDL.h>

#ifdef BOOST_MSVC
	#include <Objbase.h> // for CoInitializeEx
#endif

#define HAS_DIFFUSE_TEXTURE 1
#define HAS_SPECULAR_TEXTURE 2
#define HAS_NORMAL_TEXTURE 4

#define GBUFFER_TEX_DIFFUSE 0
#define GBUFFER_TEX_SPECULAR 1
#define GBUFFER_TEX_NORMAL 2

namespace gintonic {

// Anybody want to make the Renderer a proper singleton class?

GeometryBuffer* sGeometryBuffer = nullptr;

std::shared_ptr<Camera> sDefaultCamera = std::make_shared<Camera>();

std::vector<std::shared_ptr<Entity>> sShadowCastingLightEntities;
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
				sShadowCastingLightEntities.push_back(entity);
				if (!entity->shadowBuffer)
				{
					entity->light->initializeShadowBuffer(entity);
				}
			}
			if (entity->material && entity->mesh)
			{
				sShadowCastingGeometryEntities.emplace_back(std::move(entity));
			}
		}
		else // non-shadow casting entity
		{
			if (entity->light)
			{
				sNonShadowCastingLightEntities.push_back(entity);
			}
			if (entity->material && entity->mesh)
			{
				sNonShadowCastingGeometryEntities.emplace_back(std::move(entity));
			}
		}
		return true;
	}
};

// ALL the global variables.

#ifdef ENABLE_DEBUG_TRACE
std::shared_ptr<Font> sDebugFont = nullptr;
FontStream* sDebugStream = nullptr;
#endif

SDL_Window* sWindow = nullptr;
SDL_GLContext sContext;
SDL_Event sEvent = SDL_Event();

bool Renderer::sShouldClose = false;
bool Renderer::sFullscreen = false;
bool Renderer::sRenderInWireframeMode = false;
bool Renderer::sViewGeometryBuffers = false;
int Renderer::sWidth = 800;
int Renderer::sHeight = 640;
float Renderer::sAspectRatio = (float)Renderer::sWidth / (float)Renderer::sHeight;

Renderer::time_point_type Renderer::sStartTime;
Renderer::duration_type Renderer::sDeltaTime = Renderer::duration_type();
Renderer::duration_type Renderer::sPrevElapsedTime = Renderer::duration_type();
Renderer::duration_type Renderer::sElapsedTime = Renderer::duration_type();
boost::circular_buffer<Renderer::duration_type> Renderer::s_circle_buffer = boost::circular_buffer<Renderer::duration_type>();
vec2f Renderer::sMouseDelta = vec2f(0,0);

Uint8* sKeyPrevState = nullptr;
const Uint8* sKeyState = nullptr;
int sKeyStateCount;

bool Renderer::s_matrix_P_dirty = true;
bool Renderer::s_matrix_VM_dirty = true;
bool Renderer::s_matrix_PVM_dirty = true;
bool Renderer::s_matrix_N_dirty = true;

mat4f Renderer::s_matrix_P = mat4f();
mat4f Renderer::s_matrix_V = mat4f();
mat4f Renderer::s_matrix_M = mat4f();
mat4f Renderer::s_matrix_VM = mat4f();
mat4f Renderer::s_matrix_PVM = mat4f();
mat3f Renderer::s_matrix_N = mat3f();

std::shared_ptr<Entity> Renderer::sCameraEntity = std::shared_ptr<Entity>(nullptr);
vec3f Renderer::sCameraPosition = vec3f(0.0f, 0.0f, 0.0f);

std::shared_ptr<Mesh> Renderer::sUnitQuadPUN        = nullptr;
std::shared_ptr<Mesh> Renderer::sUnitCubePUN        = nullptr;
std::shared_ptr<Mesh> Renderer::sUnitCubePUNTB      = nullptr;
std::shared_ptr<Mesh> Renderer::sFlippedUnitCubePUN = nullptr;
std::shared_ptr<Mesh> Renderer::sUnitSpherePUN      = nullptr;
std::shared_ptr<Mesh> Renderer::sUnitConePUN        = nullptr;
std::shared_ptr<Mesh> Renderer::sUnitCylinderPUN    = nullptr;

boost::signals2::signal<void(wchar_t)> Renderer::onCharTyped;
boost::signals2::signal<void(double, double)> Renderer::onMouseScroll;
boost::signals2::signal<void(double, double)> Renderer::onMouseMove;
boost::signals2::signal<void(int, int, int, int)> Renderer::onKeyPress;
boost::signals2::signal<void(int, int, int)> Renderer::onMousePressed;
boost::signals2::signal<void(int, int)> Renderer::onWindowResize;
boost::signals2::signal<void(void)> Renderer::onMouseEnter;
boost::signals2::signal<void(void)> Renderer::onMouseLeave;
boost::signals2::signal<void(void)> Renderer::onAboutToClose;

#ifdef ENABLE_DEBUG_TRACE
FontStream& Renderer::cerr() { return *sDebugStream; }
#endif

void Renderer::getElapsedAndDeltaTime(double& t, double& dt)
{
	using std::chrono::duration_cast;
	using std::chrono::nanoseconds;
	t = static_cast<double>(duration_cast<nanoseconds>(elapsedTime()).count()) / double(1e9);
	dt = static_cast<double>(duration_cast<nanoseconds>(deltaTime()).count()) / double(1e9);
}

void Renderer::init(
	const char* title, 
	std::shared_ptr<Entity> cameraEntity, 
	const bool fullscreen, 
	const int width, 
	const int height)
{

	DEBUG_PRINT;
	bool was_already_initialized;
	if (isInitialized())
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

	sFullscreen = fullscreen;

	DEBUG_PRINT;

	if (sFullscreen)
	{
		const auto lNumDisplays = SDL_GetNumVideoDisplays();
		if (lNumDisplays <= 0)
		{
			std::clog << "No displays.\n";
			return;
		}
		SDL_Rect lRect;
		SDL_GetDisplayBounds(0, &lRect);
		sWidth = lRect.w;
		sHeight = lRect.h;
	}
	else
	{
		sWidth = width;
		sHeight = height;
	}

	DEBUG_PRINT;

	sAspectRatio = (float) sWidth / (float) sHeight;

	if (was_already_initialized == false) std::atexit(SDL_Quit);

	Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
	if (sFullscreen) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	sWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, sWidth, sHeight, flags);
	// SDL_GetWindowSize(sWindow, &sWidth, &sHeight);

	DEBUG_PRINT;

	//
	// We start by trying to obtain an OpenGL 4.1 context.
	//
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

	DEBUG_PRINT;

	SDL_GetKeyboardState(&sKeyStateCount);
	sKeyPrevState = new Uint8[sKeyStateCount];
	std::memset(sKeyPrevState, 0, sizeof(Uint8) * sKeyStateCount);
	sKeyState = SDL_GetKeyboardState(nullptr);
	
	if (was_already_initialized == false) std::atexit(Renderer::release);
	
	vsync(true);
	glClearColor(0.03f, 0.03f, 0.03f, 0.0f);
	
	sStartTime = clock_type::now();

	#ifdef BOOST_MSVC
	CoInitialize(nullptr); // initialize COM
	#endif

	DEBUG_PRINT;

	//
	// Initialize framebuffers
	//
	try
	{
		sGeometryBuffer = new GeometryBuffer(sWidth, sHeight);	
	}
	catch (const OpenGL::Framebuffer::Exception& framebufferException)
	{
		exception lException(name());
		lException.append(": GeometryBuffer failed to initialize: ");
		lException.append(framebufferException.what());
		throw lException;
	}

	//
	// Initialize default camera and camera entity
	//
	sDefaultCamera->name = "DefaultCamera";
	sDefaultCamera->setNearPlane(1.0f);
	sDefaultCamera->setFarPlane(1000.0f);
	sDefaultCamera->setProjectionType(Camera::kPerspectiveProjection);
	setCameraEntity(std::move(cameraEntity));

	DEBUG_PRINT;

	//
	// Initializer shaders
	//
	init_shaders();

	DEBUG_PRINT;

	//
	// Initialize basic shapes
	//

	initializeBasicShapes();

	DEBUG_PRINT;

	//
	// Initialize debug variables
	//
	#ifdef ENABLE_DEBUG_TRACE
	// Determine the point size. We take 1/100 of the width
	const unsigned int lPointSize = static_cast<unsigned int>((float)sWidth / 80.0f);
	PRINT_VAR(sWidth);
	PRINT_VAR(sHeight);
	PRINT_VAR(lPointSize);
	sDebugFont = std::make_shared<Font>("Resources/Inconsolata-Regular.ttf", lPointSize);
	sDebugStream = new FontStream();
	sDebugStream->open(sDebugFont);
	#endif

	DEBUG_PRINT;
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

void Renderer::resize(const int width, const int height)
{
	//
	// resize viewport
	//
	sWidth = width;
	sHeight = height;
	sAspectRatio = (float)sWidth / (float)sHeight;
	s_matrix_P_dirty = true;
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
		throw lException;
	}
	
	//
	// Update projection matrix
	//
	sCameraEntity->camera->setWidth(static_cast<float>(sWidth));
	sCameraEntity->camera->setHeight(static_cast<float>(sHeight));

	DEBUG_PRINT;
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
	sCameraEntity->camera->setWidth(static_cast<float>(sWidth));
	sCameraEntity->camera->setHeight(static_cast<float>(sHeight));
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

void Renderer::setWireframeMode(const bool yesOrNo) noexcept
{
	sRenderInWireframeMode = yesOrNo;
}

void Renderer::setViewGeometryBuffers(const bool yesOrNo) noexcept
{
	sViewGeometryBuffers = yesOrNo;
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
	if (sGeometryBuffer)
	{
		delete sGeometryBuffer;
		sGeometryBuffer = nullptr;
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
	prepareRendering();
	renderGeometry();
	if (sViewGeometryBuffers)
	{
		sGeometryBuffer->blitDrawbuffersToPostProcessingBuffer(sWidth, sHeight);
	}
	else
	{
		renderShadows();
		renderLights();
	}
	finalizeRendering();
	processEvents();
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
	sGeometryBuffer->prepareGeometryPhase();
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	const auto& lMaterialShaderProgram = MaterialShaderProgram::get();
	lMaterialShaderProgram.activate();
	lMaterialShaderProgram.setMaterialDiffuseTexture(GBUFFER_TEX_DIFFUSE);
	lMaterialShaderProgram.setMaterialSpecularTexture(GBUFFER_TEX_SPECULAR);
	lMaterialShaderProgram.setMaterialNormalTexture(GBUFFER_TEX_NORMAL);
	lMaterialShaderProgram.setInstancedRendering(0);

	for (auto lEntity : sShadowCastingGeometryEntities)
	{
		GLint lMaterialFlag = 0;
		GLint lHasTangentsAndBitangents = 0;

		setModelMatrix(lEntity->globalTransform());

		if (lEntity->material->diffuseTexture)
		{
			lMaterialFlag |= HAS_DIFFUSE_TEXTURE;
			lEntity->material->diffuseTexture->bind(GBUFFER_TEX_DIFFUSE);
		}
		if (lEntity->material->specularTexture)
		{
			lMaterialFlag |= HAS_SPECULAR_TEXTURE;
			lEntity->material->specularTexture->bind(GBUFFER_TEX_SPECULAR);
		}
		if (lEntity->material->normalTexture)
		{
			lMaterialFlag |= HAS_NORMAL_TEXTURE;
			lEntity->material->normalTexture->bind(GBUFFER_TEX_NORMAL);
		}

		if (lEntity->mesh->hasTangentsAndBitangents()) lHasTangentsAndBitangents = 1;

		lMaterialShaderProgram.setMaterialDiffuseColor(lEntity->material->diffuseColor);
		lMaterialShaderProgram.setMaterialSpecularColor(lEntity->material->specularColor);
		lMaterialShaderProgram.setMaterialFlag(lMaterialFlag);
		lMaterialShaderProgram.setHasTangentsAndBitangents(lHasTangentsAndBitangents);
		lMaterialShaderProgram.setMatrixPVM(matrix_PVM());
		lMaterialShaderProgram.setMatrixVM(matrix_VM());
		lMaterialShaderProgram.setMatrixN(matrix_N());

		lEntity->mesh->draw();
	}
	for (const auto lEntity : sNonShadowCastingGeometryEntities)
	{
		GLint lMaterialFlag = 0;
		GLint lHasTangentsAndBitangents = 0;

		setModelMatrix(lEntity->globalTransform());

		if (lEntity->material->diffuseTexture)
		{
			lMaterialFlag |= HAS_DIFFUSE_TEXTURE;
			lEntity->material->diffuseTexture->bind(GBUFFER_TEX_DIFFUSE);
		}
		if (lEntity->material->specularTexture)
		{
			lMaterialFlag |= HAS_SPECULAR_TEXTURE;
			lEntity->material->specularTexture->bind(GBUFFER_TEX_SPECULAR);
		}
		if (lEntity->material->normalTexture)
		{
			lMaterialFlag |= HAS_NORMAL_TEXTURE;
			lEntity->material->normalTexture->bind(GBUFFER_TEX_NORMAL);
		}

		if (lEntity->mesh->hasTangentsAndBitangents()) lHasTangentsAndBitangents = 1;

		lMaterialShaderProgram.setMaterialDiffuseColor(lEntity->material->diffuseColor);
		lMaterialShaderProgram.setMaterialSpecularColor(lEntity->material->specularColor);
		lMaterialShaderProgram.setMaterialFlag(lMaterialFlag);
		lMaterialShaderProgram.setHasTangentsAndBitangents(lHasTangentsAndBitangents);
		lMaterialShaderProgram.setMatrixPVM(matrix_PVM());
		lMaterialShaderProgram.setMatrixVM(matrix_VM());
		lMaterialShaderProgram.setMatrixN(matrix_N());

		lEntity->mesh->draw();
	}
}

void Renderer::renderShadows() noexcept
{
	ShadowShaderProgram::get().activate();
	ShadowShaderProgram::get().setInstancedRendering(0);
	for (auto lEntity : sShadowCastingLightEntities)
	{
		lEntity->shadowBuffer->collect(*lEntity, sShadowCastingGeometryEntities);
	}
}

void Renderer::renderLights() noexcept
{
	sGeometryBuffer->prepareLightingPhase();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	// Ambient lighting
	const auto& lAmbientLightShaderProgram = AmbientLightShaderProgram::get();
	lAmbientLightShaderProgram.activate();
	lAmbientLightShaderProgram.setGeometryBufferDiffuseTexture(GBUFFER_DIFFUSE);
	lAmbientLightShaderProgram.setViewportSize(viewportSize());
	lAmbientLightShaderProgram.setLightIntensity(vec4f(1.0f, 1.0f, 1.0f, 1.0f));
	sUnitQuadPUN->draw();
	
	for (auto lEntity : sShadowCastingLightEntities)
	{
		lEntity->light->shine(*lEntity);
	}
	for (auto lEntity : sNonShadowCastingLightEntities)
	{
		lEntity->light->shine(*lEntity);
	}
}

void Renderer::finalizeRendering() noexcept
{
	sShadowCastingLightEntities.clear();
	sShadowCastingGeometryEntities.clear();
	sNonShadowCastingLightEntities.clear();
	sNonShadowCastingGeometryEntities.clear();
	sEntitiesLock.release();

	#ifdef ENABLE_DEBUG_TRACE
	FlatTextShaderProgram::get().activate();
	FlatTextShaderProgram::get().setColor(vec3f(1.0f, 1.0f, 1.0f));
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	sDebugStream->close();
	#endif

	sGeometryBuffer->finalize(sWidth, sHeight);

	SDL_GL_SwapWindow(sWindow);

	#ifdef ENABLE_DEBUG_TRACE
	sDebugStream->open(sDebugFont);
	#endif
}

void Renderer::processEvents() noexcept
{
	sPrevElapsedTime = sElapsedTime;
	sElapsedTime = clock_type::now() - sStartTime;
	sDeltaTime = sElapsedTime - sPrevElapsedTime;

	std::memcpy(sKeyPrevState, sKeyState, sizeof(Uint8) * sKeyStateCount);
	sKeyState = SDL_GetKeyboardState(nullptr);

	sMouseDelta = 0.0f;
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
		case SDL_KEYDOWN:
			break;
		case SDL_KEYUP:
			break;
		case SDL_MOUSEMOTION:
			sMouseDelta.x += (float)sEvent.motion.xrel;
			sMouseDelta.y += (float)sEvent.motion.yrel;
			break;
		case SDL_QUIT:
			close();
			break;
		default:
			break;
		}
	}
	if (sMouseDelta.x != 0.0f || sMouseDelta.y != 0.0f)
	{
		onMouseMove(sMouseDelta.x, sMouseDelta.y);
	}

	// Update the WORLD->VIEW matrix.
	sCameraEntity->getViewMatrix(s_matrix_V);
	sCameraPosition = vec3f((sCameraEntity->globalTransform() * vec4f(0.0f, 0.0f, 0.0f, 1.0f)).data);
}

// void Renderer::renderRawGeometryBuffers() noexcept
// {
// 	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
// 	glBindFramebuffer(GL_READ_FRAMEBUFFER, s_fbo);

// 	const GLsizei halfwidth = (GLsizei)(width() / 2.0f);
// 	const GLsizei halfheight = (GLsizei)(height() / 2.0f);

// 	glReadBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_POSITION);
// 	glBlitFramebuffer(0, 0, width(), height(), 0, 0, halfwidth, halfheight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
// 	glReadBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_NORMAL);
// 	glBlitFramebuffer(0, 0, width(), height(), 0, halfheight, halfwidth, height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
// 	glReadBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_DIFFUSE);
// 	glBlitFramebuffer(0, 0, width(), height(), halfwidth, halfheight, width(), height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
// 	glReadBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_SPECULAR);
// 	glBlitFramebuffer(0, 0, width(), height(), halfwidth, 0, width(), halfheight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
// }


// void Renderer::begin_geometry_pass()
// {
// 	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, s_fbo);
// 	glDrawBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_FINAL_COLOR);
// 	glClear(GL_COLOR_BUFFER_BIT);
// 	constexpr GLenum DrawBuffers[4] = 
// 	{
// 		GL_COLOR_ATTACHMENT0 + GBUFFER_POSITION, 
// 		GL_COLOR_ATTACHMENT0 + GBUFFER_DIFFUSE, 
// 		GL_COLOR_ATTACHMENT0 + GBUFFER_SPECULAR, 
// 		GL_COLOR_ATTACHMENT0 + GBUFFER_NORMAL
// 	}; 
// 	glDrawBuffers(4, DrawBuffers);
// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
// 	glDepthMask(GL_TRUE);
// 	glEnable(GL_DEPTH_TEST);
// 	glDisable(GL_BLEND);
// 	glEnable(GL_CULL_FACE);
// 	glCullFace(GL_BACK);
// }

// void Renderer::begin_stencil_pass()
// {
// 	glDrawBuffer(GL_NONE);
// 	glEnable(GL_DEPTH_TEST);
// 	glDisable(GL_CULL_FACE);
// 	glClear(GL_STENCIL_BUFFER_BIT);
// 	glStencilFunc(GL_ALWAYS, 0, 0);
// 	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
// 	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
// }

// void Renderer::begin_light_pass()
// {
// 	glDrawBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_FINAL_COLOR);
// 	for (unsigned int i = 0; i < GBUFFER_FINAL_COLOR; ++i) 
// 	{
// 		glActiveTexture(GL_TEXTURE0 + i);
// 		glBindTexture(GL_TEXTURE_2D, s_textures[i]);
// 	}
// 	glDisable(GL_DEPTH_TEST);
// 	glEnable(GL_BLEND);
// 	glBlendEquation(GL_FUNC_ADD);
// 	glBlendFunc(GL_ONE, GL_ONE);
// 	// glClear(GL_COLOR_BUFFER_BIT);
// }

// void Renderer::set_read_buffer(const enum GBUFFER type)
// {
// 	glReadBuffer(GL_COLOR_ATTACHMENT0 + type);
// }

// void Renderer::blit_drawbuffers_to_screen()
// {
// 	// Take s_fbo as the active framebuffer.
// 	// We blit the geometry stuff into yet another color attachment.
// 	// This final color attachment gets blitted to the screen in the update()
// 	// method which is (or rather should be) called at the end of the main render loop.
// 	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, s_fbo);
// 	glBindFramebuffer(GL_READ_FRAMEBUFFER, s_fbo);
// 	glDrawBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_FINAL_COLOR);

// 	const GLsizei halfwidth = (GLsizei)(width() / 2.0f);
// 	const GLsizei halfheight = (GLsizei)(height() / 2.0f);

// 	set_read_buffer(GBUFFER_POSITION);
// 	glBlitFramebuffer(0, 0, width(), height(), 0, 0, halfwidth, halfheight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
// 	set_read_buffer(GBUFFER_NORMAL);
// 	glBlitFramebuffer(0, 0, width(), height(), 0, halfheight, halfwidth, height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
// 	set_read_buffer(GBUFFER_DIFFUSE);
// 	glBlitFramebuffer(0, 0, width(), height(), halfwidth, halfheight, width(), height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
// 	set_read_buffer(GBUFFER_SPECULAR);
// 	glBlitFramebuffer(0, 0, width(), height(), halfwidth, 0, width(), halfheight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
// }

// void Renderer::blit_drawbuffers_to_screen(FontStream& stream)
// {
// 	// Take s_fbo as the active framebuffer.
// 	// We blit the geometry stuff into yet another color attachment.
// 	// This final color attachment gets blitted to the screen in the update()
// 	// method which is (or rather should be) called at the end of the main render loop.
// 	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, s_fbo);
// 	glBindFramebuffer(GL_READ_FRAMEBUFFER, s_fbo);
// 	glDrawBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_FINAL_COLOR);

// 	const GLsizei halfwidth = (GLsizei)(width() / 2.0f);
// 	const GLsizei halfheight = (GLsizei)(height() / 2.0f);		
	
// 	set_read_buffer(GBUFFER_POSITION);
// 	glBlitFramebuffer(0, 0, width(), height(), 0, 0, halfwidth, halfheight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	
// 	set_read_buffer(GBUFFER_NORMAL);
// 	glBlitFramebuffer(0, 0, width(), height(), 0, halfheight, halfwidth, height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
	
// 	set_read_buffer(GBUFFER_DIFFUSE);
// 	glBlitFramebuffer(0, 0, width(), height(), halfwidth, halfheight, width(), height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
	
// 	set_read_buffer(GBUFFER_SPECULAR);
// 	glBlitFramebuffer(0, 0, width(), height(), halfwidth, 0, width(), halfheight, GL_COLOR_BUFFER_BIT, GL_LINEAR);


// 	glDisable(GL_CULL_FACE);
// 	glEnable(GL_BLEND);
// 	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
// 	FlatTextShaderProgram::get().activate();
// 	FlatTextShaderProgram::get().setColor(vec3f(1.0f, 1.0f, 1.0f));

// 	stream << "GBUFFER_NORMAL" << std::endl;
// 	stream->position.x += 1.0f;
// 	stream << "GBUFFER_DIFFUSE" << std::endl;
// 	stream->position.y -= 1.0f;
// 	stream << "GBUFFER_SPECULAR" << std::endl;
// 	stream->position.x -= 1.0f;
// 	stream << "GBUFFER_POSITION" << std::endl;

// 	glEnable(GL_CULL_FACE);
// }

// void Renderer::ambient_light_pass() noexcept
// {
// 	const auto& s = get_AmbientLightShaderProgram();
// 	s.activate();
// 	s.set_gbuffer_diffuse(GBUFFER_DIFFUSE);
// 	s.set_viewport_size(viewportSize());
// 	s.set_light_intensity(vec4f(1.0f, 1.0f, 1.0f, 1.0f));
// 	get_unit_quad_P().draw();
// }

void Renderer::update_matrix_P()
{
	if (s_matrix_P_dirty)
	{
		s_matrix_P = sCameraEntity->camera->projectionMatrix();
		s_matrix_P_dirty = false;
	}
}

void Renderer::update_matrix_VM()
{
	if (s_matrix_VM_dirty)
	{
		s_matrix_VM = s_matrix_V * s_matrix_M;
		s_matrix_VM_dirty = false;
	}
}

void Renderer::update_matrix_PVM()
{
	update_matrix_P();
	update_matrix_VM();
	if (s_matrix_PVM_dirty)
	{
		s_matrix_PVM = s_matrix_P * s_matrix_VM;
		s_matrix_PVM_dirty = false;
	}
}

void Renderer::update_matrix_N()
{
	update_matrix_VM();
	if (s_matrix_N_dirty)
	{
		s_matrix_N = s_matrix_VM.upper_left_33().invert().transpose();
		s_matrix_N_dirty = false;
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

// void Renderer::submitEntityRecursiveHelper(std::shared_ptr<Entity> current)
// {
// 	if ((current->mesh && current->material) || current->light)
// 	{
// 		sFutureQueue.push_back(current);
// 	}
// 	for (auto child : *current)
// 	{
// 		submitEntityRecursiveHelper(child);
// 	}
// }

std::shared_ptr<Entity> Renderer::createGizmo()
{
	auto lRedMaterial = std::make_shared<Material>();
	lRedMaterial->name = "Red";
	lRedMaterial->diffuseColor = vec4f(1.0f, 0.0f, 0.0f, 0.0f);
	lRedMaterial->specularColor = vec4f(0.0f, 0.0f, 0.0f, 0.0f);

	auto lGreenMaterial = std::make_shared<Material>();
	lGreenMaterial->name = "Green";
	lGreenMaterial->diffuseColor = vec4f(0.0f, 1.0f, 0.0f, 0.0f);
	lGreenMaterial->specularColor = vec4f(0.0f, 0.0f, 0.0f, 0.0f);

	auto lBlueMaterial = std::make_shared<Material>();
	lBlueMaterial->name = "Blue";
	lBlueMaterial->diffuseColor = vec4f(0.0f, 0.0f, 1.0f, 0.0f);
	lBlueMaterial->specularColor = vec4f(0.0f, 0.0f, 0.0f, 0.0f);

	auto lRightArrowCone = std::make_shared<Entity>
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

	auto lRightArrowCylinder = std::make_shared<Entity>
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

	auto lUpArrowCone = std::make_shared<Entity>
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

	auto lUpArrowCylinder = std::make_shared<Entity>
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

	auto lForwardArrowCone = std::make_shared<Entity>
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

	auto lBackwardArrowCylinder = std::make_shared<Entity>
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

	auto lGizmoRoot = std::make_shared<Entity>
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

	DEBUG_PRINT;

	INIT_SHADER(ShadowShaderProgram);
	INIT_SHADER(MaterialShaderProgram);
	INIT_SHADER(AmbientLightShaderProgram);
	INIT_SHADER(DirectionalLightShaderProgram);
	INIT_SHADER(PointLightShaderProgram);
	INIT_SHADER(SpotLightShaderProgram);
	INIT_SHADER(FlatTextShaderProgram);

	DEBUG_PRINT;
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

void Renderer::initializeBasicShapes()
{
	/* Set up the unit quad */

	sUnitQuadPUN.reset(new Mesh());
	sUnitQuadPUN->name = "UnitQuad";
	sUnitQuadPUN->set(
		{
			0,1,2,2,1,3 // indices
		},
		{
			{-1.0f, -1.0f, 0.0f, -1.0f}, // position_XYZ_texcoords_X
			{ 1.0f, -1.0f, 0.0f,  1.0f},
			{-1.0f,  1.0f, 0.0f, -1.0f},
			{ 1.0f,  1.0f, 0.0f,  1.0f}
		},
		{
			{ 0.0f,  0.0f, 1.0f, -1.0f}, // normal_XYZ_texcoords_Y
			{ 0.0f,  0.0f, 1.0f, -1.0f},
			{ 0.0f,  0.0f, 1.0f,  1.0f},
			{ 0.0f,  0.0f, 1.0f,  1.0f}
		}
	);

	/* Set up the unit cube */

	sUnitCubePUN.reset(new Mesh());
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

	sUnitCubePUNTB.reset(new Mesh());
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

	sFlippedUnitCubePUN.reset(new Mesh());
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
	GLsizei lSubdivisions = 8;
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

	sUnitSpherePUN.reset(new Mesh());
	sUnitSpherePUN->name = "UnitSphere";
	sUnitSpherePUN->set(lIndices, lPosition_XYZ_TexCoord_X, lNormal_XYZ_TexCoord_Y);

	/* Set up the unit cone */

	lSubdivisions *= 6; // HACKY!
	// lSubdivisions = 4;
	const GLfloat lAngle = 2.0f * static_cast<float>(M_PI) / static_cast<float>(lSubdivisions);
	GLfloat lSin, lCos, lNextSin(0.0f), lNextCos(1.0f);

	lPosition_XYZ_TexCoord_X.clear();
	lNormal_XYZ_TexCoord_Y.clear();
	lIndices.clear();
	for (GLsizei lDiv = 0; lDiv < lSubdivisions; ++lDiv)
	{
		lCos = lNextCos;
		lSin = lNextSin;
		lNextCos = std::cos(static_cast<float>(lDiv + 1) * lAngle);
		lNextSin = std::sin(static_cast<float>(lDiv + 1) * lAngle);

		lPosition_XYZ_TexCoord_X.emplace_back(    0.0f,     0.0f, 0.0f,                            0.5f);
		lPosition_XYZ_TexCoord_X.emplace_back(    lCos,     lSin, 0.0f,            (1.0f + lCos) * 0.5f);
		lPosition_XYZ_TexCoord_X.emplace_back(lNextCos, lNextSin, 0.0f,        (1.0f + lNextCos) * 0.5f);
		lNormal_XYZ_TexCoord_Y.emplace_back(      0.0f,     0.0f, 1.0f,                            0.5f);
		lNormal_XYZ_TexCoord_Y.emplace_back(      0.0f,     0.0f, 1.0f,     1.0f - (1.0f + lSin) * 0.5f);
		lNormal_XYZ_TexCoord_Y.emplace_back(      0.0f,     0.0f, 1.0f, 1.0f - (1.0f + lNextSin) * 0.5f);

		lIndices.push_back(3 * lDiv + 0);
		lIndices.push_back(3 * lDiv + 1);
		lIndices.push_back(3 * lDiv + 2);
	}

	lNextSin = 0.0f;
	lNextCos = 1.0f;
	for (GLsizei lDiv = 0; lDiv < lSubdivisions; ++lDiv)
	{
		lCos = lNextCos;
		lSin = lNextSin;
		lNextCos = std::cos(static_cast<float>(lDiv + 1) * lAngle);
		lNextSin = std::sin(static_cast<float>(lDiv + 1) * lAngle);

		const gintonic::vec3f P0(    0.0f,      0.0f, -1.0f);
		const gintonic::vec3f P1(    lCos,      lSin,  0.0f);
		const gintonic::vec3f P2(lNextCos,  lNextSin,  0.0f);
		const auto N = cross(P2 - P0, P1 - P0).normalize();

		lPosition_XYZ_TexCoord_X.emplace_back(    0.0f,      0.0f, -1.0f,                     0.5f);
		lPosition_XYZ_TexCoord_X.emplace_back(    lCos,     -lSin,  0.0f,     (1.0f + lCos) * 0.5f);
		lPosition_XYZ_TexCoord_X.emplace_back(lNextCos, -lNextSin,  0.0f, (1.0f + lNextCos) * 0.5f);
		lNormal_XYZ_TexCoord_Y.emplace_back(       N.x,      N.y,    N.z,                     0.5f);
		lNormal_XYZ_TexCoord_Y.emplace_back(       N.x,      N.y,    N.z,     (1.0f + lSin) * 0.5f);
		lNormal_XYZ_TexCoord_Y.emplace_back(       N.x,      N.y,    N.z, (1.0f + lNextSin) * 0.5f);

		lIndices.push_back(3 * lSubdivisions + 3 * lDiv + 0);
		lIndices.push_back(3 * lSubdivisions + 3 * lDiv + 1);
		lIndices.push_back(3 * lSubdivisions + 3 * lDiv + 2);
	}

	sUnitConePUN.reset(new Mesh());
	sUnitConePUN->name = "UnitCone";
	sUnitConePUN->set(lIndices, lPosition_XYZ_TexCoord_X, lNormal_XYZ_TexCoord_Y);

	/* Set up the unit cylinder */

	lNextSin = 0.0f;
	lNextCos = 1.0f;
	lPosition_XYZ_TexCoord_X.clear();
	lNormal_XYZ_TexCoord_Y.clear();
	lIndices.clear();
	for (GLsizei lDiv = 0; lDiv < lSubdivisions; ++lDiv) // front
	{
		lCos = lNextCos;
		lSin = lNextSin;
		lNextCos = std::cos(static_cast<float>(lDiv + 1) * lAngle);
		lNextSin = std::sin(static_cast<float>(lDiv + 1) * lAngle);

		lPosition_XYZ_TexCoord_X.emplace_back(    0.0f,     0.0f, 1.0f,                            0.5f);
		lPosition_XYZ_TexCoord_X.emplace_back(    lCos,     lSin, 1.0f,            (1.0f + lCos) * 0.5f);
		lPosition_XYZ_TexCoord_X.emplace_back(lNextCos, lNextSin, 1.0f,        (1.0f + lNextCos) * 0.5f);
		lNormal_XYZ_TexCoord_Y.emplace_back(      0.0f,     0.0f, 1.0f,                            0.5f);
		lNormal_XYZ_TexCoord_Y.emplace_back(      0.0f,     0.0f, 1.0f,     1.0f - (1.0f + lSin) * 0.5f);
		lNormal_XYZ_TexCoord_Y.emplace_back(      0.0f,     0.0f, 1.0f, 1.0f - (1.0f + lNextSin) * 0.5f);

		lIndices.push_back(3 * lDiv + 0);
		lIndices.push_back(3 * lDiv + 1);
		lIndices.push_back(3 * lDiv + 2);
	}
	lNextSin = 0.0f;
	lNextCos = 1.0f;
	for (GLsizei lDiv = 0; lDiv < lSubdivisions; ++lDiv) // back
	{
		lCos = lNextCos;
		lSin = lNextSin;
		lNextCos = std::cos(static_cast<float>(lDiv + 1) * lAngle);
		lNextSin = std::sin(static_cast<float>(lDiv + 1) * lAngle);

		lPosition_XYZ_TexCoord_X.emplace_back(    0.0f,      0.0f, -1.0f,                     0.5f);
		lPosition_XYZ_TexCoord_X.emplace_back(    lCos,     -lSin, -1.0f,     (1.0f + lCos) * 0.5f);
		lPosition_XYZ_TexCoord_X.emplace_back(lNextCos, -lNextSin, -1.0f, (1.0f + lNextCos) * 0.5f);
		lNormal_XYZ_TexCoord_Y.emplace_back(      0.0f,      0.0f, -1.0f,                     0.5f);
		lNormal_XYZ_TexCoord_Y.emplace_back(      0.0f,      0.0f, -1.0f,     (1.0f + lSin) * 0.5f);
		lNormal_XYZ_TexCoord_Y.emplace_back(      0.0f,      0.0f, -1.0f, (1.0f + lNextSin) * 0.5f);

		lIndices.push_back(3 * lSubdivisions + 3 * lDiv + 0);
		lIndices.push_back(3 * lSubdivisions + 3 * lDiv + 1);
		lIndices.push_back(3 * lSubdivisions + 3 * lDiv + 2);
	}
	lNextSin = 0.0f;
	lNextCos = 1.0f;
	for (GLsizei lDiv = 0; lDiv < lSubdivisions; ++lDiv) // hollow tube
	{
		lCos = lNextCos;
		lSin = lNextSin;
		lNextCos = std::cos(static_cast<float>(lDiv + 1) * lAngle);
		lNextSin = std::sin(static_cast<float>(lDiv + 1) * lAngle);

		// The UV coordinates are not yet totally correct.
		lPosition_XYZ_TexCoord_X.emplace_back(    lCos,      lSin,  1.0f,            (1.0f + lCos) * 0.5f);
		lPosition_XYZ_TexCoord_X.emplace_back(    lCos,      lSin, -1.0f,     1.0f - (1.0f + lCos) * 0.5f);
		lPosition_XYZ_TexCoord_X.emplace_back(lNextCos,  lNextSin, -1.0f, 1.0f - (1.0f + lNextCos) * 0.5f);
		lPosition_XYZ_TexCoord_X.emplace_back(lNextCos,  lNextSin,  1.0f,        (1.0f + lNextCos) * 0.5f);
		lNormal_XYZ_TexCoord_Y.emplace_back(      lCos,      lSin,  0.0f,            (1.0f + lSin) * 0.5f);
		lNormal_XYZ_TexCoord_Y.emplace_back(      lCos,      lSin,  0.0f,            (1.0f + lSin) * 0.5f);
		lNormal_XYZ_TexCoord_Y.emplace_back(  lNextCos,  lNextSin,  0.0f,        (1.0f + lNextSin) * 0.5f);
		lNormal_XYZ_TexCoord_Y.emplace_back(  lNextCos,  lNextSin,  0.0f,        (1.0f + lNextSin) * 0.5f);

		lIndices.push_back(6 * lSubdivisions + 4 * lDiv + 0);
		lIndices.push_back(6 * lSubdivisions + 4 * lDiv + 1);
		lIndices.push_back(6 * lSubdivisions + 4 * lDiv + 2);
		lIndices.push_back(6 * lSubdivisions + 4 * lDiv + 2);
		lIndices.push_back(6 * lSubdivisions + 4 * lDiv + 3);
		lIndices.push_back(6 * lSubdivisions + 4 * lDiv + 0);
	}

	sUnitCylinderPUN.reset(new Mesh());
	sUnitCylinderPUN->name = "UnitCylinder";
	sUnitCylinderPUN->set(lIndices, lPosition_XYZ_TexCoord_X, lNormal_XYZ_TexCoord_Y);
}

} // namespace gintonic