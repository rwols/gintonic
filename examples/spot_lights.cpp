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
	try
	{
		#ifdef BOOST_MSVC
			boost::filesystem::current_path(gt::get_executable_path());
		#else
			boost::filesystem::current_path(gt::get_executable_path() / "..");
		#endif
		gt::init_all("spot_lights");
		
		gt::renderer::set_freeform_cursor(true);

		gt::font::flyweight font_inconsolata(
			"../examples/Inconsolata-Regular.ttf", 
			20);

		gt::fontstream stream;
		
		gt::opengl::unit_cube_PUN the_shape;

		gt::sqt_transformf the_shape_transform;
		
		std::vector<gt::point_light> lights(3);
		std::vector<gt::sqt_transformf> light_transforms(3);
		std::vector<std::unique_ptr<gt::material>> light_materials;

		{
			gt::vec4f attenuation(0.0f, 0.0f, 1.0f, 20.0f);

			lights[0].intensity = gt::vec4f(1.0f, 0.0f, 0.0f, 6.0f);
			lights[1].intensity = gt::vec4f(0.0f, 1.0f, 0.0f, 6.0f);
			lights[2].intensity = gt::vec4f(0.0f, 0.0f, 1.0f, 6.0f);

			lights[0].set_attenuation(attenuation);
			lights[1].set_attenuation(attenuation);
			lights[2].set_attenuation(attenuation);

			light_materials.emplace_back(new gt::material_c(gt::vec4f(1.0f, 0.0f, 0.0f, 0.0f)));
			light_materials.emplace_back(new gt::material_c(gt::vec4f(0.0f, 1.0f, 0.0f, 0.0f)));
			light_materials.emplace_back(new gt::material_c(gt::vec4f(0.0f, 0.0f, 1.0f, 0.0f)));
		}

		the_shape_transform.scale = 6.0f;
		the_shape_transform.rotation = gt::quatf::from_angle_axis(0.0f, gt::vec3f(1.0f, 0.0f, 0.0f));
		the_shape_transform.translation = gt::vec3f(0.0f, -11.0f, 0.0f);
		
		light_transforms[0].scale = 0.1f;
		light_transforms[1].scale = 0.1f;
		light_transforms[2].scale = 0.1f;

		const auto light_radius = 3.0f;
		const auto light_elevation = 0.0f;

		light_transforms[0].rotation = light_transforms[1].rotation = light_transforms[2].rotation
			= gt::quatf::from_angle_axis(static_cast<float>(-M_PI) / 2.0f, gt::vec3f(1.0f, 0.0f, 0.0f));

		std::unique_ptr<gt::material> the_material(
			new gt::material_cd(
				gt::vec4f(1.0f,1.0f,1.0f,0.99f), 
				"../examples/bricks.jpg"));

		gt::renderer::show();

		float curtime = 0.0f, dt;
		float current_cos, current_sin;
		float yaxis, zaxis;
		bool move_objects = false;
		float toggle_spot_lights = 1.0f;
		gt::vec3f rotation_axis;
		gt::vec2f mousedelta;
		
		while (!gt::renderer::should_close())
		{
			dt = get_dt<float>();
			if (move_objects) curtime += dt;
			
			current_cos = std::cos(curtime);
			current_sin = std::sin(curtime);
			
			if (gintonic::renderer::key_toggle_press(SDL_SCANCODE_Q))
			{
				gintonic::renderer::close();
			}
			if (gintonic::renderer::key(SDL_SCANCODE_W))
			{
				gt::get_default_camera().move_forward(MOVE_SPEED * dt);
			}
			if (gintonic::renderer::key(SDL_SCANCODE_A))
			{
				gt::get_default_camera().move_left(MOVE_SPEED * dt);
			}
			if (gintonic::renderer::key(SDL_SCANCODE_S))
			{
				gt::get_default_camera().move_backward(MOVE_SPEED * dt);
			}
			if (gintonic::renderer::key(SDL_SCANCODE_D))
			{
				gt::get_default_camera().move_right(MOVE_SPEED * dt);
			}
			if (gintonic::renderer::key(SDL_SCANCODE_SPACE))
			{
				gt::get_default_camera().move_up(MOVE_SPEED * dt);
			}
			if (gintonic::renderer::key_toggle_press(SDL_SCANCODE_B))
			{
				move_objects = !move_objects;
			}
			if (gintonic::renderer::key_toggle_press(SDL_SCANCODE_T))
			{
				light_transforms[0].rotation 
					= light_transforms[1].rotation 
					= light_transforms[2].rotation
					= gt::quatf::from_angle_axis(
						toggle_spot_lights * static_cast<float>(M_PI) / 2.0f, 
						gt::vec3f(1.0f, 0.0f, 0.0f)
				);
				toggle_spot_lights *= -1.0f;
			}
			if (gintonic::renderer::key(SDL_SCANCODE_EQUALS))
			{
				for (auto& l : lights)
				{
					l.set_attenuation(l.attenuation() + gt::vec4f(0.0f, 0.0f, 0.0f, dt));
				}
			}
			else if (gintonic::renderer::key(SDL_SCANCODE_MINUS))
			{
				for (auto& l : lights)
				{
					l.set_attenuation(l.attenuation() + gt::vec4f(0.0f, 0.0f, 0.0f, -dt));
				}
			}
			if (gintonic::renderer::key(SDL_SCANCODE_UP))
			{
				for (auto& l : lights)
				{
					l.intensity[3] += dt;
				}
			}
			else if (gintonic::renderer::key(SDL_SCANCODE_DOWN))
			{
				for (auto& l : lights)
				{
					l.intensity[3] -= dt;
				}
			}

			mousedelta = gintonic::renderer::mouse_delta();
			mousedelta[0] = -gintonic::deg_to_rad(mousedelta[0]) / 4.0f;
			mousedelta[1] = -gintonic::deg_to_rad(mousedelta[1]) / 4.0f;
			
			gt::get_default_camera().add_horizontal_and_vertical_angles(mousedelta[0], mousedelta[1]);
			
			gt::renderer::begin_geometry_pass();
			
			yaxis = (1.0f + current_cos) / 2.0f;
			
			zaxis = (1.0f + current_sin) / 2.0f;
			
			rotation_axis = gt::normalize(gt::vec3f(0.0f, yaxis, zaxis));
			
			gt::renderer::set_model_matrix(the_shape_transform.get_matrix());
			the_material->bind();
			the_shape.draw();

			// flip the cube in the XZ plane
			the_shape_transform.translation[1] = -the_shape_transform.translation[1];
			gt::renderer::set_model_matrix(the_shape_transform.get_matrix());
			the_material->bind();
			the_shape.draw();

			for (std::size_t i = 0; i < lights.size(); ++i)
			{
				const auto numlights = static_cast<float>(lights.size());
				
				light_transforms[i].translation[0] = light_radius 
					* std::cos(curtime + 2.0f * float(i) * static_cast<float>(M_PI) / numlights);
				
				light_transforms[i].translation[1] = light_elevation;
				
				light_transforms[i].translation[2] = light_radius 
					* std::sin(curtime + 2.0f * float(i) * static_cast<float>(M_PI) / numlights);
				
				gt::renderer::set_model_matrix(light_transforms[i].get_matrix());
				
				light_materials[i]->bind();
				
				gt::renderer::get_unit_sphere_P().draw();
			}
			gt::renderer::begin_light_pass();

			gt::renderer::null_light_pass();

			for (std::size_t i = 0; i < lights.size(); ++i)
			{
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
				<< "Holding +/- will increase/decrease the spot lights' power coefficient.\n"
				<< "Holding Up/Down arrows will increase/decrease the spot lights' intensity coefficient.\n"
				<< "Press Q to quit.\n"
				<< "Press B to start/stop the simulation.\n"
				<< "Press T to toggle the spot lights' direction.\n"
				<< "Camera position: " << std::fixed << std::setprecision(1) 
				<< gt::renderer::camera().position << '\n'
				<< "FPS: " << 1.0f / dt << '\n';
				for (std::size_t i = 0; i < lights.size(); ++i)
				{
					stream << "Light " << (i+1) << " intensity:     " 
						<< std::fixed << std::setprecision(1) << lights[i].intensity << '\n';
						
					stream << "Light " << (i+1) << " attenuation:   " 
						<< std::fixed << std::setprecision(1) << lights[i].attenuation() << '\n';

					stream << "Light " << (i+1) << " cutoff radius: " 
						<< std::fixed << std::setprecision(1) << lights[i].cutoff_point() << '\n';
				}
			stream.close();
			
			glEnable(GL_CULL_FACE);
			
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