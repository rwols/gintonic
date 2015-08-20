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
		
		gt::opengl::unit_cube_PUNTB the_shape;
		
		std::vector<gt::point_light> lights;
		std::vector<gt::SQT> light_transforms(1);
		std::vector<gt::material> light_materials;

		{
			const gt::vec4f attenuation(1.0f, 1.0f, 1.0f, 0.0f);
			const gt::vec4f specularity(1.0f, 1.0f, 1.0f, 1.0f);

			lights.emplace_back(gt::vec4f(1.0f, 1.0f, 1.0f, 4.0f), attenuation);

			light_materials.emplace_back(gt::vec4f(1.0f, 1.0f, 1.0f, 0.0f), specularity);
		}
		
		light_transforms[0].scale = 0.1f;
		
		gt::material the_material(
			gt::vec4f(0.7f, 0.7f, 0.7f, 0.9f),  // base diffuse color
			gt::vec4f(0.3f, 0.3f, 0.3f, 1.0f)); // base specular color
		the_material.set_normal_texture("../examples/normal_test.jpg");

		gt::renderer::show();

		float curtime = 0.0f, dt;
		float current_cos, current_sin;
		bool move_objects = true;
		bool show_gbuffer = false;
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
			if (gintonic::renderer::key_toggle_press(SDL_SCANCODE_G))
			{
				show_gbuffer = !show_gbuffer;
			}
			if (gintonic::renderer::key(SDL_SCANCODE_EQUALS))
			{
				for (auto& l : lights)
				{
					l.set_brightness(l.brightness() + dt);
				}
			}
			else if (gintonic::renderer::key(SDL_SCANCODE_MINUS))
			{
				for (auto& l : lights)
				{
					l.set_brightness(l.brightness() - dt);
				}
			}

			auto mousedelta = gt::renderer::mouse_delta();
			mousedelta = -gt::deg2rad(mousedelta) / 2.0f;
			gt::get_default_camera().add_horizontal_and_vertical_angles(mousedelta.x, mousedelta.y);
			
			gt::renderer::begin_geometry_pass();
			
			gt::renderer::set_model_matrix(gt::vec3f(0.0f, 1.0f, 0.0f), curtime / 8.0f);
			the_material.bind();
			the_shape.draw();

			for (std::size_t i = 0; i < lights.size(); ++i)
			{
				const auto numlights = static_cast<float>(lights.size());
				const auto radius = 3.0f;
				const auto elevation = 0.0f;
				
				light_transforms[i].translation.x = radius 
					* std::cos(curtime / 3.0f + 2.0f * float(i) * static_cast<float>(M_PI) / numlights);
				
				light_transforms[i].translation.y = elevation;
				
				light_transforms[i].translation.z = radius 
					* std::sin(curtime / 3.0f + 2.0f * float(i) * static_cast<float>(M_PI) / numlights);
				
				gt::renderer::set_model_matrix(light_transforms[i]);
				
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
					<< "Holding +/- will decrease/increase the light intensity.\n"
					<< "Press Q to quit.\n"
					<< "Press B to start/stop the simulation.\n"
					<< "Press G to view the contents of the geometry buffers.\n"
					<< "Camera position: " << std::fixed << std::setprecision(1) 
					<< gt::renderer::camera().position << '\n'
					<< "FPS: " << 1.0f / dt << '\n';
				for (std::size_t i = 0; i < lights.size(); ++i)
				{
					stream << "Light " << (i+1) << ' ' << std::fixed 
						<< std::setprecision(1) << lights[i] << '\n';
				}
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