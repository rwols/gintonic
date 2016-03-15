#include "gintonic.hpp"

#define APPNAME "BasicShapes"

struct GameState
{
	std::vector<std::shared_ptr<gintonic::Entity>> mEntities;
	std::vector<std::shared_ptr<gintonic::Material>> mMaterials;
	std::vector<std::shared_ptr<gintonic::Mesh>> mMeshes;
	std::vector<std::shared_ptr<gintonic::Light>> mLights;
	std::vector<std::shared_ptr<gintonic::Camera>> mCameras;
	std::shared_ptr<gintonic::Entity> mRootEntity;

	float mMoveSpeed = 1.0f;

	void clear() noexcept
	{
		mEntities.clear();
		mMaterials.clear();
		mMeshes.clear();
		mLights.clear();
		mCameras.clear();
		mRootEntity = nullptr;
	}
};

GameState gState;


bool initialize(const int argc, char** argv)
{
	using namespace gintonic;

	// Create Entity and camera.
	auto lCamera = std::make_shared<Camera>();
	lCamera->name = "DefaultCamera";
	lCamera->setNearPlane(0.01f);
	lCamera->setFarPlane(100.0f);
	lCamera->setProjectionType(Camera::kPerspectiveProjection);
	auto lCameraEntity = std::make_shared<Entity>();
	lCameraEntity->name = "DefaultCamera";
	lCameraEntity->setRotation(quatf(1.0f, 0.0f, 0.0f, 0.0f));
	lCameraEntity->setScale(vec3f(1.0f, 1.0f, 1.0f));
	lCameraEntity->setTranslation(vec3f(0.0f, 0.0f, 4.0f));
	lCameraEntity->camera = lCamera;

	gState.mEntities.push_back(lCameraEntity);
	gState.mCameras.push_back(lCamera);

	try
	{
		initializeEverything(APPNAME, lCameraEntity);
	}
	catch (const exception& e)
	{
		std::cerr << e.what() << '\n';
		return false;
	}

	// Put a directional light in the scene
	// so that we see something interesting.
	// The directional light shines downwards.
	auto lLightEntity = std::make_shared<gintonic::Entity>();
	auto lLight = std::shared_ptr<Light>(new DirectionalLight());
	lLight->intensity = 1.0f;
	lLightEntity->name = "DefaultDirectionalLight";
	lLightEntity->setLocalTransform
	(
		SQT
		(
			vec3f(1.0f, 1.0f, 1.0f), 
			quatf::axis_angle
			(
				vec3f(1.0f, 0.0f, 0.0f), 
				-M_PI / 2.0f + 1.0f
			), 
			vec3f(0.0f, 0.0f, 0.0f)
		)
	);
	lLight->name = "DefaultDirectionalLight";
	lLightEntity->light = lLight;
	
	gState.mEntities.push_back(lLightEntity);
	gState.mLights.push_back(lLight);

	// Initialize the basic shapes (from the renderer).

	auto lMaterial = std::make_shared<Material>();
	lMaterial->diffuseColor = vec4f(0.8f, 0.8f, 0.8f, 0.5f);
	lMaterial->specularColor = vec4f(0.2f, 0.2f, 0.2f, 1.0f);
	lMaterial->diffuseTexture = std::make_shared<Texture2D>("../../Examples/RuralBrickWall.jpg");

	auto lCubeEntity = std::make_shared<Entity>();
	lCubeEntity->mesh = Renderer::getUnitCube();
	lCubeEntity->material = lMaterial;

	auto lSphereEntity = std::make_shared<Entity>();
	lSphereEntity->mesh = Renderer::getUnitSphere();
	lSphereEntity->material = lMaterial;

	gState.mMaterials.push_back(lMaterial);
	gState.mMeshes.push_back(Renderer::getUnitCube());
	gState.mMeshes.push_back(Renderer::getUnitSphere());

	gState.mEntities.push_back(lCubeEntity);
	gState.mEntities.push_back(lSphereEntity);

	lCubeEntity->setScale(vec3f(1.0f, 1.0f, 1.0f));
	lCubeEntity->setRotation(quatf(1.0f, 0.0f, 0.0f, 0.0f));
	lCubeEntity->setTranslation(vec3f(-3.0f, 0.0f, 0.0f));

	lSphereEntity->setScale(vec3f(1.0f, 1.0f, 1.0f));
	lSphereEntity->setRotation(quatf(1.0f, 0.0f, 0.0f, 0.0f));
	lSphereEntity->setTranslation(vec3f(3.0f, 0.0f, 0.0f));

	auto lRootEntity = std::make_shared<Entity>();
	lRootEntity->setScale(vec3f(1.0f, 1.0f, 1.0f));
	lRootEntity->setRotation(quatf(1.0f, 0.0f, 0.0f, 0.0f));
	lRootEntity->setTranslation(vec3f(0.0f, 0.0f, 0.0f));

	lRootEntity->addChild(lCubeEntity);
	lRootEntity->addChild(lSphereEntity);

	gState.mRootEntity = lRootEntity;
	gState.mEntities.push_back(lRootEntity);

	return true;
}


int main(int argc, char* argv[])
{
	using namespace gintonic;

	if (!initialize(argc, argv))
	{
		return EXIT_FAILURE;
	}

	std::cout << Renderer::name() << '\n';
	std::cout << Renderer::version() << '\n';

	Renderer::setFreeformCursor(true);
	Renderer::show();

	double lDeltaTime, lElapsedTime;

	while (Renderer::shouldClose() == false)
	{

		Renderer::getElapsedAndDeltaTime(lElapsedTime, lDeltaTime);
		auto lCameraEntity = Renderer::getCameraEntity();

		if (Renderer::key(SDL_SCANCODE_Q))
		{
			Renderer::close();
		}
		if (Renderer::key(SDL_SCANCODE_W))
		{
			lCameraEntity->moveForward(gState.mMoveSpeed * lDeltaTime);
		}
		if (Renderer::key(SDL_SCANCODE_A))
		{
			lCameraEntity->moveLeft(gState.mMoveSpeed * lDeltaTime);
		}
		if (Renderer::key(SDL_SCANCODE_S))
		{
			lCameraEntity->moveBackward(gState.mMoveSpeed * lDeltaTime);
		}
		if (Renderer::key(SDL_SCANCODE_D))
		{
			lCameraEntity->moveRight(gState.mMoveSpeed * lDeltaTime);
		}
		if (Renderer::key(SDL_SCANCODE_SPACE))
		{
			lCameraEntity->moveUp(gState.mMoveSpeed * lDeltaTime);
		}
		if (Renderer::key(SDL_SCANCODE_C))
		{
			lCameraEntity->moveDown(gState.mMoveSpeed * lDeltaTime);
		}
		if (Renderer::keyTogglePress(SDL_SCANCODE_T))
		{
			Renderer::setWireframeMode(!Renderer::getWireframeMode());
		}

		const auto lMouseDelta = -deg2rad(Renderer::mouseDelta()) / 10.0f;
		lCameraEntity->camera->addMouse(lMouseDelta);
		lCameraEntity->setRotation(quatf::mouse(lCameraEntity->camera->angles()));

		gState.mRootEntity->setRotation(quatf::axis_angle(vec3f(0.0f, 1.0f, 0.0f), lElapsedTime / 10.0f));

		Renderer::submitEntities(gState.mEntities.begin(), gState.mEntities.end());

		Renderer::update();
	}
	return EXIT_SUCCESS;
}