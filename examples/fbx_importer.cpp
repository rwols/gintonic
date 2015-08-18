#include "gintonic.hpp"
// #define FBXSDK_NEW_API
#include <fbxsdk.h>

namespace gt = gintonic;

#define MOVE_SPEED 2.77778f // 10 km/s

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
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <fbx-file>\n";
		return EXIT_FAILURE;
	}
	{
		const boost::filesystem::path filename(argv[1]);
		const auto ext = filename.extension();
		if (ext != ".fbx" && ext != ".FBX")
		{
			std::cout << "You need to supply an FBX file.\n";
			return EXIT_FAILURE;
		}
	}
	try
	{
		// #ifdef BOOST_MSVC
		// 	boost::filesystem::current_path(gt::get_executable_path());
		// #else
			boost::filesystem::current_path(gt::get_executable_path() / "..");
		// #endif
		gt::init_all("fbx_importer");

		bool view_gbuffers = false;
		
		gt::renderer::set_freeform_cursor(true);

		gt::font::flyweight font_inconsolata(
			"../examples/Inconsolata-Regular.ttf", 
			20);

		gt::fontstream stream;

		boost::filesystem::current_path(gt::get_executable_path());

		std::unique_ptr<gt::actor> the_actor;
		{
			auto manager = FbxManager::Create();
			auto scene = FbxScene::Create(manager, "");
			auto importer = FbxImporter::Create(manager, "");
			const boost::filesystem::path filename(argv[1]);
			if (!boost::filesystem::is_regular_file(filename))
			{
				std::cerr << filename << " is not a regular file.\n";
				importer->Destroy();
				scene->Destroy();
				manager->Destroy();
				return EXIT_FAILURE;
			}
			else if (filename.extension() != ".fbx" && filename.extension() != ".FBX")
			{
				std::cerr << filename << " is not an FBX file.\n";
				importer->Destroy();
				scene->Destroy();
				manager->Destroy();
				return EXIT_FAILURE;
			}
			const auto status = importer->Initialize(argv[1], -1, manager->GetIOSettings());
			if (!status)
			{
				std::cerr << importer->GetFileName().Buffer() << '\n';
				std::cerr << importer->GetStatus().GetErrorString() << '\n';
				importer->Destroy();
				scene->Destroy();
				manager->Destroy();
				return EXIT_FAILURE;
			}
			importer->Import(scene);
			importer->Destroy();
			{
				FbxAxisSystem axis_system(FbxAxisSystem::EUpVector::eYAxis,
    				FbxAxisSystem::EFrontVector::eParityEven,
    				FbxAxisSystem::eLeftHanded);
				axis_system.ConvertScene(scene);
				FbxGeometryConverter converter(manager);
				converter.Triangulate(scene, true);
			}
			the_actor.reset(new gt::static_model_actor(scene));
			scene->Destroy();
			manager->Destroy();
		}

		// Test instanced rendering
		const int instancebound = 30;
		for (int i = -instancebound; i <= instancebound; ++i)
		{
			for (int j = -instancebound; j <= instancebound; ++j)
			{
				gt::sqt_transformf t;
				t.scale = 1.0f;
				t.translation = { static_cast<float>(i), 0.0f, static_cast<float>(j) };
				the_actor->transforms.push_back(t);
			}
		}

		// the_actor->transforms.emplace_back(gt::sqt_transformf());
		
		std::vector<std::unique_ptr<gt::light>> lights;
		std::vector<gt::sqt_transformf> light_transforms(4);
		std::vector<std::unique_ptr<gt::material>> light_materials;

		{
			gt::vec4f attenuation(1.0f, 0.0f, 1.0f, 1.0f);

			lights.emplace_back(new gt::point_light(gt::vec4f(1.0f, 0.0f, 0.0f, 1.0f), attenuation));
			lights.emplace_back(new gt::point_light(gt::vec4f(0.0f, 1.0f, 0.0f, 1.0f), attenuation));
			lights.emplace_back(new gt::point_light(gt::vec4f(0.0f, 0.0f, 1.0f, 1.0f), attenuation));
			lights.emplace_back(new gt::directional_light(gt::vec4f(0.8f, 0.8f, 0.8f, 1.0f)));

			light_materials.emplace_back(new gt::material(gt::vec4f(1.0f, 0.0f, 0.0f, 0.0f)));
			light_materials.emplace_back(new gt::material(gt::vec4f(0.0f, 1.0f, 0.0f, 0.0f)));
			light_materials.emplace_back(new gt::material(gt::vec4f(0.0f, 0.0f, 1.0f, 0.0f)));
		}
		
		light_transforms[0].scale = 0.1f;
		light_transforms[1].scale = 0.1f;
		light_transforms[2].scale = 0.1f;
		
		light_transforms[0].translation = gt::vec3f( 0.0f, 2.0f, 0.0f);
		light_transforms[1].translation = gt::vec3f( 1.0f, 1.2f, 0.0f);
		light_transforms[2].translation = gt::vec3f(-1.0f, 1.2f, 0.0f);
		light_transforms[3].rotation = gt::quatf::from_angle_axis(
			gt::deg_to_rad(10) 
			+ static_cast<float>(-M_PI) / 2.0f, 
			gt::vec3f(1.0f, 0.0f, 0.0f));

		gt::opengl::unit_cube_PUNTB cube;
		gt::sqt_transformf cube_transform;
		cube_transform.scale = 0.5f;
		gt::material cube_material
		(
			{1.0f, 1.0f, 1.0f, 1.0f},
			{1.0f, 1.0f, 1.0f, 2.0f},
			"../../examples/RuralBrickWall.jpg",
			"../../examples/RuralBrickWall_SPEC.png",
			"../../examples/RuralBrickWall_NRM.png"
		);

		gt::renderer::show();

		float curtime, dt;
		float speed = 1.0f;
		float addspeed = 0.0f;
		// gt::vec3f rotation_axis;
		gt::vec2f mousedelta;
		
		while (!gt::renderer::should_close())
		{
			curtime = get_elapsed_time<float>();
			dt = get_dt<float>();
			
			if (gt::renderer::key_toggle_press(SDL_SCANCODE_G))
			{
				view_gbuffers = !view_gbuffers;
			}
			if (gt::renderer::key_toggle_press(SDL_SCANCODE_Q))
			{
				gt::renderer::close();
			}
			if (gt::renderer::key(SDL_SCANCODE_W))
			{
				gt::get_default_camera().move_forward(MOVE_SPEED * dt);
			}
			if (gt::renderer::key(SDL_SCANCODE_A))
			{
				gt::get_default_camera().move_left(MOVE_SPEED * dt);
			}
			if (gt::renderer::key(SDL_SCANCODE_S))
			{
				gt::get_default_camera().move_backward(MOVE_SPEED * dt);
			}
			if (gt::renderer::key(SDL_SCANCODE_D))
			{
				gt::get_default_camera().move_right(MOVE_SPEED * dt);
			}
			if (gt::renderer::key(SDL_SCANCODE_SPACE))
			{
				gt::get_default_camera().move_up(MOVE_SPEED * dt);
			}
			if (gt::renderer::key(SDL_SCANCODE_EQUALS))
			{
				addspeed = 0.01f;
			}
			else if (gt::renderer::key(SDL_SCANCODE_MINUS))
			{
				addspeed = -0.01f;
			}
			else
			{
				addspeed = 0.0f;
			}

			mousedelta = gt::renderer::mouse_delta();
			mousedelta[0] = -gt::deg_to_rad(mousedelta[0]) / 4.0f;
			mousedelta[1] = -gt::deg_to_rad(mousedelta[1]) / 4.0f;
			
			gt::get_default_camera().add_horizontal_and_vertical_angles(mousedelta[0], mousedelta[1]);
			
			gt::renderer::begin_geometry_pass();

			the_actor->draw_geometry();
			
			for (std::size_t i = 0; i < light_materials.size(); ++i)
			{
				const auto numlights = static_cast<float>(light_materials.size());
				const auto radius = 3.0f;
				const auto elevation = 0.0f;
				
				light_transforms[i].translation[0] = radius 
					* std::cos(speed * curtime + 2.0f * float(i) * static_cast<float>(M_PI) / numlights);
				
				light_transforms[i].translation[1] = elevation;
				
				light_transforms[i].translation[2] = radius 
					* std::sin(speed * curtime + 2.0f * float(i) * static_cast<float>(M_PI) / numlights);
				
				gt::renderer::set_model_matrix(light_transforms[i].get_matrix());
				
				light_materials[i]->bind();
				
				gt::renderer::get_unit_sphere_P().draw();
			}

			if (view_gbuffers)
			{
				stream.open(font_inconsolata);
				gt::renderer::blit_drawbuffers_to_screen(stream);
				stream.close();
			}
			else
			{
				gt::renderer::begin_light_pass();

				gt::renderer::null_light_pass();

				the_actor->draw_lights();

				for (std::size_t i = 0; i < lights.size(); ++i)
				{
					lights[i]->shine(light_transforms[i]);
				}
				
				#ifdef ENABLE_DEBUG_STREAM
				gt::renderer::cerr() << "Move around with WASD.\n"
					<< "Look around with the mouse.\n"
					<< "Go up by holding the spacebar.\n"
					<< "Pressing -/+ will decrease/increase the speed of the balls.\n"
					<< "Press Q to quit.\n"
					<< "Camera position: " << std::fixed << std::setprecision(1) 
					<< gt::renderer::camera().position << '\n';
				#endif
			}

			gt::renderer::update();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}