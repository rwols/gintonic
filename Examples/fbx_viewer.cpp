#include "gintonic.hpp"
#define FBXSDK_NEW_API
#include <fbxsdk.h>

#define MOVE_SPEED 2.77778f // 10 km/s

namespace gt = gintonic;

template <class FloatType> FloatType get_elapsed_time()
{
	using std::chrono::duration_cast;
	using std::chrono::nanoseconds;
	using gintonic::renderer;
	return static_cast<FloatType>(duration_cast<nanoseconds>(Renderer::elapsed_time()).count()) / FloatType(1e9);
}

template <class FloatType> FloatType get_dt()
{
	using std::chrono::duration_cast;
	using std::chrono::nanoseconds;
	using gintonic::renderer;
	return static_cast<FloatType>(duration_cast<nanoseconds>(Renderer::deltaTime()).count()) / FloatType(1e9);
}

template <class MeshOutputIter>
void handle_mesh(const FbxMesh* fbxmesh, gt::entity& ent, MeshOutputIter mesh_iter)
{
	gt::mesh* new_mesh = new gt::static_mesh(fbxmesh);
	new_mesh->attach(ent);
	assert(ent.mesh_component() == new_mesh);
	*mesh_iter = new_mesh;
	++mesh_iter;
}

template <class MaterialOutputIter>
void handle_material(const FbxSurfaceMaterial* fbxmat, gt::entity& ent, MaterialOutputIter material_iter)
{
	gt::material* new_mat = new gt::material(fbxmat);
	new_mat->attach(ent);
	assert(ent.material_component() == new_mat);
	*material_iter = new_mat;
	++material_iter;
}

template <class LightOutputIter>
void handle_light(FbxLight* fbxlight, gt::entity& ent, LightOutputIter light_iter)
{
	const auto intensity = static_cast<float>(fbxlight->Intensity.Get());
	const auto fbx_color = fbxlight->Color.Get();
	const gt::vec4f color(static_cast<float>(fbx_color[0]), 
		static_cast<float>(fbx_color[1]), 
		static_cast<float>(fbx_color[2]), 
		intensity);
	std::cerr << "\tColor: " << color << '\n';
	switch (fbxlight->LightType.Get())
	{
		case FbxLight::ePoint:
		{
			std::cerr << "\tPoint light\n";
			gt::vec4f attenuation(0.0f, 0.0f, 0.0f, 0.0f);

			switch (fbxlight->DecayType.Get())
			{
				case FbxLight::eNone:
				{
					attenuation.x = 1.0f;
					break;
				}
				case FbxLight::eLinear:
				{
					attenuation.y = 1.0f;
					break;
				}
				case FbxLight::eQuadratic:
				{
					attenuation.z = 1.0f;
					break;
				}
				case FbxLight::eCubic:
				{
					throw std::runtime_error("There's not support for cubic falloff.");
				}
				default:
				{
					attenuation.z = 1.0f; // quadratic is default
				}
			}
			std::cerr << "\tAttenuation: " << attenuation << '\n';
			break;
		}
		case FbxLight::eDirectional:
		{
			std::cerr << "\tDirectional light\n";
			// const gt::vec3f direction = t.rotation.direction();
			// std::cerr << "\tDirection: " << direction << '\n';
			break;
		}
		case FbxLight::eSpot:
		{
			std::cerr << "\tSpot light\n";
			break;
		}
		case FbxLight::eArea:
		{
			std::cerr << "\tArea light\n";
			break;
		}
		case FbxLight::eVolume:
		{
			std::cerr << "\tVolume light\n";
			break;
		}
		default:
		{
			std::cerr << "\tUnknown light\n";
			break;
		}
	}
	// throw std::runtime_error("static_model_actor::process_light: not yet implemented.");
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
	gt::entity*        parentnode,
	EntityOutputIter   ent_iter, 
	MeshOutputIter     mesh_iter, 
	MaterialOutputIter material_iter, 
	LightOutputIter    light_iter)
{
	std::cout << "\nNow inspecting FBX Node: " << node->GetName() << "\n\n";
	gt::SQT sqt;
	sqt.scale = node->EvaluateLocalScaling();
	sqt.rotation = node->EvaluateLocalRotation();
	sqt.translation = node->EvaluateLocalTranslation();
	gt::box3f bbox;
	gt::entity* new_ent = new gt::entity(sqt, bbox, nullptr, parentnode);
	std::cout << "\tCreated entity " << std::hex << new_ent 
		<< ", with parent " << std::hex << parentnode << std::dec << '\n';
	std::cout << "\tLocal SQT: " << sqt << '\n';
	*ent_iter = new_ent;
	++ent_iter;
	if (node->GetMesh())
	{
		handle_mesh(node->GetMesh(), *new_ent, mesh_iter);
		if (node->GetMaterialCount())
		{
			handle_material(node->GetMaterial(0), *new_ent, material_iter);
		}
		else
		{
			throw std::runtime_error("Mesh has no material.");
		}
	}
	if (node->GetLight())
	{
		handle_light(node->GetLight(), *new_ent, light_iter);
	}
	for (int i = 0; i < node->GetChildCount(); ++i)
	{
		traverse_scene(node->GetChild(i), new_ent, ent_iter, mesh_iter, material_iter, light_iter);
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
	const auto status = importer->Initialize(filepath, -1, manager->GetIOSettings());
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

	// We don't need the importer anymore.
	importer->Import(scene);
	importer->Destroy();

	// Convert the scene to the correct axis system and orientation.
	// Also triangulate everything in the scene in-place, just in case.
	FbxAxisSystem::OpenGL.ConvertScene(scene);
	FbxGeometryConverter converter(manager);
	converter.Triangulate(scene, true);

	// So at this point we have the FbxScene and we can traverse it
	// for entities, lights, materials and meshes.
	traverse_scene(scene->GetRootNode(), nullptr, ent_iter, mesh_iter, material_iter, light_iter);

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
	gt::proj_info projection_component;
	gt::camera cam_component;
	gt::entity cam_entity;
	projection_component.attach(cam_entity);
	cam_component.attach(cam_entity);

	// Position the camera
	cam_entity.set_translation(gt::vec3f(-2.78f, 0.83f, 1.17f));
	cam_component.add_mouse(gt::vec2f(-gt::deg2rad(60.0f), -gt::deg2rad(9.0f)));
	try
	{
		gt::init_all("fbx_viewer", cam_entity);

		gt::Renderer::setFreeformCursor(true);

		gt::font::flyweight font_inconsolata(
			"../examples/Inconsolata-Regular.ttf", 
			20);

		gt::fontstream stream;

		boost::filesystem::current_path(gt::get_executable_path());

		// This data will probably be encapsulated
		// in some sort of "world" or "level" class,
		// along with all the other possible component classes.
		std::vector<gt::entity*> scene_entities;
		std::vector<gt::material*> scene_materials;
		std::vector<gt::mesh*> scene_meshes;
		std::vector<gt::light*> scene_lights;

		load_fbx_file(argv[1],
			std::back_inserter(scene_entities),
			std::back_inserter(scene_meshes),
			std::back_inserter(scene_materials),
			std::back_inserter(scene_lights));

		// If there are no lights in the loaded FBX scene,
		// we create a directional light that shines
		// downwards, otherwise we don't see anything...
		if (scene_lights.empty())
		{
			std::cout << "\nThere are no lights in the scene.\n";
			std::cout << "Adding a directional light so we can see something...\n";
			gt::light* l = new gt::directional_light(gt::vec4f(1.0f, 1.0f, 1.0f, 1.0f));
			gt::SQT light_transform;
			light_transform.scale = 1.0f;
			light_transform.rotation = gt::quatf::axis_angle(gt::vec3f(1.0f, 0.0f, 0.0f), gt::deg2rad(10) + static_cast<float>(-M_PI) / 2.0f);
			gt::entity* e = new gt::entity(light_transform, gt::box3f());
			l->attach(*e);
			scene_entities.push_back(e);
			scene_lights.push_back(l);
		}

		std::cout << "\nGrand totals:\n";
		std::cout << "\tEntities:  " << scene_entities.size() << '\n';
		std::cout << "\tMeshes:    " << scene_meshes.size() << '\n';
		std::cout << "\tMaterials: " << scene_materials.size() << '\n';
		std::cout << "\tLights:    " << scene_lights.size() << '\n';

		gt::Renderer::show();

		float curtime, dt;
		gt::vec2f mousedelta;
		bool view_gbuffers = false;
		
		while (!gt::Renderer::shouldClose())
		{
			curtime = get_elapsed_time<float>();
			dt = get_dt<float>();
			if (gt::Renderer::keyTogglePress(SDL_SCANCODE_Q))
			{
				gt::Renderer::close();
			}
			if (gt::Renderer::key(SDL_SCANCODE_W))
			{
				cam_entity.move_forward(dt);
			}
			if (gt::Renderer::key(SDL_SCANCODE_A))
			{
				cam_entity.move_left(dt);
			}
			if (gt::Renderer::key(SDL_SCANCODE_S))
			{
				cam_entity.move_backward(dt);
			}
			if (gt::Renderer::key(SDL_SCANCODE_D))
			{
				cam_entity.move_right(dt);
			}
			if (gt::Renderer::key(SDL_SCANCODE_SPACE))
			{
				cam_entity.move_up(dt);
			}
			if (gt::Renderer::key(SDL_SCANCODE_C))
			{
				cam_entity.move_down(dt);
			}
			if (gt::Renderer::keyTogglePress(SDL_SCANCODE_G))
			{
				view_gbuffers = !view_gbuffers;
			}

			mousedelta = -gt::deg2rad(gt::Renderer::mouse_delta()) / 10.0f;
			cam_component.add_mouse(mousedelta);

			gt::Renderer::begin_geometry_pass();
			glEnable(GL_CULL_FACE);

			// The first entity added should always be the root node of the
			// FBX file. Every entity from the FBX file is attached to this
			// root node, so we can change the orientation of all the entities
			// by just changing the orientation of the root node.

			scene_entities[0]->set_rotation(gt::quatf::axis_angle(gt::vec3f(0.0f, 1.0f, 0.0f), -curtime / 8.0f));

			for (const auto* m : scene_meshes)
			{
				for (const auto* e : *m)
				{
					gt::Renderer::set_model_matrix(*e);
					e->material_component()->bind();
					e->mesh_component()->draw();
				}
			}

			if (view_gbuffers)
			{
				stream.open(font_inconsolata);
				gt::Renderer::blit_drawbuffers_to_screen(stream);
				stream.close();
			}
			else
			{
				gt::Renderer::begin_light_pass();
				gt::Renderer::ambient_light_pass();

				for (const auto* l : scene_lights)
				{
					for (const auto* e : *l)
					{
						e->light_component()->shine(*e);
					}
				}

				glDisable(GL_CULL_FACE);
				gt::Renderer::get_text_shader()->activate();
				gt::Renderer::get_text_shader()->set_color(gt::vec3f(1.0f, 1.0f, 1.0f));
				stream.open(font_inconsolata);
				stream << "Move around with WASD.\n"
					<< "Look around with the mouse.\n"
					<< "Go up by holding the spacebar.\n"
					<< "Press Q to quit.\n"
					<< "Press G to view the G-buffer contents.\n";
				stream.close();
			}
			gt::Renderer::update();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}