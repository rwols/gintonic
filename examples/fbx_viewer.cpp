#include "gintonic.hpp"
// #define FBXSDK_NEW_API
#include <fbxsdk.h>

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

template <class MeshOutputIter>
void handle_mesh(FbxMesh* m, gt::entity& ent, MeshOutputIter mesh_iter)
{
	*mesh_iter = gt::static_mesh(m);
	ent.mesh_component = &(*mesh_iter);
	++mesh_iter;
}

template <class MaterialOutputIter>
void handle_material(FbxSurfaceMaterial* mat, gt::entity& ent, MaterialOutputIter material_iter)
{
	*material_iter = gt::material(mat);
	ent.material_component = &(*material_iter);
	++material_iter;
}

template <class LightOutputIter>
void handle_light(FbxLight* fbxlight, gt::entity& ent, LightOutputIter light_iter)
{

}

template
<
	class EntityOutputIter,
	class MeshOutputIter,
	class MaterialOutputIter,
	class LightOutputIter
>
void traverse_scene(
	FbxNode*           node,
	EntityOutputIter   ent_iter, 
	MeshOutputIter     mesh_iter, 
	MaterialOutputIter material_iter, 
	LightOutputIter    light_iter)
{
	std::cout << "Handling node: " << node->GetName() << '\n';

	sqt_stack.emplace(
		node->EvaluateLocalScaling(),
		node->EvaluateLocalRotation(),
		node->EvaluateLocalTranslation());
	{
		gt::SQT sqt;
		sqt.scale = node->EvaluateLocalScaling();
		sqt.rotation = node->EvaluateLocalRotation();
		sqt.translation = node->EvaluateLocalTranslation();
		gt::box3f bbox;
		gt::entity new_ent(sqt, bbox);

		std::cout << "Local SQT: " << sqt << '\n';

		*ent_iter = new_ent;
		++ent_iter;

		if (node->GetMesh())
		{
			handle_mesh(
				node->GetMesh(),
				new_ent,
				mesh_iter);
		}
		if (node->GetMaterialCount())
		{
			// We do only one material per mesh...
			handle_material(
				node->GetMaterial(0), 
				new_ent, 
				material_iter);
		}
		if (node->GetLight())
		{
			handle_light(
				node->GetLight(),
				new_ent,
				light_iter);
		}
	}

	for (int i = 0; i < node->GetChildCount(); ++i)
	{
		traverse_scene(
			node->GetChild(i),
			ent_iter, 
			mesh_iter, 
			material_iter, 
			light_iter);
	}
}

template
<
	class EntityOutputIter,
	class MeshOutputIter,
	class MaterialOutputIter,
	class LightOutputIter
>
void load_fbx_file(
	const char*        filepath, 
	EntityOutputIter   ent_iter, 
	MeshOutputIter     mesh_iter, 
	MaterialOutputIter material_iter, 
	LightOutputIter    light_iter)
{
	auto manager = FbxManager::Create();
	auto scene = FbxScene::Create(manager, "");
	auto importer = FbxImporter::Create(manager, "");
	const boost::filesystem::path filename(filepath);
	if (!boost::filesystem::is_regular_file(filename))
	{
		importer->Destroy();
		scene->Destroy();
		manager->Destroy();
		gt::exception e(filename.string());
		e.append(" is not a regular file");
		throw e;
	}
	else if (filename.extension() != ".fbx" && filename.extension() != ".FBX")
	{
		importer->Destroy();
		scene->Destroy();
		manager->Destroy();
		gt::exception e(filename.string());
		e.append(" is not an FBX file.");
		throw e;
	}
	const auto status = importer->Initialize(argv[1], -1, manager->GetIOSettings());
	if (!status)
	{
		gt::exception e(importer->GetFileName().Buffer());
		e.append(": ");
		e.append(importer->GetStatus().GetErrorString());
		importer->Destroy();
		scene->Destroy();
		manager->Destroy();
		throw e;
	}
	importer->Import(scene);
	importer->Destroy();
	{
		FbxAxisSystem axis_system(FbxAxisSystem::EUpVector::eYAxis,
			FbxAxisSystem::EFrontVector::eParityEven,
			FbxAxisSystem::eRightHanded);
		axis_system.ConvertScene(scene);
		FbxGeometryConverter converter(manager);
		converter.Triangulate(scene, true);
	}
	// So at this point we have the FbxScene and we can traverse it
	// for entities, lights, materials and meshes.
	
	traverse_scene(
		scene->GetRootNode(),
		ent_iter, 
		mesh_iter, 
		material_iter, 
		light_iter);

	scene->Destroy();
	manager->Destroy();
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <fbx-file>\n";
		return EXIT_FAILURE;
	}
	{
		const boost::filesystem::path filename(argv[1]);
		const auto ext = filename.extension();
		if (ext != ".fbx" && ext != ".FBX")
		{
			std::cout << "You need to supply an FBX file.\n";
			return EXIT_FAILURE;
		}
	}
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
		gt::init_all("fbx_viewer", cam_entity);

		bool view_gbuffers = false;
		
		gt::renderer::set_freeform_cursor(true);

		gt::font::flyweight font_inconsolata(
			"../examples/Inconsolata-Regular.ttf", 
			20);

		gt::fontstream stream;

		boost::filesystem::current_path(gt::get_executable_path());

		std::unique_ptr<gt::actor> the_actor;

		std::vector<gt::entity, gt::allocator<gt::entity>> scene_entities;
		std::vector<gt::static_mesh> scene_meshes;
		std::vector<gt::material, gt::allocator<gt::material>> scene_materials;
		std::vector<std::shared_ptr<gt::ligt>> scene_lights;

		load_fbx_file(
			argv[1],
			std::back_inserter(scene_entities),
			std::back_inserter(scene_meshes),
			std::back_inserter(scene_materials),
			std::back_inserter(scene_lights));

		std::vector<std::unique_ptr<gt::light>> lights;
		std::vector<gt::SQT> light_transforms(4);
		std::vector<gt::material> light_materials;

		{
			gt::vec4f attenuation(1.0f, 0.0f, 1.0f, 1.0f);

			lights.emplace_back(new gt::point_light(gt::vec4f(1.0f, 0.0f, 0.0f, 1.0f), attenuation));
			lights.emplace_back(new gt::point_light(gt::vec4f(0.0f, 1.0f, 0.0f, 1.0f), attenuation));
			lights.emplace_back(new gt::point_light(gt::vec4f(0.0f, 0.0f, 1.0f, 1.0f), attenuation));
			lights.emplace_back(new gt::directional_light(gt::vec4f(0.8f, 0.8f, 0.8f, 1.0f)));

			light_materials.emplace_back(gt::vec4f(1.0f, 0.0f, 0.0f, 0.0f));
			light_materials.emplace_back(gt::vec4f(0.0f, 1.0f, 0.0f, 0.0f));
			light_materials.emplace_back(gt::vec4f(0.0f, 0.0f, 1.0f, 0.0f));
		}
		
		light_transforms[0].scale = 0.1f;
		light_transforms[1].scale = 0.1f;
		light_transforms[2].scale = 0.1f;
		
		light_transforms[0].translation = gt::vec3f( 0.0f, 2.0f, 0.0f);
		light_transforms[1].translation = gt::vec3f( 1.0f, 1.2f, 0.0f);
		light_transforms[2].translation = gt::vec3f(-1.0f, 1.2f, 0.0f);
		light_transforms[3].rotation = gt::quatf::axis_angle(
			gt::vec3f(1.0f, 0.0f, 0.0f),
			gt::deg2rad(10) + static_cast<float>(-M_PI) / 2.0f);

		gt::opengl::unit_cube_PUNTB cube;
		gt::SQT cube_transform;
		cube_transform.scale = 0.5f;
		gt::material cube_material
		(
			{1.0f, 1.0f, 1.0f, 1.0f},
			{1.0f, 1.0f, 1.0f, 2.0f},
			"../../examples/RuralBrickWall.jpg",
			"../../examples/RuralBrickWall_SPEC.png",
			"../../examples/RuralBrickWall_NRM.png"
		);

		gt::renderer::show();

		float curtime, dt;
		float speed = 1.0f;
		float addspeed = 0.0f;
		// gt::vec3f rotation_axis;
		gt::vec2f mousedelta;
		
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

			for (const auto& m : scene_meshes)
			{
				for (const auto& ent : m)
				{
					renderer::set_model_matrix(ent);
					ent.material_component->bind();
					m.draw();
				}
			}

			for (const auto& ent : scene_entities)
			{
				if (ent.mesh_component && ent.material_component)
				{
					renderer::set_model_matrix(ent);
					ent.material_component->bind();
					ent.mesh_component->draw();
				}
			}
			
			// for (std::size_t i = 0; i < light_materials.size(); ++i)
			// {
			// 	const auto numlights = static_cast<float>(light_materials.size());
			// 	const auto radius = 3.0f;
			// 	const auto elevation = 0.0f;
				
			// 	light_transforms[i].translation.x = radius 
			// 		* std::cos(speed * curtime + 2.0f * float(i) * static_cast<float>(M_PI) / numlights);
				
			// 	light_transforms[i].translation.y = elevation;
				
			// 	light_transforms[i].translation.z = radius 
			// 		* std::sin(speed * curtime + 2.0f * float(i) * static_cast<float>(M_PI) / numlights);
				
			// 	gt::renderer::set_model_matrix(light_transforms[i]);
				
			// 	light_materials[i]->bind();
				
			// 	gt::renderer::get_unit_sphere_P().draw();
			// }

			if (view_gbuffers)
			{
				stream.open(font_inconsolata);
				gt::renderer::blit_drawbuffers_to_screen(stream);
				stream.close();
			}
			else
			{
				gt::renderer::begin_light_pass();
				gt::renderer::null_light_pass();

				for (const auto& ent : scene_entities)
				{
					if (ent.light_component)
					{
						ent.light_component->shine(ent.global_transform());
					}
				}

				// for (std::size_t i = 0; i < lights.size(); ++i)
				// {
				// 	lights[i]->shine(light_transforms[i]);
				// }
				
				#ifdef ENABLE_DEBUG_STREAM
				gt::renderer::cerr() << "Move around with WASD.\n"
					<< "Look around with the mouse.\n"
					<< "Go up by holding the spacebar.\n"
					<< "Pressing -/+ will decrease/increase the speed of the balls.\n"
					<< "Press Q to quit.\n"
					<< "Camera position: " << std::fixed << std::setprecision(1) 
					<< gt::renderer::camera().position << '\n';
				#endif
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