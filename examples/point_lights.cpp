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
		gt::init_all("point_lights");
		
		gt::renderer::set_freeform_cursor(true);

		gt::font::flyweight font_inconsolata(
			"../examples/Inconsolata-Regular.ttf", 
			20);

		gt::fontstream stream;
		
		gt::opengl::unit_cube_PUN the_shape;
		
		std::vector<std::unique_ptr<gt::light>> lights;
		std::vector<gt::sqt_transformf> light_transforms(3);
		std::vector<std::unique_ptr<gt::material>> light_materials;

		{
			gt::vec3f attenuation(0.1f, 0.5f, 1.0f);

			lights.emplace_back(new gt::point_light(gt::vec3f(1.0f, 0.1f, 0.1f), attenuation));
			lights.emplace_back(new gt::point_light(gt::vec3f(0.1f, 1.0f, 0.1f), attenuation));
			lights.emplace_back(new gt::point_light(gt::vec3f(0.1f, 0.1f, 1.0f), attenuation));

			light_materials.emplace_back(new gt::material_c(gt::vec4f(1.0f, 0.0f, 0.0f, 0.0f)));
			light_materials.emplace_back(new gt::material_c(gt::vec4f(0.0f, 1.0f, 0.0f, 0.0f)));
			light_materials.emplace_back(new gt::material_c(gt::vec4f(0.0f, 0.0f, 1.0f, 0.0f)));
		}
		light_transforms[0].scale = 0.1f;
		light_transforms[1].scale = 0.1f;
		light_transforms[2].scale = 0.1f;
		
		light_transforms[0].translation = gt::vec3f( 0.0f, 2.0f, 0.0f);
		light_transforms[1].translation = gt::vec3f( 1.0f, 1.2f, 0.0f);
		light_transforms[2].translation = gt::vec3f(-1.0f, 1.2f, 0.0f);

		std::unique_ptr<gt::material> the_material(
			new gt::material_cd(
				gt::vec4f(1.0f,1.0f,1.0f,0.9f), 
				"../examples/bricks.jpg"));

		gt::renderer::show();

		float curtime, dt;
		float current_cos, current_sin;
		float yaxis, zaxis;
		float speed = 1.0f;
		float addspeed = 0.0f;
		gt::vec3f rotation_axis;
		gt::vec2f mousedelta;
		
		while (!gt::renderer::should_close())
		{
			curtime = get_elapsed_time<float>();
			dt = get_dt<float>();
			
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
			if (gintonic::renderer::key(SDL_SCANCODE_EQUALS))
			{
				addspeed = 0.01f;
			}
			else if (gintonic::renderer::key(SDL_SCANCODE_MINUS))
			{
				addspeed = -0.01f;
			}
			else
			{
				addspeed = 0.0f;
			}

			mousedelta = gintonic::renderer::mouse_delta();
			mousedelta[0] = -gintonic::deg_to_rad(mousedelta[0]) / 4.0f;
			mousedelta[1] = -gintonic::deg_to_rad(mousedelta[1]) / 4.0f;
			
			gt::get_default_camera().add_horizontal_and_vertical_angles(mousedelta[0], mousedelta[1]);
			
			gt::renderer::begin_geometry_pass();
			
			yaxis = (1.0f + current_cos) / 2.0f;
			
			zaxis = (1.0f + current_sin) / 2.0f;
			
			rotation_axis = gt::normalize(gt::vec3f(0.0f, yaxis, zaxis));
			
			gt::renderer::set_model_matrix(-curtime / 4.0f, rotation_axis);
			the_material->bind();
			the_shape.draw();

			for (std::size_t i = 0; i < lights.size(); ++i)
			{
				const auto numlights = static_cast<float>(lights.size());
				const auto radius = 3.0f;
				const auto elevation = 0.0f;
				light_transforms[i].translation[0] = radius * std::cos(speed * curtime + 2.0f * float(i) * M_PI / numlights);
				light_transforms[i].translation[1] = elevation;
				light_transforms[i].translation[2] = radius * std::sin(speed * curtime + 2.0f * float(i) * M_PI / numlights);
				gt::renderer::set_model_matrix(light_transforms[i].get_matrix());
				light_materials[i]->bind();
				gt::renderer::get_unit_sphere_P().draw();
			}
			gt::renderer::begin_light_pass();

			gt::renderer::null_light_pass();

			for (std::size_t i = 0; i < lights.size(); ++i)
			{
				lights[i]->shine(light_transforms[i]);
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
				<< "Pressing -/+ will decrease/increase the speed of the balls.\n"
				<< "Press Q to quit.\n"
				<< "Camera position: " << std::fixed << std::setprecision(1) 
				<< gt::renderer::camera().position << '\n';
				// << "Light intensity: " << red_light->intensity << '\n'
				// << "Light position:  " << red_light_transform.translation 
				// << '\n';
			stream.close();
			
			glEnable(GL_CULL_FACE);

			speed += addspeed;
			
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