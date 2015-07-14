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
#include <boost/program_options.hpp>

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
	if (g_text_to_render_end == g_text_to_render + g_text_to_render_size + 1)
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
	GLint matrix_PVM;
	GLint matrix_VM;
	GLint matrix_N;
	GLint light_intensity;
	GLint light_position;
	GLint light_attenuation;
	GLint material_diffuse_color;
	GLint material_diffuse_factor;

	point_diffuse_shader() : program("../s/point_diffuse.vs", "", "../s/point_diffuse.fs")
	{
		matrix_PVM = program.get().get_uniform_location("matrix_PVM");
		matrix_VM = program.get().get_uniform_location("matrix_VM");
		matrix_N = program.get().get_uniform_location("matrix_N");
		light_position = program.get().get_uniform_location("light.position");
		light_intensity = program.get().get_uniform_location("light.intensity");
		light_attenuation = program.get().get_uniform_location("light.attenuation");
		material_diffuse_color = program.get().get_uniform_location("material.diffuse_color");
		material_diffuse_factor = program.get().get_uniform_location("material.diffuse_factor");
	}

	void set_uniforms(const gintonic::mat4f& matrix_PVM_value, 
		const gintonic::mat4f& matrix_VM_value, 
		const gintonic::mat3f& matrix_N_value, 
		const gintonic::vec3f& light_position_value, 
		const gintonic::vec3f& light_intensity_value, 
		const gintonic::vec3f& light_attenuation_value, 
		const GLint material_diffuse_color_value, 
		const GLfloat material_diffuse_factor_value) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		const auto& s = program.get();
		s.set_uniform(matrix_PVM, matrix_PVM_value);
		s.set_uniform(matrix_VM, matrix_VM_value);
		s.set_uniform(matrix_N, matrix_N_value);
		s.set_uniform(light_position, light_position_value);
		s.set_uniform(light_intensity, light_intensity_value);
		s.set_uniform(light_attenuation, light_attenuation_value);
		s.set_uniform(material_diffuse_color, material_diffuse_color_value);
		s.set_uniform(material_diffuse_factor, material_diffuse_factor_value);
	}
};

struct mesh_shader
{
	gintonic::opengl::shader::flyweight program;
	GLint matrix_PVM;
	GLint matrix_VM;
	GLint matrix_N;
	GLint light_intensity;
	GLint light_position;
	GLint light_attenuation;
	GLint color;

	mesh_shader() : program("../s/pn_point.vs", "", "../s/pn_point.fs")
	{
		matrix_PVM = program.get().get_uniform_location("matrix_PVM");
		matrix_VM = program.get().get_uniform_location("matrix_VM");
		matrix_N = program.get().get_uniform_location("matrix_N");
		light_position = program.get().get_uniform_location("light.position");
		light_intensity = program.get().get_uniform_location("light.intensity");
		light_attenuation = program.get().get_uniform_location("light.attenuation");
		color = program.get().get_uniform_location("color");
	}

	void set_uniforms(const gintonic::mat4f& matrix_PVM_value, 
		const gintonic::mat4f& matrix_VM_value, 
		const gintonic::mat3f& matrix_N_value, 
		const gintonic::vec3f& light_position_value, 
		const gintonic::vec3f& light_intensity_value, 
		const gintonic::vec3f& light_attenuation_value, 
		const gintonic::vec3f& color_value) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		const auto& s = program.get();
		s.set_uniform(matrix_PVM, matrix_PVM_value);
		s.set_uniform(matrix_VM, matrix_VM_value);
		s.set_uniform(matrix_N, matrix_N_value);
		s.set_uniform(light_position, light_position_value);
		s.set_uniform(light_intensity, light_intensity_value);
		s.set_uniform(light_attenuation, light_attenuation_value);
		s.set_uniform(color, color_value);
	}
};

struct text_shader
{
	gintonic::opengl::shader::flyweight program;
	GLint color;
	GLint tex;

	text_shader() : program("../s/flat_text_uniform_color.vs", "", "../s/flat_text_uniform_color.fs")
	{
		color = program.get().get_uniform_location("color");
		tex = program.get().get_uniform_location("tex");
	}

	void set_uniforms(const gintonic::vec3f color_value, 
		const GLint tex_value) const BOOST_NOEXCEPT_OR_NOTHROW
	{
		const auto& s = program.get();
		s.set_uniform(color, color_value);
		s.set_uniform(tex, tex_value);
	}
};

boost::filesystem::path g_model_path;

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
		boost::program_options::options_description desc("Allowed options");
		desc.add_options()
			("help", "Produce this help message.")
			("validate-model,m", boost::program_options::value<std::string>(), "Validate a .obj or .fbx model.")
			("process-model,p", boost::program_options::value<std::string>(), "Process a .obj or .fbx model.")
			("view-model,v", boost::program_options::value<std::string>(), "View a single model in native format.")
		;

		boost::program_options::variables_map vm;
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
		boost::program_options::notify(vm);

		if (vm.count("help")) {
		    std::cout << desc << '\n';
		    return EXIT_FAILURE;
		}
		else if (vm.count("validate-model"))
		{
			const boost::filesystem::path model_path = vm["validate-model"].as<std::string>();
			gintonic::camera_transform<float> default_camera;
			gintonic::renderer::init("dummy", default_camera, false, 800, 640);
			gintonic::renderer::focus_context();
			gintonic::renderer::hide();
			const auto success = gintonic::mesh::validate(std::cout, model_path);
			return success? EXIT_SUCCESS : EXIT_FAILURE;
		}
		else if (vm.count("process-model"))
		{
			const boost::filesystem::path model_path = vm["process-model"].as<std::string>();
			gintonic::camera_transform<float> default_camera;
			gintonic::renderer::init("dummy", default_camera, false, 800, 640);
			gintonic::renderer::focus_context();
			gintonic::renderer::hide();
			try
			{
				const auto new_model = gintonic::mesh::process(model_path);
				std::cout << "Native model format saved to: " << new_model << '\n';
			}
			catch (const std::exception& e)
			{
				std::cerr << e.what() << '\n';
				return EXIT_FAILURE;
			}
			return EXIT_SUCCESS;
		}
		else if (vm.count("view-model"))
		{
			g_model_path = vm["view-model"].as<std::string>();
		}
		else
		{
			std::cout << desc << '\n';
			return EXIT_FAILURE;
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

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
		default_camera.position = {0, 0, 20};
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

		renderer::show();
		renderer::focus_context();
		renderer::set_freeform_cursor(true);

		//
		// initialize things to render
		//
		mesh_shader the_mesh_shader;
		point_diffuse_shader the_point_diffuse_shader;
		text_shader the_text_shader;
		std::ostringstream textlog;

		textlog << "Mesh: " << g_model_path << '\n';
		gintonic::opengl::unit_cube_PUN the_shape;
		const gintonic::mesh::flyweight the_mesh(g_model_path);
		auto tex = texture2d::flyweight("../resources/sample.jpg");
		auto font_scriptin48 = gintonic::font::flyweight("../resources/SCRIPTIN.ttf", 48);
		auto font_inconsolata20 = gintonic::font::flyweight("../resources/Inconsolata-Regular.ttf", 20);
		gintonic::fontstream stream;

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
			


			using gintonic::normalize;
			using gintonic::vec3f;
			const auto light_position_world = gintonic::vec3f(10,20,3);
			const auto temp = renderer::camera().matrix_V() * gintonic::vec4f(light_position_world[0], 
				light_position_world[1], light_position_world[2], 1.0f);
			const gintonic::vec3f light_position_view(temp[0], temp[1], temp[2]);
			const auto light_intensity = gintonic::vec3f(1,1,1);
			const auto light_attenuation = gintonic::vec3f(1,1,1);
			const auto diffuse_factor = 0.8f;

			// program.get().set_uniform(loc_diffuse, texture_unit);
			// program.get().set_uniform(loc_diffuse_factor, diffuse_factor);
			// // program.get().set_uniform(loc_color, color);
			// program.get().set_uniform(loc_matrix_pvm, renderer::matrix_PVM());
			// program.get().set_uniform(loc_matrix_vm, renderer::matrix_VM());
			// program.get().set_uniform(loc_matrix_n, renderer::matrix_N());
			// program.get().set_uniform(loc_light_position, light_position);
			// program.get().set_uniform(loc_light_intensity, light_intensity);
			// program.get().set_uniform(loc_light_attenuation, light_attenuation);

			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glEnable(GL_CULL_FACE);
			glFrontFace(GL_CCW);

			renderer::set_model_matrix
			(
				// translation constructor
				gintonic::mat4f(1.0f)
			);

			const GLint texture_unit = 0;
			tex.get().bind(texture_unit);
			the_point_diffuse_shader.program.get().activate();
			the_point_diffuse_shader.set_uniforms
			(
				renderer::matrix_PVM(), 
				renderer::matrix_VM(), 
				renderer::matrix_N(), 
				light_position_view, 
				light_intensity, 
				light_attenuation, 
				texture_unit,
				diffuse_factor
			);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			the_shape.draw();
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			renderer::set_model_matrix
			(
				// angle-axis constructor
				gintonic::mat4f(static_cast<float>(curtime / 2.0), gintonic::vec3f(0,1,0))
			);

			the_mesh_shader.program.get().activate();
			the_mesh_shader.set_uniforms(renderer::matrix_PVM(), 
				renderer::matrix_VM(),
				renderer::matrix_N(),
				light_position_view,
				light_intensity,
				light_attenuation,
				light_intensity);
			the_mesh.get().draw();

			the_text_shader.program.get().activate();
			the_text_shader.set_uniforms(gintonic::vec3f(1,1,1), 0);

			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			stream.open(font_inconsolata20);
			stream << "Elapsed time: " << std::fixed << std::setprecision(4) << curtime << " seconds\n";
			stream << "Frames per second: " << std::fixed << std::setprecision(4) << 1.0f / dt << '\n';
			stream << textlog.str();
			stream << "Camera position: " << renderer::camera().position << '\n';
			stream << "Camera up:       " << renderer::camera().up << '\n';
			stream << "Camera right:    " << renderer::camera().right <<'\n';
			stream << std::endl;
			stream.close();

			the_text_shader.program.get().set_uniform(the_text_shader.color, gintonic::vec3f(1.0f, 0.5f, 0.3f));
			
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