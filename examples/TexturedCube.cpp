#include "gintonic.hpp"

#include <iomanip>

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
	lCamera->setName("DefaultCamera");
	lCamera->setNearPlane(0.01f);
	lCamera->setFarPlane(100.0f);
	lCamera->setProjectionType(Camera::kPerspectiveProjection);
	auto lCameraEntity = std::make_shared<Entity>();
	lCameraEntity->setName("DefaultCamera");
	lCameraEntity->set_rotation(quatf(1.0f, 0.0f, 0.0f, 0.0f));
	lCameraEntity->set_scale(vec3f(1.0f, 1.0f, 1.0f));
	lCameraEntity->set_translation(vec3f(0.0f, 0.0f, 4.0f));
	lCameraEntity->camera = lCamera;

	gState.mEntities.push_back(lCameraEntity);
	gState.mCameras.push_back(lCamera);

	try
	{
		initializeEverything("Texture Cube With Mod Color", lCameraEntity);
	}
	catch (const exception& e)
	{
		std::cerr << e.what() << '\n';
		return false;
	}

	auto lCubeEntity = std::make_shared<Entity>();
	std::shared_ptr<Texture2D> lDiffuseTexture;
	try
	{
		lDiffuseTexture = std::make_shared<Texture2D>("../examples/RuralBrickWall.jpg");
		std::cerr << "Loaded texture: " << lDiffuseTexture->getName() << '\n';
	}
	catch (const exception& e)
	{
		std::cerr << e.what() << '\n';
		return false;
	}
	auto lMaterial = std::make_shared<Material>(
		vec4f(1.0f, 1.0f, 1.0f, 0.0f), 
		vec4f(1.0f, 1.0f, 1.0f, 8.0f), 
		lDiffuseTexture, 
		nullptr, 
		nullptr);
	lMaterial->setName("Rural Brick Wall");
	lCubeEntity->setName("Cube");
	lCubeEntity->material = lMaterial;
	lCubeEntity->mesh = renderer::getUnitCube();

	gState.mEntities.push_back(lCubeEntity);
	gState.mMaterials.push_back(lMaterial);

	return true;
}


int main(int argc, char* argv[])
{
	if (!initialize(argc, argv))
	{
		return EXIT_FAILURE;
	}

	using namespace gintonic;

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

		const auto lYAxis = (1.0f + std::cos(lElapsedTime)) / 2.0f;
		const auto lZAxis = (1.0f + std::sin(lElapsedTime)) / 2.0f;
		const auto lRotationAxis = vec3f(0.0f, lYAxis, lZAxis).normalize();
		const auto lQuaternion = quatf::axis_angle(lRotationAxis, -lElapsedTime / 4.0f);

		for (auto lEntity : gState.mEntities)
		{
			if (lEntity->material)
			{
				lEntity->material->diffuseColor.x = lYAxis;
				lEntity->material->diffuseColor.y = lZAxis;
			}
			if (lEntity->mesh)
			{
				lEntity->set_rotation(lQuaternion);
			}
		}

		renderer::submitEntities(gState.mEntities.begin(), gState.mEntities.end());

		renderer::update();
	}

	return EXIT_SUCCESS;
}