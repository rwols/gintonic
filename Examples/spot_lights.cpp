#include "gintonic.hpp"

namespace gt = gintonic;

#define MOVE_SPEED 2.77778f // 10 km/s

template <class FloatType> FloatType get_elapsed_time()
{
	using std::chrono::duration_cast;
	using std::chrono::nanoseconds;
	using gintonic::renderer;
	return static_cast<FloatType>(duration_cast<nanoseconds>(Renderer::elapsed_time()).count()) / FloatType(1e9);
}

template <class FloatType> FloatType get_dt()
{
	using std::chrono::duration_cast;
	using std::chrono::nanoseconds;
	using gintonic::renderer;
	return static_cast<FloatType>(duration_cast<nanoseconds>(Renderer::deltaTime()).count()) / FloatType(1e9);
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <grid-size> <number-of-point-lights>\n";
		std::cerr << "(For example: " << argv[0] << " 1 1)\n";
		return EXIT_FAILURE;
	}

	// Define a camera.
	gt::proj_info projection_component;
	gt::camera cam_component;
	gt::entity cam_entity;
	projection_component.attach(cam_entity);
	cam_component.attach(cam_entity);

	// Position the camera
	cam_entity.set_translation(gt::vec3f(0.0f, 20.0f, 0.0f));
	cam_component.add_mouse(gt::vec2f(0.0f, -static_cast<float>(M_PI) / 2.0f));

	try
	{
		gt::init_all("spot_lights", cam_entity);
		
		gt::Renderer::setFreeformCursor(true);

		gt::font::flyweight font_inconsolata(
			"../examples/Inconsolata-Regular.ttf", 
			20);

		gt::fontstream stream;

		const int numobjects = std::atoi(argv[1]);
		const int numlights = std::atoi(argv[2]);

		if (numobjects < 0)
		{
			throw std::runtime_error("Cannot have a negative number of objects.");
		}
		if (numlights <= 0)
		{
			throw std::runtime_error("Cannot have zero or less point lights.");
		}

		// This data will probably be encapsulated
		// in some sort of "world" or "level" class,
		// along with all the other possible component classes.
		std::vector<gt::entity*> scene_entities;
		std::list<gt::material*> scene_materials;
		std::list<gt::mesh*> scene_meshes;
		std::list<gt::light*> scene_lights;

		scene_meshes.push_back(new gt::unit_sphere_PUN(16));
		auto* unit_sphere_mesh = scene_meshes.back();
		
		gt::SQT shape_transform;

		// Generate the lights
		// First we create a "root light entity", its children
		// will be the actual point lights.
		scene_entities.push_back(new gt::entity);
		auto* root_light = scene_entities.back();
		for (int i = 0; i < numlights; ++i)
		{
			const gt::vec4f attenuation(1.0f, 1.0f, 8.0f, 0.0f);
			const gt::vec4f specularity(0.2f, 0.2f, 0.2f, 4.0f);
			gt::vec4f intensity;
			gt::vec3f color;
			float ceiling;
			color.x = std::rand() % 1000;
			color.y = std::rand() % 1000;
			color.z = std::rand() % 1000;
			color.normalize();
			ceiling = std::max(std::max(color.x, color.y), color.z);
			ceiling = 1.0f - ceiling;
			color.x += ceiling;
			color.y += ceiling;
			color.z += ceiling;
			intensity = {color.x, color.y, color.z, 9.0f};
			intensity.w = 0.0f;

			const auto N = static_cast<float>(numlights);

			shape_transform.scale = 0.1f;
			shape_transform.translation.x = N * std::cos(2.0f * float(i) * static_cast<float>(M_PI) / N);
			shape_transform.translation.y = 0.0f;
			shape_transform.translation.z = N * std::sin(2.0f * float(i) * static_cast<float>(M_PI) / N);
			gt::box3f bbox;

			gt::entity* lightentity = new gt::entity(shape_transform, bbox, nullptr, root_light);
			gt::light* lightcomponent = new gt::spot_light(intensity, attenuation);
			gt::material* matcomponent = new gt::material(intensity, specularity);
			
			lightcomponent->attach(*lightentity);
			matcomponent->attach(*lightentity);
			unit_sphere_mesh->attach(*lightentity);

			scene_lights.push_back(lightcomponent);
			scene_materials.push_back(matcomponent);
			scene_entities.push_back(lightentity);
		}

		shape_transform.scale = 1.0f;
		
		scene_materials.push_back(
			new gt::material(
				gt::vec4f(1.0f, 1.0f, 1.0f,  1.0f), // base diffuse color
				gt::vec4f(1.0f, 1.0f, 1.0f, 20.0f), // base specular color
				"../examples/bricks.jpg",           // diffuse texture
				"../examples/bricks_SPEC.png"       // specular texture
			)
		);

		auto* brick_material = scene_materials.back();

		const std::size_t sphere_index_start = scene_entities.size();

		for (int i = -numobjects; i <= numobjects; ++i)
		{
			for (int j  = -numobjects; j <= numobjects; ++j)
			{
				// Draw a grid of (2*numobjects + 1)^2 rotating cubes/spheres :-)

				shape_transform.translation.x = 2.0f * static_cast<float>(i);
				shape_transform.translation.y = 0.0f;
				shape_transform.translation.z = 2.0f * static_cast<float>(j);

				gt::Renderer::set_model_matrix(shape_transform);

				gt::entity* e = new gt::entity(shape_transform, gt::box3f());
				unit_sphere_mesh->attach(*e);
				brick_material->attach(*e);
				scene_entities.push_back(e);
			}
		}

		float curtime = 0.0f, dt;
		float attenuation_factor = 8.0f;
		float light_intensity_value = 9.0f;
		float light_elevation = 4.0f;
		float light_power_coeff = 4.0f;
		float spot_light_orientation = -1.0f;
		bool move_objects = true;
		bool show_gbuffer = false;
		int attenuation_type = 2; // 0 is constant, 1 is linear, 2 is quadratic
		gt::vec3f rotation_axis;
		gt::vec2f mousedelta;

		gt::Renderer::show();
		
		while (!gt::Renderer::shouldClose())
		{
			dt = get_dt<float>();
			if (move_objects) curtime += dt;
			
			if (gt::Renderer::keyTogglePress(SDL_SCANCODE_Q))
			{
				gt::Renderer::close();
			}
			if (gt::Renderer::key(SDL_SCANCODE_W))
			{
				cam_entity.move_forward(MOVE_SPEED * dt);
			}
			if (gt::Renderer::key(SDL_SCANCODE_A))
			{
				cam_entity.move_left(MOVE_SPEED * dt);
			}
			if (gt::Renderer::key(SDL_SCANCODE_S))
			{
				cam_entity.move_backward(MOVE_SPEED * dt);
			}
			if (gt::Renderer::key(SDL_SCANCODE_D))
			{
				cam_entity.move_right(MOVE_SPEED * dt);
			}
			if (gt::Renderer::key(SDL_SCANCODE_SPACE))
			{
				cam_entity.move_up(MOVE_SPEED * dt);
			}
			if (gt::Renderer::keyTogglePress(SDL_SCANCODE_B))
			{
				move_objects = !move_objects;
			}
			if (gt::Renderer::keyTogglePress(SDL_SCANCODE_G))
			{
				show_gbuffer = !show_gbuffer;
			}
			if (gt::Renderer::key(SDL_SCANCODE_EQUALS))
			{
				attenuation_factor += dt;
			}
			else if (gt::Renderer::key(SDL_SCANCODE_MINUS))
			{
				attenuation_factor -= dt;
			}
			if (gt::Renderer::keyTogglePress(SDL_SCANCODE_1))
			{
				attenuation_type = 0;
			}
			else if (gt::Renderer::keyTogglePress(SDL_SCANCODE_2))
			{
				attenuation_type = 1;
			}
			else if (gt::Renderer::keyTogglePress(SDL_SCANCODE_3))
			{
				attenuation_type = 2;
			}
			if (gt::Renderer::key(SDL_SCANCODE_UP))
			{
				light_intensity_value += dt;
			}
			else if (gt::Renderer::key(SDL_SCANCODE_DOWN))
			{
				light_intensity_value -= dt;
			}
			if (gt::Renderer::key(SDL_SCANCODE_I))
			{
				light_elevation += dt;
			}
			else if (gt::Renderer::key(SDL_SCANCODE_K))
			{
				light_elevation -= dt;
			}
			if (gt::Renderer::key(SDL_SCANCODE_O))
			{
				light_power_coeff += dt;
			}
			else if (gt::Renderer::key(SDL_SCANCODE_L))
			{
				light_power_coeff -= dt;
			}
			if (gt::Renderer::keyTogglePress(SDL_SCANCODE_T))
			{
				spot_light_orientation *= -1.0f;
			}

			auto mousedelta = gt::Renderer::mouse_delta();
			mousedelta = -gt::deg2rad(mousedelta) / 10.0f;
			cam_component.add_mouse(mousedelta);

			shape_transform.rotation = gt::quatf::axis_angle(gt::vec3f(0.0f, 1.0f, 0.0f), curtime);
			shape_transform.rotation *= gt::quatf::axis_angle(gt::vec3f(0.0f, 0.0f, 1.0f), curtime / 2.0f);
			shape_transform.translation.x = 0.0f;
			shape_transform.translation.y = light_elevation + std::sin(curtime * 4.0f);
			shape_transform.translation.z = 0.0f;

			root_light->set_local_transform(shape_transform);

			shape_transform.rotation = gt::quatf(1.0f, 0.0f, 0.0f, 0.0f);
			for (auto i = sphere_index_start; i < scene_entities.size(); ++i)
			{
				scene_entities[i]->set_translation_y(std::sin(static_cast<float>(i) + curtime));
			}
			
			gt::Renderer::begin_geometry_pass();

			for (const auto* m : scene_meshes)
			{
				for (const auto* e : *m)
				{
					gt::Renderer::set_model_matrix(e->global_transform());
					e->material_component()->bind();
					e->mesh_component()->draw();
				}
			}

			if (show_gbuffer)
			{
				stream.open(font_inconsolata);
				gt::Renderer::blit_drawbuffers_to_screen(stream);
				stream.close();
			}
			else
			{
				gt::Renderer::begin_light_pass();

				gt::Renderer::ambient_light_pass();

				for (auto* l : scene_lights)
				{
					gt::vec4f attenuation;
					switch (attenuation_type)
					{
						case 0:  attenuation = gt::vec4f(attenuation_factor, 1.0f, 1.0f, 0.0f); break;
						case 1:  attenuation = gt::vec4f(1.0f, attenuation_factor, 1.0f, 0.0f); break;
						case 2:  attenuation = gt::vec4f(1.0f, 1.0f, attenuation_factor, 0.0f); break;
						default: attenuation = gt::vec4f(1.0f, 1.0f, attenuation_factor, 0.0f); break;
					}

					reinterpret_cast<gt::point_light*>(l)->set_attenuation(attenuation);
					reinterpret_cast<gt::point_light*>(l)->set_brightness(light_intensity_value);

					for (const auto* e : *l)
					{
						e->light_component()->shine(*e);
					}
				}

				gt::Renderer::get_text_shader()->activate();
				gt::Renderer::get_text_shader()->set_color(gt::vec3f(1.0f, 1.0f, 1.0f));
				glDisable(GL_CULL_FACE);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				
				stream.open(font_inconsolata);
				
				stream << "Move around with WASD.\n"
					<< "Look around with the mouse.\n"
					<< "Go up by holding the spacebar.\n"
					<< "Press [Q] to quit.\n"
					<< "Press [B] to start/stop the simulation.\n"
					<< "Press [G] to view the contents of the geometry buffers.\n"
					<< "Press [T] to toggle the orientation of the spot lights.\n"
					<< "[-]/[+] to increase/decrease attenuation.\n"
					<< "[1], [2] or [3] to select attenuation type.\n"
					<< "[UP]/[DOWN] to increase/decrease intensity.\n"
					<< "[I]/[K] to change the elevation of the lights.\n"
					<< "[O]/[L] to decrease/increase the power coefficient.\n"
					<< "FPS: " << 1.0f / dt << '\n'
					<< "Number of objects: " << 2 * (2 * numobjects + 1) * (2 * numobjects + 1) << '\n'
					<< "Number of lights: " << numlights << '\n'
					<< "Attenuation factor: " << attenuation_factor << '\n'
					<< "Attenuation type: ";
				switch (attenuation_type)
				{
					case 0: stream << "constant\n"; break;
					case 1: stream << "linear\n"; break;
					case 2: stream << "quadratic\n"; break;
					default: stream << "unknown!\n";	
				}
				stream << "Intensity: " << light_intensity_value << '\n'
					<< "Power coefficient: " << light_power_coeff << '\n'
					<< "Light elevation: " << light_elevation << '\n';
				stream.close();
				
				glEnable(GL_CULL_FACE);
			}

			gt::Renderer::update();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}