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
#include "actors.hpp"
#include "shaders.hpp"
#include "materials.hpp"
#include "lights.hpp"
#include <boost/program_options.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#define APP_NAME "Hello world!"

#ifndef DEBUG_PRINT
#define DEBUG_PRINT_START std::cerr << __FILE__ << '(' << __LINE__ << ')'
#define DEBUG_PRINT DEBUG_PRINT_START << '\n'
#define PRINT_VAR(var) DEBUG_PRINT_START << ": " << #var << " == " << var << '\n'
#endif

gintonic::actor* initialize_view_actor(const boost::program_options::variables_map& vm)
{
	auto manager = FbxManager::Create();
	auto iosettings = FbxIOSettings::Create(manager, IOSROOT);
	manager->SetIOSettings(iosettings);
	auto importer = FbxImporter::Create(manager, "");
	{
		const auto filename = vm["view-model"].as<std::string>();
		const auto status = importer->Initialize(filename.c_str(), -1, manager->GetIOSettings());
		if (!status)
		{
			std::cout << "Unable to import scene " << filename << '\n';
			return nullptr;
		}
	}
	
	auto scene = FbxScene::Create(manager, "");
	
	importer->Import(scene);
	importer->Destroy();
	
	FbxGeometryConverter converter(manager);
	converter.Triangulate(scene, true);
	gintonic::actor* new_actor = nullptr;
	try
	{
		new_actor = new gintonic::static_model_actor(scene);
		std::cout << "Total number of meshes: " 
			<< reinterpret_cast<gintonic::static_model_actor*>(new_actor)->model.size() << '\n'
			<< "Total number of lights: " 
			<< reinterpret_cast<gintonic::static_model_actor*>(new_actor)->lights.size() << '\n';
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << '\n';
		manager->Destroy();
		return nullptr;
	}
	manager->Destroy();
	return new_actor;
}

template <class FloatType> FloatType get_time() BOOST_NOEXCEPT_OR_NOTHROW
{
	using gintonic::renderer;
	using std::chrono::duration_cast;
	using std::chrono::nanoseconds;
	return duration_cast<nanoseconds>(renderer::elapsed_time()).count() / FloatType(1e9);
}

template <class FloatType> FloatType get_dt() BOOST_NOEXCEPT_OR_NOTHROW
{
	using gintonic::renderer;
	using std::chrono::duration_cast;
	using std::chrono::nanoseconds;
	return duration_cast<nanoseconds>(renderer::delta_time()).count() / FloatType(1e9);
}

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

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
		("help", "Produce this help message.")
		// ("validate-model,m", boost::program_options::value<std::string>(), "Validate a .obj or .fbx model.")
		// ("process-model,p", boost::program_options::value<std::string>(), "Process a .obj or .fbx model.")
		("view-model,v", boost::program_options::value<std::string>(), "View a single model in native format.")
	;

	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm);

	if (vm.count("help")) 
	{
		std::cout << desc << '\n';
		return EXIT_FAILURE;
	}

	gintonic::actor* an_actor = nullptr;

	gintonic::camera_transform<float> default_camera;
	default_camera.projection_type = gintonic::camera_transform<float>::e_projection_type::projective;
	default_camera.field_of_view = gintonic::deg_to_rad(60.0f);
	default_camera.near_plane = 0.1f;
	default_camera.far_plane = 100.0f;
	default_camera.position = {0, 0, 4};
	default_camera.add_horizontal_and_vertical_angles(static_cast<float>(M_PI), 0.0f);

	try 
	{
		std::string title(PROJECT_NAME);
		title.append(" version ");
		title.append(PROJECT_VERSION);
		gintonic::renderer::init(title.c_str(), default_camera, true, 800, 640);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Failed to initialize renderer: " << e.what() << '\n';
		return EXIT_FAILURE;
	}
	try
	{
		gintonic::opengl::texture2d::init();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Failed to initialize textures: " << e.what() << '\n';
		return EXIT_FAILURE;
	}
	try
	{
		gintonic::font::init();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Failed to initialize fonts: " << e.what() << '\n';
		return EXIT_FAILURE;
	}
	std::cout << gintonic::renderer::name() << '\n';
	std::cout << gintonic::renderer::version() << '\n';

	if (vm.count("view-model"))
	{
		an_actor = initialize_view_actor(vm);
	}

	gintonic::opengl::unit_cube_PUN the_shape;
	gintonic::color_material the_material;

	auto font_inconsolata20 = gintonic::font::flyweight("../resources/Inconsolata-Regular.ttf", 20);
	gintonic::fontstream stream;

	gintonic::renderer::show();
	gintonic::renderer::focus_context();
	gintonic::renderer::set_freeform_cursor(true);

	float curtime, dt;
	glClearColor(0,0,0,1);
	while (!gintonic::renderer::should_close())
	{
		curtime = get_time<float>();
		dt = get_dt<float>();
		gintonic::timer::update_all(gintonic::renderer::delta_time());
		//
		// process events
		//
		if (gintonic::renderer::key_toggle_press(SDL_SCANCODE_Q))
		{
			gintonic::renderer::close();
		}
		if (gintonic::renderer::key(SDL_SCANCODE_W))
		{
			default_camera.move_forward(dt);
		}
		if (gintonic::renderer::key(SDL_SCANCODE_A))
		{
			default_camera.move_left(dt);
		}
		if (gintonic::renderer::key(SDL_SCANCODE_S))
		{
			default_camera.move_backward(dt);
		}
		if (gintonic::renderer::key(SDL_SCANCODE_D))
		{
			default_camera.move_right(dt);
		}
		if (gintonic::renderer::key(SDL_SCANCODE_SPACE))
		{
			default_camera.move_up(dt);
		}
		auto mousedelta = gintonic::renderer::mouse_delta();
		mousedelta[0] = -gintonic::deg_to_rad(mousedelta[0]) / 4.0f;
		mousedelta[1] = -gintonic::deg_to_rad(mousedelta[1]) / 4.0f;
		default_camera.add_horizontal_and_vertical_angles(mousedelta[0], mousedelta[1]);

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);

		gintonic::renderer::bind_for_writing();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (an_actor) gintonic::renderer::set_model_matrix(an_actor->transform.get_matrix());
		else gintonic::renderer::set_model_matrix(1);

		the_material.diffuse_color = gintonic::vec4f(0,1,0,1);
		the_material.bind();
		the_shape.draw();

		if (an_actor) an_actor->draw_geometry();

		auto the_text_shader = gintonic::renderer::get_text_shader();
		the_text_shader->activate();
		the_text_shader->set_color(gintonic::vec3f(1,1,1));
		the_text_shader->set_texture(0);

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		stream.open(font_inconsolata20);
		stream << "Elapsed time:      " << std::fixed << std::setprecision(4) << curtime << " seconds\n";
		stream << "Frames per second: " << std::fixed << std::setprecision(4) << 1.0f / dt << '\n';
		stream << "Camera position:   " << gintonic::renderer::camera().position << '\n';
		stream << "Camera up:         " << gintonic::renderer::camera().up << '\n';
		stream << "Camera right:      " << gintonic::renderer::camera().right <<'\n';
		stream << std::endl;
		stream.close();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		gintonic::renderer::bind_for_reading();

		if (an_actor) an_actor->draw_lights();

		const GLsizei halfwidth = (GLsizei)(gintonic::renderer::width() / 2.0f);
		const GLsizei halfheight = (GLsizei)(gintonic::renderer::height() / 2.0f);

		gintonic::renderer::set_read_buffer(gintonic::renderer::GBUFFER_POSITION);
		glBlitFramebuffer(0, 0, gintonic::renderer::width(), gintonic::renderer::height(), 0, 0, halfwidth, halfheight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		gintonic::renderer::set_read_buffer(gintonic::renderer::GBUFFER_DIFFUSE);
		glBlitFramebuffer(0, 0, gintonic::renderer::width(), gintonic::renderer::height(), 0, halfheight, halfwidth, gintonic::renderer::height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);

		gintonic::renderer::set_read_buffer(gintonic::renderer::GBUFFER_NORMAL);
		glBlitFramebuffer(0, 0, gintonic::renderer::width(), gintonic::renderer::height(), halfwidth, halfheight, gintonic::renderer::width(), gintonic::renderer::height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);

		gintonic::renderer::set_read_buffer(gintonic::renderer::GBUFFER_TEXCOORD);
		glBlitFramebuffer(0, 0, gintonic::renderer::width(), gintonic::renderer::height(), halfwidth, 0, gintonic::renderer::width(), halfheight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		gintonic::renderer::update();
	}

	if (an_actor) delete an_actor;

	return EXIT_SUCCESS;

	// try
	// {
	// 	using namespace gintonic;
	// 	using gintonic::renderer;
	// 	using gintonic::opengl::texture2d;
	// 	using gintonic::opengl::shader;
		
	// 	gintonic::camera_transform<float> default_camera;
		
	// 	//
	// 	// set up projection matrix parameters
	// 	//
	// 	default_camera.projection_type = gintonic::camera_transform<float>::e_projection_type::projective;
	// 	default_camera.field_of_view = gintonic::deg_to_rad(60.0f);
	// 	default_camera.near_plane = 0.1f;
	// 	default_camera.far_plane = 100.0f;
		
	// 	//
	// 	// set up view matrix parameters
	// 	//
	// 	default_camera.position = {0, 0, 20};
	// 	default_camera.add_horizontal_and_vertical_angles(static_cast<float>(M_PI), 0.0f);

	// 	//
	// 	// initialize renderer
	// 	//
	// 	{
	// 		std::string title(PROJECT_NAME);
	// 		title.append(" version ");
	// 		title.append(PROJECT_VERSION);
	// 		gintonic::renderer::init(title.c_str(), default_camera, true, 800, 640);
	// 		gintonic::opengl::texture2d::init();
	// 		gintonic::font::init();
	// 	}

	// 	std::cout << renderer::name() << '\n';
	// 	std::cout << renderer::version() << '\n';

	// 	renderer::about_to_close.connect(about_to_close_handler);

	// 	renderer::show();
	// 	renderer::focus_context();
	// 	renderer::set_freeform_cursor(true);

	// 	//
	// 	// initialize things to render
	// 	//
	// 	mesh_shader the_mesh_shader;
	// 	point_diffuse_shader the_point_diffuse_shader;
	// 	text_shader the_text_shader;
	// 	std::ostringstream textlog;

	// 	std::shared_ptr<opengl::shader> geom_shader;
	// 	std::shared_ptr<opengl::shader> light_shader;
	// 	std::string infolog;
	// 	try
	// 	{
	// 		geom_shader = std::make_shared<geometry_pass_shader>(infolog);
	// 	}
	// 	catch (const std::exception& e)
	// 	{
	// 		std::cerr << "Error in geometry shader:\n";
	// 		std::cerr << infolog << '\n';
	// 		throw;
	// 	}
	// 	try
	// 	{
	// 		light_shader = std::make_shared<directional_light_pass_shader>(infolog);
	// 	}
	// 	catch (const std::exception& e)
	// 	{
	// 		std::cerr << "Error in directional light shader:\n";
	// 		std::cerr << infolog << '\n';
	// 		throw;
	// 	}
	// 	std::shared_ptr<static_model_actor> some_actor;
	// 	try
	// 	{
	// 		some_actor = std::make_shared<static_model_actor>(geom_shader);
	// 		std::shared_ptr<material> some_material;
	// 		std::shared_ptr<mesh> some_mesh;
	// 		sqt_transformf some_transform;

	// 		// ...

	// 		some_actor->model.emplace_back(some_mesh, some_material, some_transform);
	// 	}
	// 	catch (const std::exception& e)
	// 	{
	// 		std::cerr << "Error while constructing actor.\n";
	// 		throw;
	// 	}

	// 	// ...

	// 	some_actor->draw_geometry();

	// 	// ...

	// 	some_actor->draw_lights();


	// 	textlog << "Mesh: " << g_model_path << '\n';
	// 	gintonic::opengl::unit_cube_PUN the_shape;
	// 	const gintonic::mesh::flyweight the_mesh(g_model_path);
	// 	auto tex = texture2d::flyweight("../resources/sample.jpg");
	// 	auto mesh_diffuse_tex = texture2d::flyweight("../resources/Cube.png");
	// 	auto font_scriptin48 = gintonic::font::flyweight("../resources/SCRIPTIN.ttf", 48);
	// 	auto font_inconsolata20 = gintonic::font::flyweight("../resources/Inconsolata-Regular.ttf", 20);
	// 	gintonic::fontstream stream;

	// 	//
	// 	// start main loop
	// 	//
	// 	glClearColor(0,0,0,1);

	// 	double curtime, dt;

	// 	{
	// 		gintonic::timer* alooptimer = new gintonic::loop_timer(g_timer_duration);
	// 		alooptimer->action.connect(increment_counter);
	// 		gintonic::timer::add(alooptimer);
	// 	}

	// 	while (!renderer::should_close())
	// 	{
	// 		curtime = get_time<double>();
	// 		dt = get_dt<double>();
	// 		gintonic::timer::update_all(renderer::delta_time());
	// 		//
	// 		// process events
	// 		//
	// 		if (renderer::key_toggle_press(SDL_SCANCODE_Q))
	// 		{
	// 			renderer::close();
	// 		}
	// 		if (renderer::key(SDL_SCANCODE_W))
	// 		{
	// 			default_camera.move_forward(dt);
	// 		}
	// 		if (renderer::key(SDL_SCANCODE_A))
	// 		{
	// 			default_camera.move_left(dt);
	// 		}
	// 		if (renderer::key(SDL_SCANCODE_S))
	// 		{
	// 			default_camera.move_backward(dt);
	// 		}
	// 		if (renderer::key(SDL_SCANCODE_D))
	// 		{
	// 			default_camera.move_right(dt);
	// 		}
	// 		if (renderer::key(SDL_SCANCODE_SPACE))
	// 		{
	// 			default_camera.move_up(dt);
	// 		}
	// 		auto mousedelta = renderer::mouse_delta();
	// 		mousedelta[0] = -gintonic::deg_to_rad(mousedelta[0]) / 4.0f;
	// 		mousedelta[1] = -gintonic::deg_to_rad(mousedelta[1]) / 4.0f;
	// 		default_camera.add_horizontal_and_vertical_angles(mousedelta[0], mousedelta[1]);

	// 		//
	// 		// drawing code goes here
	// 		//
	// 		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 		using gintonic::normalize;
	// 		using gintonic::vec3f;
	// 		const auto light_position_world = gintonic::vec3f(10,20,3);
	// 		const auto temp = renderer::camera().matrix_V() * gintonic::vec4f(light_position_world[0], 
	// 			light_position_world[1], light_position_world[2], 1.0f);
	// 		const gintonic::vec3f light_position_view(temp[0], temp[1], temp[2]);
	// 		const auto light_intensity = gintonic::vec3f(1,1,1);
	// 		const auto light_attenuation = gintonic::vec3f(1,1,1);
	// 		const auto diffuse_factor = 0.8f;

	// 		// program.get().set_uniform(loc_diffuse, texture_unit);
	// 		// program.get().set_uniform(loc_diffuse_factor, diffuse_factor);
	// 		// // program.get().set_uniform(loc_color, color);
	// 		// program.get().set_uniform(loc_matrix_pvm, renderer::matrix_PVM());
	// 		// program.get().set_uniform(loc_matrix_vm, renderer::matrix_VM());
	// 		// program.get().set_uniform(loc_matrix_n, renderer::matrix_N());
	// 		// program.get().set_uniform(loc_light_position, light_position);
	// 		// program.get().set_uniform(loc_light_intensity, light_intensity);
	// 		// program.get().set_uniform(loc_light_attenuation, light_attenuation);

	// 		glDisable(GL_BLEND);
	// 		glEnable(GL_DEPTH_TEST);
	// 		glDepthMask(GL_TRUE);
	// 		glEnable(GL_CULL_FACE);
	// 		glFrontFace(GL_CCW);

	// 		renderer::set_model_matrix
	// 		(
	// 			// translation constructor
	// 			gintonic::mat4f(light_position_world)
	// 		);

	// 		const GLint texture_unit = 0;
	// 		tex.get().bind(texture_unit);
	// 		the_point_diffuse_shader.program.get().activate();
	// 		the_point_diffuse_shader.set_uniforms
	// 		(
	// 			renderer::matrix_PVM(), 
	// 			renderer::matrix_VM(), 
	// 			renderer::matrix_N(), 
	// 			light_position_view, 
	// 			light_intensity, 
	// 			light_attenuation, 
	// 			texture_unit,
	// 			diffuse_factor
	// 		);
	// 		the_shape.draw();

	// 		renderer::set_model_matrix
	// 		(
	// 			// angle-axis constructor
	// 			gintonic::mat4f(static_cast<float>(curtime / 2.0), gintonic::vec3f(0,1,0))
	// 		);

	// 		the_mesh_shader.program.get().activate();
	// 		the_mesh_shader.set_uniforms(
	// 			renderer::matrix_PVM(), 
	// 			renderer::matrix_VM(),
	// 			renderer::matrix_N(),
	// 			light_position_view,
	// 			light_intensity,
	// 			light_attenuation,
	// 			0,
	// 			0.8f);
	// 		mesh_diffuse_tex.get().bind(0);
	// 		the_mesh.get().draw();

	// 		the_text_shader.program.get().activate();
	// 		the_text_shader.set_uniforms(gintonic::vec3f(1,1,1), 0);

	// 		glDisable(GL_CULL_FACE);
	// 		glDisable(GL_DEPTH_TEST);
	// 		glEnable(GL_BLEND);
	// 		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// 		stream.open(font_inconsolata20);
	// 		stream << "Elapsed time: " << std::fixed << std::setprecision(4) << curtime << " seconds\n";
	// 		stream << "Frames per second: " << std::fixed << std::setprecision(4) << 1.0f / dt << '\n';
	// 		stream << textlog.str();
	// 		stream << "Camera position: " << renderer::camera().position << '\n';
	// 		stream << "Camera up:       " << renderer::camera().up << '\n';
	// 		stream << "Camera right:    " << renderer::camera().right <<'\n';
	// 		stream << std::endl;
	// 		stream.close();

	// 		the_text_shader.program.get().set_uniform(the_text_shader.color, gintonic::vec3f(1.0f, 0.5f, 0.3f));
			
	// 		stream.open(font_scriptin48);
	// 		stream->position[1] -= 1.0f;
	// 		{
	// 			const std::string text_to_render(g_text_to_render, g_text_to_render_end);
	// 			stream << text_to_render;
	// 		}
	// 		stream << std::endl;
	// 		stream.close();

	// 		renderer::update();
	// 	}

	// 	return EXIT_SUCCESS;
	// }
	// //
	// // error handling
	// //
	// catch (const gintonic::opengl::vertex_shader::error& e)
	// {
	// 	std::cerr << "Vertex shader compilation error";
	// 	if (const auto* t = boost::get_error_info<gintonic::opengl::vertex_shader::pathinfo>(e))
	// 	{
	// 		std::cerr << " in " << *t << ":\n";
	// 	}
	// 	else
	// 	{
	// 		std::cerr << ":\n";
	// 	}
	// 	if (const auto* t = boost::get_error_info<gintonic::opengl::vertex_shader::errinfo>(e))
	// 	{
	// 		std::cerr << *t << '\n';
	// 	}
	// 	else
	// 	{
	// 		std::cerr << "Unable to retrieve information log.\n";
	// 	}

	// }
	// catch (const gintonic::opengl::fragment_shader::error& e)
	// {
	// 	std::cerr << "Fragment shader compilation error";
	// 	if (const auto* t = boost::get_error_info<gintonic::opengl::fragment_shader::pathinfo>(e))
	// 	{
	// 		std::cerr << " in " << *t << ":\n";
	// 	}
	// 	else
	// 	{
	// 		std::cerr << ":\n";
	// 	}
	// 	if (const auto* t = boost::get_error_info<gintonic::opengl::fragment_shader::errinfo>(e))
	// 	{
	// 		std::cerr << *t << '\n';
	// 	}
	// 	else
	// 	{
	// 		std::cerr << "Unable to retrieve information log.\n";
	// 	}

	// }
	// catch (const gintonic::opengl::shader::uniform_not_found& e)
	// {
	// 	std::cerr << "Shader uniform error:\n";
	// 	if (const auto* t = boost::get_error_info<gintonic::opengl::shader::shader_errinfo>(e))
	// 	{
	// 		const auto& vs = std::get<0>(*t);
	// 		const auto& gs = std::get<1>(*t);
	// 		const auto& fs = std::get<2>(*t);
	// 		if (gs == "") std::cerr << "Files: \n\t" << vs << "\n\t" << fs << '\n';
	// 		else std::cerr << "Files: \n\t" << vs << "\n\t" << gs << "\n\t" << fs << '\n';
	// 	}
	// 	if (const auto* t = boost::get_error_info<gintonic::opengl::shader::uniform_errinfo>(e))
	// 	{
	// 		std::cerr << "Uniform name:\n" << *t << '\n';
	// 	}
	// }
	// catch (const gintonic::opengl::shader::link_error& e)
	// {
	// 	std::cerr << "Shader link error:\n";
	// 	if (const auto* t = boost::get_error_info<gintonic::opengl::shader::shader_errinfo>(e))
	// 	{
	// 		const auto& vs = std::get<0>(*t);
	// 		const auto& gs = std::get<1>(*t);
	// 		const auto& fs = std::get<2>(*t);
	// 		if (gs == "") std::cerr << "Files: \n\t" << vs << "\n\t" << fs << '\n';
	// 		else std::cerr << "Files: \n\t" << vs << "\n\t" << gs << "\n\t" << fs << '\n';
	// 	}
	// 	if (const auto* t = boost::get_error_info<gintonic::opengl::shader::link_errinfo>(e))
	// 	{
	// 		std::cerr << "Linker message:\n" << *t << '\n';
	// 	}
	// }
	// catch (const boost::exception& e)
	// {
	// 	std::cerr << boost::diagnostic_information(e, true) << '\n';
	// }

	#if defined(HIDE_CONSOLE) && defined(REDIRECT_OUTPUT_WHEN_HIDDEN_CONSOLE)
	coutfileredirect.close();
	cerrfileredirect.close();
	clogfileredirect.close();
	#endif

	return EXIT_FAILURE;
}