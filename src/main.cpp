#include <iostream>
#include <cstddef>
#include <cassert>

#include "renderer.hpp"
#include "basic_shapes.hpp"
#include "textures.hpp"
#include "fonts.hpp"

#define APP_NAME "Hello world!"

#ifndef DEBUG_PRINT
#define DEBUG_PRINT_START std::cerr << __FILE__ << '(' << __LINE__ << ')'
#define DEBUG_PRINT DEBUG_PRINT_START << '\n'
#define PRINT_VAR(var) DEBUG_PRINT_START << ": " << #var << " == " << var << '\n'
#endif

void about_to_close_handler()
{
	std::cerr << "Bye cruel world!\n";
}

void window_unfocus_handler()
{
	gintonic::renderer::set_freeform_cursor(false);
}

void window_focus_handler()
{
	gintonic::renderer::set_freeform_cursor(true);
}

inline float get_elapsed_seconds()
{
	using gintonic::renderer;
	using std::chrono::duration_cast;
	using std::chrono::microseconds;
	return static_cast<float>(duration_cast<microseconds>(renderer::elapsed_time()).count()) / 1e6;
}

template <typename FloatType> inline FloatType get_dt() BOOST_NOEXCEPT_OR_NOTHROW
{
	using gintonic::renderer;
	using std::chrono::duration_cast;
	using std::chrono::microseconds;
	return static_cast<FloatType>(duration_cast<microseconds>(renderer::delta_time()).count()) / FloatType(1e6);
}

int main(int argc, char* argv[])
{
	std::cerr << "sizeof(gintonic::font) == " << sizeof(gintonic::font) << '\n';
	try
	{
		using gintonic::renderer;
		using gintonic::opengl::texture2d;
		using gintonic::opengl::shader;
		
		gintonic::camera_transform<float> default_camera;
		
		//
		// set up projection matrix parameters
		//
		default_camera.projection_type = gintonic::camera_transform<float>::e_projection_type::projective;
		default_camera.field_of_view = gintonic::deg_to_rad(60.0f);
		default_camera.near_plane = 0.1f;
		default_camera.far_plane = 100.0f;
		
		//
		// set up view matrix parameters
		//
		default_camera.position = {0, 0, 4};
		default_camera.add_horizontal_and_vertical_angles(static_cast<float>(M_PI), 0.0f);

		//
		// initialize renderer
		//
		{
			std::string title(PROJECT_NAME);
			title.append(" version ");
			title.append(PROJECT_VERSION);
			gintonic::renderer::init(title.c_str(), default_camera, true, 800, 640);
			gintonic::opengl::texture2d::init();
			gintonic::font::init();
		}

		std::cout << renderer::name() << '\n';
		std::cout << renderer::version() << '\n';

		renderer::about_to_close.connect(about_to_close_handler);

		renderer::focus_context();
		renderer::set_freeform_cursor(true);

		//
		// initialize things to render
		//
		gintonic::opengl::unit_cube_PUN the_shape;
		auto program = shader::flyweight("../s/point_diffuse.vs", "", "../s/point_diffuse.fs");
		auto textshader = shader::flyweight("../s/flat_text_uniform_color.vs", "", "../s/flat_text_uniform_color.fs");
		auto tex = texture2d::flyweight("../resources/sample.jpg");
		auto font_scriptin48 = gintonic::font::flyweight("../resources/SCRIPTIN.ttf", 48);
		auto font_inconsolata20 = gintonic::font::flyweight("../resources/Inconsolata-Regular.ttf", 20);
		gintonic::fontstream stream;

		const auto loc_diffuse = program.get().get_uniform_location("material.diffuse_color");
		const auto loc_diffuse_factor = program.get().get_uniform_location("material.diffuse_factor");
		const auto loc_light_position = program.get().get_uniform_location("light.position");
		const auto loc_light_intensity = program.get().get_uniform_location("light.intensity");
		const auto loc_light_attenuation = program.get().get_uniform_location("light.attenuation");
		const auto loc_matrix_pvm = program.get().get_uniform_location("matrix_PVM");
		// const auto loc_color = program.get().get_uniform_location("color");
		const auto loc_matrix_vm = program.get().get_uniform_location("matrix_VM");
		const auto loc_matrix_n = program.get().get_uniform_location("matrix_N");

		const auto loc_text_color = textshader.get().get_uniform_location("color");
		const auto loc_text_tex = textshader.get().get_uniform_location("tex");

		//
		// start main loop
		//
		glClearColor(0,0,0,1);
		while (!renderer::should_close())
		{
			//
			// process events
			//
			if (renderer::key_toggle_press(SDL_SCANCODE_Q))
			{
				renderer::close();
			}
			if (renderer::key(SDL_SCANCODE_W))
			{
				default_camera.move_forward(get_dt<float>());
			}
			if (renderer::key(SDL_SCANCODE_A))
			{
				default_camera.move_left(get_dt<float>());
			}
			if (renderer::key(SDL_SCANCODE_S))
			{
				default_camera.move_backward(get_dt<float>());
			}
			if (renderer::key(SDL_SCANCODE_D))
			{
				default_camera.move_right(get_dt<float>());
			}
			if (renderer::key(SDL_SCANCODE_SPACE))
			{
				default_camera.move_up(get_dt<float>());
			}
			auto mousedelta = renderer::mouse_delta();
			mousedelta[0] = -gintonic::deg_to_rad(mousedelta[0]) / 4.0f;
			mousedelta[1] = -gintonic::deg_to_rad(mousedelta[1]) / 4.0f;
			default_camera.add_horizontal_and_vertical_angles(mousedelta[0], mousedelta[1]);

			//
			// drawing code goes here
			//
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			renderer::set_model_matrix(
				gintonic::quatf(
					gintonic::euler_xyz<GLfloat>(
						0.0f,
						0.0f,
						get_elapsed_seconds()
					)
				).to_rotation_matrix()
			);

			using gintonic::normalize;
			using gintonic::vec3f;
			const auto temp = renderer::camera().matrix_V() * gintonic::vec4f(0.0f, 3.0f, 0.0f, 1.0f);
			const gintonic::vec3f light_position{temp[0], temp[1], temp[2]};
			const auto light_intensity = gintonic::vec3f(1.0f, 0.5f, 0.5f);
			const auto light_attenuation = gintonic::vec3f(0.1f, 0.3f, 0.6f);
			const auto diffuse_factor = 0.8f;

			const GLint texture_unit = 0;
			tex.get().bind(texture_unit);
			program.get().activate();
			program.get().set_uniform(loc_diffuse, texture_unit);
			program.get().set_uniform(loc_diffuse_factor, diffuse_factor);
			// program.get().set_uniform(loc_color, color);
			program.get().set_uniform(loc_matrix_pvm, renderer::matrix_PVM());
			program.get().set_uniform(loc_matrix_vm, renderer::matrix_VM());
			program.get().set_uniform(loc_matrix_n, renderer::matrix_N());
			program.get().set_uniform(loc_light_position, light_position);
			program.get().set_uniform(loc_light_intensity, light_intensity);
			program.get().set_uniform(loc_light_attenuation, light_attenuation);

			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glEnable(GL_CULL_FACE);
			glFrontFace(GL_CCW);
			the_shape.draw();
			
			textshader.get().activate();
			textshader.get().set_uniform(loc_text_color, gintonic::vec4f(1,1,1,1));
			textshader.get().set_uniform(loc_text_tex, 0);

			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			stream.open(font_inconsolata20);
			stream << "Elapsed time: " << std::fixed << std::setprecision(4) << get_elapsed_seconds() << " seconds\n";
			stream << "Frames per second: " << std::fixed << std::setprecision(4) << 1.0f / get_dt<float>();
			stream << std::endl;
			stream.close();

			textshader.get().set_uniform(loc_text_color, gintonic::vec4f(2.0f * std::sin(get_elapsed_seconds()) - 1.0f, 0.5f, 0.4f, 1.0f));
			
			stream.open(font_scriptin48);
			stream->position[1] -= 1.0f;
			stream << "Curabitur aliquet quam id dui posuere blandit.\nCras ultricies ligula sed magna dictum porta.\nNulla quis lorem ut libero malesuada feugiat.\nVivamus suscipit tortor eget felis porttitor volutpat.\nNulla porttitor accumsan tincidunt.\nDonec rutrum congue leo eget malesuada.\nCurabitur aliquet quam id dui posuere blandit.\nVivamus magna justo, lacinia eget consectetur sed, convallis at tellus.\nVestibulum ac diam sit amet quam vehicula elementum sed sit amet dui.\nMauris blandit aliquet elit, eget tincidunt nibh pulvinar a." << std::flush;
			stream.close();

			renderer::update();
		}

		return EXIT_SUCCESS;
	}

	//
	// error handling
	//
	catch (const gintonic::opengl::vertex_shader::error& e)
	{
		std::cerr << "Vertex shader compilation error";
		if (const auto* t = boost::get_error_info<gintonic::opengl::vertex_shader::pathinfo>(e))
		{
			std::cerr << " in " << *t << ":\n";
		}
		else
		{
			std::cerr << ":\n";
		}
		if (const auto* t = boost::get_error_info<gintonic::opengl::vertex_shader::errinfo>(e))
		{
			std::cerr << *t << '\n';
		}
		else
		{
			std::cerr << "Unable to retrieve information log.\n";
		}

	}
	catch (const gintonic::opengl::fragment_shader::error& e)
	{
		std::cerr << "Fragment shader compilation error";
		if (const auto* t = boost::get_error_info<gintonic::opengl::fragment_shader::pathinfo>(e))
		{
			std::cerr << " in " << *t << ":\n";
		}
		else
		{
			std::cerr << ":\n";
		}
		if (const auto* t = boost::get_error_info<gintonic::opengl::fragment_shader::errinfo>(e))
		{
			std::cerr << *t << '\n';
		}
		else
		{
			std::cerr << "Unable to retrieve information log.\n";
		}

	}
	catch (const gintonic::opengl::shader::uniform_not_found& e)
	{
		std::cerr << "Shader uniform error:\n";
		if (const auto* t = boost::get_error_info<gintonic::opengl::shader::shader_errinfo>(e))
		{
			const auto& vs = std::get<0>(*t);
			const auto& gs = std::get<1>(*t);
			const auto& fs = std::get<2>(*t);
			if (gs == "") std::cerr << "Files: \n\t" << vs << "\n\t" << fs << '\n';
			else std::cerr << "Files: \n\t" << vs << "\n\t" << gs << "\n\t" << fs << '\n';
		}
		if (const auto* t = boost::get_error_info<gintonic::opengl::shader::uniform_errinfo>(e))
		{
			std::cerr << "Uniform name:\n" << *t << '\n';
		}
	}
	catch (const gintonic::opengl::shader::link_error& e)
	{
		std::cerr << "Shader link error:\n";
		if (const auto* t = boost::get_error_info<gintonic::opengl::shader::shader_errinfo>(e))
		{
			const auto& vs = std::get<0>(*t);
			const auto& gs = std::get<1>(*t);
			const auto& fs = std::get<2>(*t);
			if (gs == "") std::cerr << "Files: \n\t" << vs << "\n\t" << fs << '\n';
			else std::cerr << "Files: \n\t" << vs << "\n\t" << gs << "\n\t" << fs << '\n';
		}
		if (const auto* t = boost::get_error_info<gintonic::opengl::shader::link_errinfo>(e))
		{
			std::cerr << "Linker message:\n" << *t << '\n';
		}
	}
	catch (const boost::exception& e)
	{
		std::cerr << boost::diagnostic_information(e, true) << '\n';
	}
	return EXIT_FAILURE;
}