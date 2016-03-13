#include "gintonic.hpp"

#include <iomanip>

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

	// Create entity and camera.
	auto lCamera = std::make_shared<Camera>();
	lCamera->setName("DefaultCamera");
	lCamera->setNearPlane(0.01f);
	lCamera->setFarPlane(100.0f);
	lCamera->setProjectionType(Camera::kPerspectiveProjection);
	auto lCameraEntity = std::make_shared<entity>();
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

	auto lCubeEntity = std::make_shared<entity>();
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

namespace gt = gintonic;

template <class FloatType> FloatType get_elapsed_time()
{
	using std::chrono::duration_cast;
	using std::chrono::nanoseconds;
	using gintonic::renderer;
	return static_cast<FloatType>(duration_cast<nanoseconds>(renderer::elapsed_time()).count()) / FloatType(1e9);
}

template <class FloatType> FloatType get_dt()
{
	using std::chrono::duration_cast;
	using std::chrono::nanoseconds;
	using gintonic::renderer;
	return static_cast<FloatType>(duration_cast<nanoseconds>(renderer::delta_time()).count()) / FloatType(1e9);
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

	// // Define a camera.
	// gt::proj_info projection_component;
	// gt::camera cam_component;
	// gt::entity cam_entity;
	// projection_component.attach(cam_entity);
	// cam_component.attach(cam_entity);

	// // Position the camera
	// cam_entity.set_translation(gt::vec3f(0.0f, 0.0f, 4.0f));
	// cam_component.add_mouse(gt::vec2f(0.0f, 0.0f));

	// try
	// {
	// 	gt::init_all("textured_cube_with_mod_color", cam_entity);
	// 	gt::renderer::set_freeform_cursor(true);
	// 	gt::font::flyweight font_inconsolata("../examples/Inconsolata-Regular.ttf", 20);
	// 	gt::fontstream stream;
	// 	gt::unit_cube_PUN the_shape;
	// 	gt::material the_material(gt::vec4f(1.0f, 1.0f, 1.0f, 0.0f), gt::vec4f(0.0f, 0.0f, 0.0f, 1.0f), "../examples/bricks.jpg");
	// 	gt::renderer::show();
	// 	float curtime, dt;
	// 	while (!gt::renderer::should_close())
	// 	{
	// 		curtime = get_elapsed_time<float>();
	// 		dt = get_dt<float>();
	// 		if (gintonic::renderer::key_toggle_press(SDL_SCANCODE_Q))
	// 		{
	// 			gintonic::renderer::close();
	// 		}
	// 		if (gintonic::renderer::key(SDL_SCANCODE_W))
	// 		{
	// 			cam_entity.move_forward(dt);
	// 		}
	// 		if (gintonic::renderer::key(SDL_SCANCODE_A))
	// 		{
	// 			cam_entity.move_left(dt);
	// 		}
	// 		if (gintonic::renderer::key(SDL_SCANCODE_S))
	// 		{
	// 			cam_entity.move_backward(dt);
	// 		}
	// 		if (gintonic::renderer::key(SDL_SCANCODE_D))
	// 		{
	// 			cam_entity.move_right(dt);
	// 		}
	// 		if (gintonic::renderer::key(SDL_SCANCODE_SPACE))
	// 		{
	// 			cam_entity.move_up(dt);
	// 		}
	// 		auto mousedelta = gt::renderer::mouse_delta();
	// 		mousedelta = -gt::deg2rad(mousedelta) / 10.0f;
	// 		cam_component.add_mouse(mousedelta);
	// 		gt::renderer::begin_geometry_pass();
	// 		const auto yaxis = (1.0f + std::cos(curtime)) / 2.0f;
	// 		const auto zaxis = (1.0f + std::sin(curtime)) / 2.0f;
	// 		const auto rotation_axis = gt::vec3f(0.0f, yaxis, zaxis).normalize();
	// 		gt::renderer::set_model_matrix(rotation_axis, -curtime / 4.0f);
	// 		the_material.diffuse_color.x = yaxis;
	// 		the_material.diffuse_color.y = zaxis;
	// 		the_material.bind();
	// 		the_shape.draw();
	// 		gt::renderer::begin_light_pass();
	// 		gt::renderer::ambient_light_pass();
	// 		gt::renderer::get_text_shader()->activate();
	// 		gt::renderer::get_text_shader()->set_color(gt::vec3f(1.0f, 1.0f, 1.0f));
	// 		glDisable(GL_CULL_FACE);
	// 		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// 		stream.open(font_inconsolata);
	// 		stream << "Move around with WASD.\n"
	// 			<< "Look around with the mouse.\n"
	// 			<< "Go up by holding the spacebar.\n"
	// 			<< "Press Q to quit.\n"
	// 			<< "Elapsed time: " << std::fixed << std::setprecision(1) << curtime << " seconds\n"
	// 			<< "Frames per second: " << std::fixed << std::setprecision(1) << 1.0f / dt << '\n'
	// 			<< "Camera position: " << cam_entity.local_transform().translation << '\n'
	// 			<< "Camera forward:  " << cam_entity.local_transform().rotation.forward_direction() << '\n'
	// 			<< "Camera up:       " << cam_entity.local_transform().rotation.up_direction() << '\n'
	// 			<< "Camera right:    " << cam_entity.local_transform().rotation.right_direction() << std::endl;
	// 		stream.close();
	// 		glEnable(GL_CULL_FACE);
	// 		gt::renderer::update();
	// 	}
	// }
	// catch (const std::exception& e)
	// {
	// 	std::cerr << e.what() << '\n';
	// 	return EXIT_FAILURE;
	// }
	// return EXIT_SUCCESS;
}