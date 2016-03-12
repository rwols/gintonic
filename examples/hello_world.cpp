// #include <iostream>
// #include <cstddef>

#include "gintonic.hpp"
#include "FbxImporter.hpp"

struct GameState
{
	std::vector<std::shared_ptr<gintonic::entity>> mEntities;
	std::vector<std::shared_ptr<gintonic::Material>> mMaterials;
	std::vector<std::shared_ptr<gintonic::Mesh>> mMeshes;
	std::vector<std::shared_ptr<gintonic::Light>> mLights;
	std::vector<std::shared_ptr<gintonic::Camera>> mCameras;
	std::shared_ptr<gintonic::entity> mRootEntity;

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

	if (argc <= 1)
	{
		std::cerr << "Supply a filename...\n";
		return false;
	}

	// Create entity and camera.
	auto lCamera = std::make_shared<Camera>();
	lCamera->setName("Global", "DefaultCamera");
	lCamera->setNearPlane(0.01f);
	lCamera->setFarPlane(100.0f);
	lCamera->setProjectionType(Camera::kPerspectiveProjection);
	auto lCameraEntity = Object<entity>::create();
	lCameraEntity->setName("Global", "DefaultCamera");
	lCameraEntity->set_rotation(quatf(1.0f, 0.0f, 0.0f, 0.0f));
	lCameraEntity->set_scale(vec3f(1.0f, 1.0f, 1.0f));
	lCameraEntity->camera = lCamera;

	try
	{
		initializeEverything("Hello World!", lCameraEntity);

		const boost::filesystem::path lFilename(argv[1]);
		if (!boost::filesystem::is_regular_file(lFilename))
		{
			std::cerr << lFilename << " is not a regular file!\n";
			return false;
		}
		const auto lFilenameAsString = lFilename.string();

		gintonic::FbxImporter lImporter;
		auto lResult = lImporter.loadFromFile(lFilenameAsString.c_str());
		gState.mEntities = std::move(lResult.entities);
		gState.mMaterials = std::move(lResult.materials);
		gState.mMeshes = std::move(lResult.meshes);
		gState.mLights = std::move(lResult.lights);
		gState.mCameras = std::move(lResult.cameras);
		gState.mRootEntity = std::move(lResult.rootEntity);

		std::cout << "Number of entities: " << gState.mEntities.size() << '\n';
		std::cout << "Root entity: " << gState.mRootEntity->getName() << '\n';
		std::cout << "Number of materials: " << gState.mMaterials.size() << '\n';
		std::cout << "Number of meshes: " << gState.mMeshes.size() << '\n';
		std::cout << "Number of lights: " << gState.mLights.size() << '\n';
		std::cout << "Number of cameras: " << gState.mCameras.size() << '\n';

		gState.mEntities.push_back(lCameraEntity);
		gState.mCameras.push_back(lCamera);
	}
	catch (const exception& e)
	{
		std::cerr << e.what() << '\n';
		return false;
	}

	if (gState.mLights.empty())
	{
		// Put a directional light in the scene
		// so that we see something interesting.
		// The directional light shines downwards.
		auto lLightEntity = std::make_shared<gintonic::entity>();
		auto lLight = std::shared_ptr<Light>(new DirectionalLight());
		lLight->intensity = 1.0f;
		lLightEntity->setName("Global", "DefaultDirectionalLight");
		lLightEntity->set_local_transform
		(
			SQT
			(
				vec3f(1.0f, 1.0f, 1.0f), 
				quatf::axis_angle
				(
					vec3f(1.0f, 0.0f, 0.0f), 
					-M_PI / 2.0f
				), 
				vec3f(0.0f, 0.0f, 0.0f)
			)
		);
		lLight->setName("Global", "DefaultDirectionalLight");
		lLightEntity->light = lLight;
		gState.mEntities.push_back(lLightEntity);
		gState.mLights.push_back(lLight);
	}

	// Determine a "nice" camera offset so that we are not "inside" geometry,
	// and at the same time determine a "nice" movement speed so that we don't
	// feel like snails or airplanes.
	float lCameraOffsetZDistance(0.0f);

	for (auto lMesh : gState.mMeshes)
	{
		const auto lBBox = lMesh->getLocalBoundingBox();
		auto lBBoxArea = 
			(lBBox.max_corner.x - lBBox.min_corner.x) * 
			(lBBox.max_corner.y - lBBox.min_corner.y) * 
			(lBBox.max_corner.z - lBBox.min_corner.z);

		// Take the cube root. Crude approximation of a single side length.
		lBBoxArea = std::pow(lBBoxArea, 1.0f / 3.0f);

		gState.mMoveSpeed += lBBoxArea / static_cast<float>(gState.mMeshes.size());

		if (lBBox.max_corner.z > lCameraOffsetZDistance)
		{
			lCameraOffsetZDistance = lBBox.max_corner.z;
		}

	}

	lCameraEntity->set_translation_z(lCameraOffsetZDistance + 4.0f);

	return true;
}

int main(int argc, char** argv)
{
	using namespace gintonic;

	if (!initialize(argc, argv))
	{
		return EXIT_FAILURE;
	}

	std::cout << renderer::name() << '\n';
	std::cout << renderer::version() << '\n';

	renderer::set_freeform_cursor(true);
	renderer::show();

	double lDeltaTime, lElapsedTime;

	while (renderer::should_close() == false)
	{
		renderer::getElapsedAndDeltaTime(lElapsedTime, lDeltaTime);
		auto lCameraEntity = renderer::getCameraEntity();

		if (renderer::key(SDL_SCANCODE_Q))
		{
			renderer::close();
		}
		if (renderer::key(SDL_SCANCODE_W))
		{
			lCameraEntity->move_forward(gState.mMoveSpeed * lDeltaTime);
		}
		if (renderer::key(SDL_SCANCODE_A))
		{
			lCameraEntity->move_left(gState.mMoveSpeed * lDeltaTime);
		}
		if (renderer::key(SDL_SCANCODE_S))
		{
			lCameraEntity->move_backward(gState.mMoveSpeed * lDeltaTime);
		}
		if (renderer::key(SDL_SCANCODE_D))
		{
			lCameraEntity->move_right(gState.mMoveSpeed * lDeltaTime);
		}
		if (renderer::key(SDL_SCANCODE_SPACE))
		{
			lCameraEntity->move_up(gState.mMoveSpeed * lDeltaTime);
		}
		if (renderer::key(SDL_SCANCODE_C))
		{
			lCameraEntity->move_down(gState.mMoveSpeed * lDeltaTime);
		}
		if (renderer::key_toggle_press(SDL_SCANCODE_T))
		{
			renderer::setWireframeMode(!renderer::getWireframeMode());
		}

		const auto lMouseDelta = -deg2rad(renderer::mouse_delta()) / 10.0f;
		lCameraEntity->camera->addMouse(lMouseDelta);
		lCameraEntity->set_rotation(quatf::mouse(lCameraEntity->camera->angles()));

		gState.mRootEntity->set_rotation(quatf::axis_angle(vec3f(0.0f, 1.0f, 0.0f), lElapsedTime / 10.0f));

		renderer::submitEntities(gState.mEntities.begin(), gState.mEntities.end());

		renderer::update();
	}

	return EXIT_SUCCESS;
}