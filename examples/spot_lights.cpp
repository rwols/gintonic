#include "gintonic.hpp"

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
		std::cerr << "Usage: " << argv[0] << " <grid-size> <number-of-point-lights>\n";
		std::cerr << "(For example: " << argv[0] << " 1 1)\n";
		return EXIT_FAILURE;
	}
	try
	{
		// #ifdef BOOST_MSVC
		// 	boost::filesystem::current_path(gt::get_executable_path());
		// #else
			boost::filesystem::current_path(gt::get_executable_path() / "..");
		// #endif
		gt::init_all("point_lights");
		
		gt::renderer::set_freeform_cursor(true);

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
		
		gt::opengl::unit_sphere_PUN a_sphere(16);
		
		std::vector<gt::spot_light> lights;
		std::vector<gt::sqt_transformf> light_transforms;
		std::vector<gt::material> light_materials;
		gt::sqt_transformf shape_transform;

		// Generate the lights
		for (int i = 0; i < numlights; ++i)
		{
			const gt::vec4f attenuation(1.0f, 1.0f, 8.0f, 4.0f);
			const gt::vec4f specularity(0.2f, 0.2f, 0.2f, 4.0f);
			gt::vec4f intensity;
			gt::vec3f color;
			float ceiling;
			color[0] = std::rand() % 1000;
			color[1] = std::rand() % 1000;
			color[2] = std::rand() % 1000;
			color = normalize(color);
			ceiling = std::max(std::max(color[0], color[1]), color[2]);
			ceiling = 1.0f - ceiling;
			color[0] += ceiling;
			color[1] += ceiling;
			color[2] += ceiling;
			intensity = {color[0], color[1], color[2], 9.0f};
			lights.emplace_back(intensity, attenuation);
			intensity[3] = 0.0f;
			light_materials.emplace_back(intensity, specularity);
			light_transforms.emplace_back(gt::sqt_transformf());
			light_transforms.back().scale = 0.1f;
			light_transforms.back().rotation = gt::quatf::from_angle_axis(
				-static_cast<float>(M_PI) / 2.0, 
				gt::vec3f(1.0f, 0.0f, 0.0f));
		}

		shape_transform.scale = 1.0f;
		
		gt::material the_material(
			gt::vec4f(1.0f, 1.0f, 1.0f,  1.0f), // base diffuse color
			gt::vec4f(1.0f, 1.0f, 1.0f, 20.0f), // base specular color
			"../examples/bricks_COLOR.png",     // diffuse texture
			"../examples/bricks_SPEC.png");     // specular texture

		// Orient the camera
		gt::get_default_camera().position = {0.0f, 2.0f, 4.0f};
		gt::get_default_camera().add_horizontal_and_vertical_angles(0.0f, 0.0f);

		float curtime = 0.0f, dt;
		float current_cos, current_sin;
		float yaxis, zaxis;
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

		gt::renderer::show();
		
		while (!gt::renderer::should_close())
		{
			dt = get_dt<float>();
			if (move_objects) curtime += dt;
			
			current_cos = std::cos(curtime);
			current_sin = std::sin(curtime);
			
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
			if (gt::renderer::key_toggle_press(SDL_SCANCODE_B))
			{
				move_objects = !move_objects;
			}
			if (gt::renderer::key_toggle_press(SDL_SCANCODE_G))
			{
				show_gbuffer = !show_gbuffer;
			}
			if (gt::renderer::key(SDL_SCANCODE_EQUALS))
			{
				attenuation_factor += dt;
			}
			else if (gt::renderer::key(SDL_SCANCODE_MINUS))
			{
				attenuation_factor -= dt;
			}
			if (gt::renderer::key_toggle_press(SDL_SCANCODE_1))
			{
				attenuation_type = 0;
			}
			else if (gt::renderer::key_toggle_press(SDL_SCANCODE_2))
			{
				attenuation_type = 1;
			}
			else if (gt::renderer::key_toggle_press(SDL_SCANCODE_3))
			{
				attenuation_type = 2;
			}
			if (gt::renderer::key(SDL_SCANCODE_UP))
			{
				light_intensity_value += dt;
			}
			else if (gt::renderer::key(SDL_SCANCODE_DOWN))
			{
				light_intensity_value -= dt;
			}
			if (gt::renderer::key(SDL_SCANCODE_I))
			{
				light_elevation += dt;
			}
			else if (gt::renderer::key(SDL_SCANCODE_K))
			{
				light_elevation -= dt;
			}
			if (gt::renderer::key(SDL_SCANCODE_O))
			{
				light_power_coeff += dt;
			}
			else if (gt::renderer::key(SDL_SCANCODE_L))
			{
				light_power_coeff -= dt;
			}
			if (gt::renderer::key_toggle_press(SDL_SCANCODE_T))
			{
				spot_light_orientation *= -1.0f;
			}

			mousedelta = gt::renderer::mouse_delta();
			mousedelta[0] = -gt::deg_to_rad(mousedelta[0]) / 4.0f;
			mousedelta[1] = -gt::deg_to_rad(mousedelta[1]) / 4.0f;
			
			gt::get_default_camera().add_horizontal_and_vertical_angles(mousedelta[0], mousedelta[1]);
			
			gt::renderer::begin_geometry_pass();
			
			yaxis = (1.0f + current_cos) / 2.0f;
			zaxis = (1.0f + current_sin) / 2.0f;
			rotation_axis = gt::normalize(gt::vec3f(0.0f, yaxis, zaxis));
			shape_transform.rotation = gt::quatf::from_angle_axis(-curtime / 4.0f, rotation_axis);

			for (int i = -numobjects; i <= numobjects; ++i)
			{
				for (int j  = -numobjects; j <= numobjects; ++j)
				{
					// Draw a grid of (2*numobjects + 1)^2 rotating cubes/spheres :-)
					const float yval = std::cos(static_cast<float>(j) + curtime) * std::sin(static_cast<float>(i) + curtime);

					shape_transform.translation = {2.0f * i, yval, 2.0f * j};
					gt::renderer::set_model_matrix(shape_transform.get_matrix());
					the_material.bind();
					a_sphere.draw();

					shape_transform.translation = {2.0f * i, 10.0f + yval, 2.0f * j};
					gt::renderer::set_model_matrix(shape_transform.get_matrix());
					the_material.bind();
					a_sphere.draw();
				}
			}

			for (std::size_t i = 0; i < lights.size(); ++i)
			{

				const auto numlights = static_cast<float>(lights.size());
				const auto radius = static_cast<float>(numlights);
				
				light_transforms[i].translation[0] = radius 
					* std::cos(curtime + 2.0f * float(i) * static_cast<float>(M_PI) / numlights);
				
				light_transforms[i].translation[1] = light_elevation;
				
				light_transforms[i].translation[2] = radius 
					* std::sin(curtime + 2.0f * float(i) * static_cast<float>(M_PI) / numlights);

				light_transforms[i].rotation = gt::quatf::from_angle_axis(
					spot_light_orientation * static_cast<float>(M_PI) / 2.0f,
					gt::vec3f(1.0f, 0.0f, 0.0f));
				
				gt::renderer::set_model_matrix(light_transforms[i].get_matrix());
				
				light_materials[i].bind();
				
				gt::renderer::get_unit_sphere_P().draw();
			}

			if (show_gbuffer)
			{
				stream.open(font_inconsolata);
				gt::renderer::blit_drawbuffers_to_screen(stream);
				stream.close();
			}
			else
			{
				gt::renderer::begin_light_pass();

				gt::renderer::null_light_pass();

				for (std::size_t i = 0; i < lights.size(); ++i)
				{
					gt::vec4f attenuation;
					switch (attenuation_type)
					{
						case 0:  attenuation = gt::vec4f(attenuation_factor, 1.0f, 1.0f, light_power_coeff); break;
						case 1:  attenuation = gt::vec4f(1.0f, attenuation_factor, 1.0f, light_power_coeff); break;
						case 2:  attenuation = gt::vec4f(1.0f, 1.0f, attenuation_factor, light_power_coeff); break;
						default: attenuation = gt::vec4f(1.0f, 1.0f, attenuation_factor, light_power_coeff); break;
					}
					lights[i].set_attenuation(attenuation);
					lights[i].set_brightness(light_intensity_value);
					lights[i].shine(light_transforms[i]);
				}

				gt::renderer::get_text_shader()->activate();
				
				gt::renderer::get_text_shader()->set_color(gt::vec3f(1.0f, 1.0f, 1.0f));
				
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