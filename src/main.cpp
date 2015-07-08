#include <iostream>
#include <cstddef>
#include <cassert>

#include "renderer.hpp"
#include "basic_shapes.hpp"
#include "textures.hpp"
#include "fonts.hpp"
#include "timers.hpp"
#include "mesh.hpp"
#include "vertices.hpp"

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

std::chrono::milliseconds g_timer_duration(1000);
const char* g_text_to_render = "Hello World! How's it hanging?";
const std::size_t g_text_to_render_size = std::strlen(g_text_to_render);
const char* g_text_to_render_end = g_text_to_render;

void increment_counter(gintonic::timer* t)
{
	g_text_to_render_end++;
	if (g_text_to_render_end - g_text_to_render == g_text_to_render_size + 1)
	{
		g_text_to_render_end = g_text_to_render;
		g_timer_duration /= 2;
		t->reset(g_timer_duration);
	}
}

template <typename FloatType> inline FloatType get_time()
{
	using gintonic::renderer;
	using std::chrono::duration_cast;
	using std::chrono::nanoseconds;
	return static_cast<FloatType>(duration_cast<nanoseconds>(renderer::elapsed_time()).count()) / FloatType(1e9);
}

template <typename FloatType> inline FloatType get_dt() BOOST_NOEXCEPT_OR_NOTHROW
{
	using gintonic::renderer;
	using std::chrono::duration_cast;
	using std::chrono::nanoseconds;
	return static_cast<FloatType>(duration_cast<nanoseconds>(renderer::delta_time()).count()) / FloatType(1e9);
}

struct point_diffuse_shader
{
	gintonic::opengl::shader::flyweight program;
};

struct mesh_shader
{
	gintonic::opengl::shader::flyweight program;
	GLuint matrix_PVM;
	GLuint matrix_VM;
	GLuint matrix_N;
	GLuint light_intensity;
	GLuint light_position;
	GLuint light_attenuation;
	GLuint color;
	mesh_shader()
	{
		program = gintonic::opengl::shader::flyweight("../s/pn.vs", "", "../s/pn.fs");
		matrix_PVM = program.get().get_uniform_location("matrix_PVM");
		matrix_VM = program.get().get_uniform_location("matrix_VM");
		matrix_N = program.get().get_uniform_location("matrix_N");
		light_position = program.get().get_uniform_location("light.position");
		light_intensity = program.get().get_uniform_location("light.intensity");
		light_attenuation = program.get().get_uniform_location("light.attenuation");
		color = program.get().get_uniform_location("color");
	}
};

struct text_shader
{
	gintonic::opengl::shader::flyweight program;
	
};

int main(int argc, char* argv[])
{

	#if defined(HIDE_CONSOLE) && defined(REDIRECT_OUTPUT_WHEN_HIDDEN_CONSOLE)
	std::filebuf coutfileredirect;
	std::filebuf cerrfileredirect;
	std::filebuf clogfileredirect;
	coutfileredirect.open("cout.txt", std::ios::out | std::ios::app);
	cerrfileredirect.open("cerr.txt", std::ios::out | std::ios::app);
	clogfileredirect.open("clog.txt", std::ios::out | std::ios::app);
	std::cout.rdbuf(&coutfileredirect);
	std::cerr.rdbuf(&cerrfileredirect);
	std::clog.rdbuf(&clogfileredirect);
	#endif

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
		typedef gintonic::mesh<gintonic::opengl::vertex_PN<GLfloat>> mesh_type;
		gintonic::opengl::unit_cube_PUN the_shape;
		const auto the_mesh = mesh_type::flyweight("../resources/Suzanne.pn");
		auto meshshader = shader::flyweight("../s/pn.vs", "", "../s/pn.fs");
		auto program = shader::flyweight("../s/point_diffuse.vs", "", "../s/point_diffuse.fs");
		auto textshader = shader::flyweight("../s/flat_text_uniform_color.vs", "", "../s/flat_text_uniform_color.fs");
		auto tex = texture2d::flyweight("../resources/sample.jpg");
		auto font_scriptin48 = gintonic::font::flyweight("../resources/SCRIPTIN.ttf", 48);
		auto font_inconsolata20 = gintonic::font::flyweight("../resources/Inconsolata-Regular.ttf", 20);
		gintonic::fontstream stream;

		std::ostringstream textlog;

		// try
		// {
		// 	if (boost::filesystem::exists("../resources/Suzanne.pn")) break;
		// 	typedef gintonic::mesh<gintonic::opengl::vertex_PN<GLfloat>> mesh_type;
		// 	textlog << "Attempting to process Suzanne.obj...\n";
		// 	const boost::filesystem::path mesh_filename = mesh_type::process("../resources/Suzanne.obj");
		// 	textlog << "Success! Location: " << mesh_filename << '\n';
		// 	textlog << "Attempting to load the mesh...\n";
		// 	const auto the_mesh = mesh_type::flyweight(mesh_filename);
		// 	textlog << "Success!\n";
		// }
		// catch (const std::exception& e)
		// {
		// 	textlog << "Failed: " << e.what() << '\n';
		// }

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

		double curtime, dt;

		{
			gintonic::timer* alooptimer = new gintonic::loop_timer(g_timer_duration);
			alooptimer->action.connect(increment_counter);
			gintonic::timer::add(alooptimer);
		}

		while (!renderer::should_close())
		{
			curtime = get_time<double>();
			dt = get_dt<double>();
			gintonic::timer::update_all(renderer::delta_time());
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
						curtime
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
			stream << "Elapsed time: " << std::fixed << std::setprecision(4) << curtime << " seconds\n";
			stream << "Frames per second: " << std::fixed << std::setprecision(4) << 1.0f / dt << '\n';
			stream << textlog.str();
			stream << std::endl;
			stream.close();

			textshader.get().set_uniform(loc_text_color, gintonic::vec4f(1.0f, 0.5f, 0.4f, 1.0f));
			
			stream.open(font_scriptin48);
			stream->position[1] -= 1.0f;
			{
				const std::string text_to_render(g_text_to_render, g_text_to_render_end);
				stream << text_to_render;
			}
			stream << std::endl;
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

	#if defined(HIDE_CONSOLE) && defined(REDIRECT_OUTPUT_WHEN_HIDDEN_CONSOLE)
	coutfileredirect.close();
	cerrfileredirect.close();
	clogfileredirect.close();
	#endif

	return EXIT_FAILURE;
}