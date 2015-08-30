#include "gintonic.hpp"
#include "vec2f.hpp"

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
	// Define a camera.
	gt::proj_info projection;
	gt::entity cam_entity;
	cam_entity.proj_info_component = &projection;
	gt::camera cam_component(cam_entity);

	// Position the camera
	cam_entity.set_translation(gt::vec3f(-2.78f, 0.83f, 1.17f));
	cam_component.add_mouse(gt::vec2f(-gt::deg2rad(60.0f), -gt::deg2rad(9.0f)));
	
	try
	{
		gt::init_all("basic_shapes", cam_entity);
		
		gt::renderer::set_freeform_cursor(true);
		
		gt::font::flyweight font_inconsolata("../examples/Inconsolata-Regular.ttf", 20);
		gt::fontstream stream;
		gt::directional_light the_light(gt::vec4f(1.0f, 1.0f, 1.0f, 1.0f));
		gt::unit_cube_PUNTB the_cube;
		gt::unit_sphere_PUN the_sphere(16);
		gt::unit_cone_PN the_cone(32);
		gt::material brick_material(
			gt::vec4f(0.8f, 0.8f, 0.8f,  0.9f), // diffuse color. 4th component is diffuse contribution
			gt::vec4f(0.3f, 0.3f, 0.3f,  4.0f), // specular color. 4th component is shininess
			"../examples/bricks_COLOR.png",     // diffuse texture.
			"../examples/bricks_SPEC.png"       // specular texture.
		);

		gt::material brick_with_normal_material(
			gt::vec4f(1.0f, 1.0f, 1.0f,  0.9f), // diffuse color. 4th component is diffuse contribution
			gt::vec4f(0.3f, 0.3f, 0.3f, 20.0f), // specular color. 4th component is shininess
			"../examples/bricks_COLOR.png",     // diffuse texture.
			"../examples/bricks_SPEC.png",      // specular texture.
			"../examples/bricks_NRM.png"
		);

		gt::material flat_material(
			gt::vec4f(0.4f, 0.4f, 0.4f, 0.8f), // diffuse color
			gt::vec4f(0.3f, 0.3f, 0.3f, 20.0f) // specular color
		);
		gt::SQT transform;
		gt::box3f bbox;
		gt::entity cube_entity(transform, bbox);
		transform.translation.x += 3.0f;
		gt::entity sphere_entity(transform, bbox);
		transform.translation.x -= 6.0f;
		gt::entity cone_entity(transform, bbox);
		transform.translation.x += 3.0f;
		transform.rotation = gt::quatf::axis_angle(gt::vec3f(1.0f, 0.0f, 0.0f), static_cast<float>(-M_PI) / 2.0f);
		gt::entity light_entity(transform, bbox);

		// "Wire up" the entities with their components
		cube_entity.mesh_component = &the_cube;
		cube_entity.material_component = &brick_with_normal_material;
		sphere_entity.mesh_component = &the_sphere;
		sphere_entity.material_component = &brick_material;
		cone_entity.mesh_component = &the_cone;
		cone_entity.material_component = &flat_material;
		light_entity.light_component = &the_light;
		
		// std::unique_ptr<gt::light> the_light(new gt::directional_light(gt::vec4f(1.0f, 1.0f, 1.0f, 1.0f)));
		gt::SQT the_light_transform;
		the_light_transform.rotation = gt::quatf::axis_angle(gt::vec3f(1.0f, 0.0f, 0.0f), static_cast<float>(-M_PI) / 2.0f);
		float curtime, dt, yaxis, zaxis, cursin, fieldofview = M_PI / 2.0f, aspectratio = 1.0f;
		gt::vec3f rotation_axis;
		gt::vec2f mousedelta;
		gt::SQT shape_transform;
		shape_transform.scale = 1.0f;
		bool show_gbuffer = false;

		gt::renderer::show();
		
		while (!gt::renderer::should_close())
		{
			curtime = get_elapsed_time<float>();
			dt = get_dt<float>();
			if (gt::renderer::key_toggle_press(SDL_SCANCODE_Q))
			{
				gt::renderer::close();
			}
			if (gt::renderer::key(SDL_SCANCODE_W))
			{
				cam_entity.move_forward(dt);
			}
			if (gt::renderer::key(SDL_SCANCODE_A))
			{
				cam_entity.move_left(dt);
			}
			if (gt::renderer::key(SDL_SCANCODE_S))
			{
				cam_entity.move_backward(dt);
			}
			if (gt::renderer::key(SDL_SCANCODE_D))
			{
				cam_entity.move_right(dt);
			}
			if (gt::renderer::key(SDL_SCANCODE_SPACE))
			{
				cam_entity.move_up(dt);
			}
			if (gt::renderer::key(SDL_SCANCODE_C))
			{
				cam_entity.move_down(dt);
			}
			if (gt::renderer::key_toggle_press(SDL_SCANCODE_G))
			{
				show_gbuffer = !show_gbuffer;
			}

			mousedelta = -gt::deg2rad(gt::renderer::mouse_delta()) / 10.0f;
			cam_component.add_mouse(mousedelta);
			
			gt::renderer::begin_geometry_pass();
			
			yaxis = 1.0f;
			zaxis = 0.0f;
			rotation_axis = gt::vec3f(0.0f, yaxis, zaxis).normalize();
			shape_transform.rotation = gt::quatf::axis_angle(
				rotation_axis,
				-curtime / 8.0f);
			shape_transform.translation = { 0.0f, 0.0f, 0.0f };

			cube_entity.set_local_transform(shape_transform);
			gt::renderer::set_model_matrix(cube_entity);
			cube_entity.material_component->bind();
			cube_entity.mesh_component->draw();

			shape_transform.translation.x += 3.0f;
			sphere_entity.set_local_transform(shape_transform);
			gt::renderer::set_model_matrix(sphere_entity);
			sphere_entity.material_component->bind();
			sphere_entity.mesh_component->draw();

			shape_transform.translation.x -= 6.0f;
			cone_entity.set_local_transform(shape_transform);
			gt::renderer::set_model_matrix(cone_entity);
			cone_entity.material_component->bind();
			cone_entity.mesh_component->draw();

			shape_transform.translation.x += 3.0f;
			shape_transform.translation.y += 4.0f;

			gt::mat4f frustummatrix;
			frustummatrix.set_inverse_perspective(fieldofview, aspectratio, 1.0f, 2.0f);
			gt::renderer::set_model_matrix(gt::mat4f(shape_transform) * frustummatrix);
			brick_with_normal_material.bind();
			the_cube.draw();

			if (show_gbuffer)
			{
				stream.open(font_inconsolata);
				gt::renderer::blit_drawbuffers_to_screen(stream);
				stream.close();
			}
			else
			{
				gt::renderer::begin_light_pass();

				cursin = std::sin(curtime) / 2.0f;
				shape_transform.rotation = gt::quatf::axis_angle(
					gt::vec3f(1.0f, 0.0f, 0.0f),
					cursin + static_cast<float>(-M_PI) / 2.0f);
				light_entity.set_local_transform(shape_transform);
				light_entity.light_component->shine(light_entity.global_transform());

				gt::renderer::get_text_shader()->activate();
				gt::renderer::get_text_shader()->set_color(gt::vec3f(1.0f, 1.0f, 1.0f));
				glDisable(GL_CULL_FACE);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				
				stream.open(font_inconsolata);
				stream << "Move around with WASD.\n"
					<< "Look around with the mouse.\n"
					<< "Go up by holding the spacebar.\n"
					<< "FOV for the frustum: " << fieldofview << '\n'
					<< "Press G to view the geometry buffer.\n"
					<< "Press Q to quit.\n"
					<< the_light << '\n'
					<< gt::renderer::camera().global_transform().translation << '\n';
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