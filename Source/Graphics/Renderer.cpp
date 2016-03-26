#include "Renderer.hpp"

#include "../Foundation/exception.hpp"
#include "../Math/vec4f.hpp"
#include "../Math/MatrixPipeline.hpp"

#include "GeometryBuffer.hpp"
#include "Mesh.hpp"
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

#define DEPTH_TEXTURE_UNIT 4

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
					entity->light->initializeShadowBuffer(*entity);
				}
				assert(entity->shadowBuffer);
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
vec2f Renderer::sMouseDelta = vec2f(0,0);

Uint8* sKeyPrevState = nullptr;
const Uint8* sKeyState = nullptr;
int sKeyStateCount;

MatrixPipeline sMatrixPipeline;

bool Renderer::s_matrix_P_dirty = true;
bool Renderer::s_matrix_VM_dirty = true;
bool Renderer::s_matrix_PVM_dirty = true;
bool Renderer::s_matrix_N_dirty = true;

mat4f Renderer::s_matrix_P = mat4f(1.0f);
mat4f Renderer::s_matrix_V = mat4f(1.0f);
mat4f Renderer::s_matrix_M = mat4f(1.0f);
mat4f Renderer::s_matrix_VM = mat4f(1.0f);
mat4f Renderer::s_matrix_PVM = mat4f(1.0f);
mat3f Renderer::s_matrix_N = mat3f(1.0f);

std::shared_ptr<Entity> Renderer::sCameraEntity = std::shared_ptr<Entity>(nullptr);
std::shared_ptr<Entity> Renderer::sDebugShadowBufferEntity = std::shared_ptr<Entity>(nullptr);
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

FontStream& Renderer::cerr() { return *sDebugErrorStream; }
FontStream& Renderer::cout() { return *sDebugLogStream;   }

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
		sWidth = width;
		sHeight = height;
	}

	sAspectRatio = (float) sWidth / (float) sHeight;

	if (lWasAlreadyInitialized == false) std::atexit(SDL_Quit);

	Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
	if (sFullscreen) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	sWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, sWidth, sHeight, flags);

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
			throw NoContextAvailableException(3, 3);
		}
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
	
	vsync(true);
	glClearColor(0.03f, 0.03f, 0.03f, 0.0f);
	
	sStartTime = clock_type::now();

	#ifdef BOOST_MSVC
	CoInitialize(nullptr); // initialize COM
	#endif

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
	sDefaultCamera->setWidth(static_cast<float>(sWidth));
	sDefaultCamera->setHeight(static_cast<float>(sHeight));
	sDefaultCamera->setNearPlane(1.0f);
	sDefaultCamera->setFarPlane(50.0f);
	sDefaultCamera->setProjectionType(Camera::kPerspective);
	setCameraEntity(std::move(cameraEntity));

	//
	// Initializer shaders
	//
	init_shaders();

	//
	// Initialize basic shapes
	//

	initializeBasicShapes();

	//
	// Initialize debug variables
	//
	// Determine the point size. We take 1/80 of the width
	// const unsigned int lPointSize = static_cast<unsigned int>((float)sWidth / 80.0f);
	const unsigned int lPointSize = 24;
	sDebugFont = std::make_shared<Font>("Resources/Inconsolata-Regular.ttf", lPointSize);
	sDebugErrorStream = new FontStream();
	sDebugLogStream = new FontStream();

	#ifdef ENABLE_DEBUG_TRACE
	sDebugErrorStream->open(sDebugFont);
	sDebugLogStream->open(sDebugFont);
	#endif
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
		std::cout << sCameraEntity->name << " had no camera component!\n";
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

void Renderer::setWireframeMode(const bool yesOrNo) noexcept
{
	sRenderInWireframeMode = yesOrNo;
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
	// MatrixPipeline lPipeline;
	// lPipeline.setProjectionMatrix(sCameraEntity->camera->projectionMatrix());
	// lPipeline.setViewMatrix(sCameraEntity->getViewMatrix());

	prepareRendering();

	sGeometryBuffer->prepareGeometryPhase();

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
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
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		renderLights();
		
		sGeometryBuffer->finalize(sWidth, sHeight);
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

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	const auto& lTextProgram = FlatTextShaderProgram::get();
	lTextProgram.activate();
	lTextProgram.setColor(vec3f(1.0f, 0.0f, 0.0f));
	sDebugErrorStream->close();
	lTextProgram.setColor(vec3f(0.8f, 0.8f, 0.8f));
	sDebugLogStream->close();

	#endif

	SDL_GL_SwapWindow(sWindow);

	#ifdef ENABLE_DEBUG_TRACE

	sDebugErrorStream->open(sDebugFont);
	sDebugLogStream->open(sDebugFont);

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
	sCameraEntity->updateViewMatrix(s_matrix_V);
	sCameraPosition = vec3f((sCameraEntity->globalTransform() * vec4f(0.0f, 0.0f, 0.0f, 1.0f)).data);
}

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
		s_matrix_PVM = sCameraEntity->camera->projectionMatrix()  * s_matrix_VM;
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

	INIT_SHADER(DepthBufferShaderProgram);
	INIT_SHADER(ShadowShaderProgram);
	INIT_SHADER(MaterialShaderProgram);
	INIT_SHADER(AmbientLightShaderProgram);
	INIT_SHADER(DirectionalLightShaderProgram);
	INIT_SHADER(PointLightShaderProgram);
	INIT_SHADER(SpotLightShaderProgram);
	INIT_SHADER(FlatTextShaderProgram);
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