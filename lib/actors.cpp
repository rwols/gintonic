#include "actors.hpp"
#include "materials.hpp"
#include "static_mesh.hpp"
#include "shaders.hpp"
#include "lights.hpp"
#include "renderer.hpp"
#include "entity.hpp"
#include <fbxsdk.h>

namespace
{
	std::size_t texture_count(
		const FbxSurfaceMaterial* mat, 
		const char* property)
	{
		const auto prop = mat->FindProperty(property);
		return prop.GetSrcObjectCount<FbxFileTexture>();
	}

	boost::filesystem::path get_texture_filename(
		const FbxSurfaceMaterial* mat, 
		const char* property, 
		const std::size_t index)
	{
		const auto prop = mat->FindProperty(property);
		const auto file_prop = FbxCast<FbxFileTexture>(prop.GetSrcObject<FbxFileTexture>(index));
		return file_prop->GetFileName();
	}

	float get_texture_factor(
		const FbxSurfaceMaterial* mat,
		const char* property)
	{
		const auto prop = mat->FindProperty(property);
		return static_cast<float>(prop.Get<FbxDouble>());
	}

	gintonic::vec3f get_material_color(
		const FbxSurfaceMaterial* mat,
		const char* property)
	{
		const auto prop = mat->FindProperty(property);
		const auto fbx_color = prop.Get<FbxDouble3>();
		return gintonic::vec3f(static_cast<float>(fbx_color[0]), static_cast<float>(fbx_color[1]), static_cast<float>(fbx_color[2]));
	}
}

namespace gintonic {

void actor::think()
{
	/* Empty on purpose. */
}

void actor::draw_geometry()
{
	/* Empty on purpose. */
}
void actor::draw_lights() const BOOST_NOEXCEPT_OR_NOTHROW
{
	/* Empty on purpose. */
}

actor::~actor() BOOST_NOEXCEPT_OR_NOTHROW
{
	/* Empty on purpose. */
}

const box3f& actor::bounds() const BOOST_NOEXCEPT_OR_NOTHROW
{
	return m_bounds;
}

static_model_actor::static_model_actor( 
	FbxScene* scene)
{
	traverse_graph(scene->GetRootNode());	
}

static_model_actor::~static_model_actor() BOOST_NOEXCEPT_OR_NOTHROW
{
	/* Empty on purpose. */
}

void static_model_actor::traverse_graph(
	FbxNode* node)
{
	FbxAMatrix& fbx_transform = node->EvaluateGlobalTransform();
	const auto fbx_scale = fbx_transform.GetS();
	const auto fbx_rotation = fbx_transform.GetQ();
	const auto fbx_translation = fbx_transform.GetT();
	SQT t;
	t.scale = fbx_scale;
	t.rotation.w = static_cast<float>(fbx_rotation[3]);
	t.rotation.x = static_cast<float>(fbx_rotation[0]);
	t.rotation.y = static_cast<float>(fbx_rotation[1]);
	t.rotation.z = static_cast<float>(fbx_rotation[2]);
	t.translation = fbx_translation;
	if (FbxCast<FbxMesh>(node->GetNodeAttribute()))
	{
		std::cerr << "Found mesh: " << node->GetName() << '\n';
		process_mesh(FbxCast<FbxMesh>(node->GetNodeAttribute()), t);

	}
	else if (FbxCast<FbxLight>(node->GetNodeAttribute()))
	{
		std::cerr << "Found light: " << node->GetName() << '\n';
		process_light(FbxCast<FbxLight>(node->GetNodeAttribute()), t);

	}
	else
	{
		std::cerr << "Found unknown node attribute: " << node->GetName() << '\n';
	}

	for (int i = 0; i < node->GetChildCount(); ++i)
	{
		traverse_graph(node->GetChild(i));
	}
}

void static_model_actor::process_mesh(
	FbxMesh* fbx_mesh, 
	const SQT& t)
{
	material mat;
	const auto num_materials = fbx_mesh->GetNode()->GetSrcObjectCount<FbxSurfaceMaterial>();
	if (num_materials)
	{
		const auto fbx_mat         = fbx_mesh->GetNode()->GetSrcObject<FbxSurfaceMaterial>(0);

		std::cout << "\tFound material: " << fbx_mat->GetName() << '\n';
		
		const auto diffuse_count   = texture_count(fbx_mat, FbxSurfaceMaterial::sDiffuse);
		const auto specular_count  = texture_count(fbx_mat, FbxSurfaceMaterial::sSpecular);
		const auto normal_count    = texture_count(fbx_mat, FbxSurfaceMaterial::sNormalMap);
		
		const auto diffuse_factor  = get_texture_factor(fbx_mat, FbxSurfaceMaterial::sDiffuseFactor);
		const auto specular_factor = get_texture_factor(fbx_mat, FbxSurfaceMaterial::sSpecularFactor);
		const auto shininess       = get_texture_factor(fbx_mat, FbxSurfaceMaterial::sShininess);

		const auto diffuse_color   = get_material_color(fbx_mat, FbxSurfaceMaterial::sDiffuse);
		auto specular_color        = get_material_color(fbx_mat, FbxSurfaceMaterial::sSpecular);

		specular_color    *= specular_factor;
		mat.diffuse_color  = vec4f(diffuse_color.x,  diffuse_color.y,  diffuse_color.z,  diffuse_factor);
		mat.specular_color = vec4f(specular_color.x, specular_color.y, specular_color.z, shininess);

		std::cout << "\tDiffuse color: " << mat.diffuse_color << '\n';
		std::cout << "\tSpecular color: " << mat.specular_color << '\n';

		boost::filesystem::path tex_filename;
		if (diffuse_count)
		{
			tex_filename = get_texture_filename(fbx_mat, FbxSurfaceMaterial::sDiffuse, 0);
			mat.set_diffuse_texture(tex_filename);
			std::cout << "\tDiffuse texture: " << tex_filename << '\n';
		}
		else
		{
			std::cout << "\tNo diffuse texture present.\n";
		}
		if (specular_count)
		{
			tex_filename = get_texture_filename(fbx_mat, FbxSurfaceMaterial::sSpecular, 0);
			mat.set_specular_texture(tex_filename);
			std::cout << "\tSpecular texture: " << tex_filename << '\n';
		}
		else
		{
			std::cout << "\tNo specular texture present.\n";
		}
		if (normal_count)
		{
			tex_filename = get_texture_filename(fbx_mat, FbxSurfaceMaterial::sNormalMap, 0);
			mat.set_normal_texture(tex_filename);
			std::cout << "\tNormal texture: " << tex_filename << '\n';
		}
		else
		{
			std::cout << "\tNo normal map present.\n";
		}
	}
	else
	{
		// make a material that does nothing
		std::cerr << fbx_mesh->GetNode()->GetName() << ": Constructing null material.\n";
		mat.diffuse_color  = vec4f(0.8f, 0.8f, 0.8f,  0.9f);
		mat.specular_color = vec4f(0.2f, 0.2f, 0.2f, 20.0f);
	}
	std::unique_ptr<mesh> m(new static_mesh(fbx_mesh));

	model.emplace_back(t, std::move(m), mat);
}

void static_model_actor::process_light( 
	FbxLight* fbx_light,
	const SQT& t)
{
	const auto intensity = static_cast<float>(fbx_light->Intensity.Get());
	const auto fbx_color = fbx_light->Color.Get();
	const vec4f color(static_cast<float>(fbx_color[0]), 
		static_cast<float>(fbx_color[1]), 
		static_cast<float>(fbx_color[2]), 
		intensity);
	std::cerr << "\tColor: " << color << '\n';
	switch (fbx_light->LightType.Get())
	{
		case FbxLight::ePoint:
		{
			std::cerr << "\tPoint light\n";
			vec4f attenuation(0.0f, 0.0f, 0.0f, 0.0f);

			switch (fbx_light->DecayType.Get())
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
			const vec3f direction = t.rotation.direction();
			std::cerr << "\tDirection: " << direction << '\n';
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

void static_model_actor::draw_geometry()
{
	switch (transforms.size())
	{
		case 0: return;
		case 1: draw_geometry_non_instanced(); break;
		default: draw_geometry_instanced();
	}
}

void static_model_actor::draw_lights() const BOOST_NOEXCEPT_OR_NOTHROW
{
	SQT t;

	for (const auto& tuple : lights)
	{
		for (const auto& transform : transforms)
		{
			t = std::get<0>(tuple);
			t.scale *= transform.scale;
			t.rotation *= transform.rotation;
			t.translation += transform.translation;

			// Shine the light given the transform.
			std::get<1>(tuple)->shine(t);
		}
	}
}

void static_model_actor::draw_geometry_instanced()
{
	SQT t;

	std::vector<mat4f, allocator<mat4f>> PVM_matrices;
	std::vector<mat4f, allocator<mat4f>> VM_matrices;
	std::vector<mat3f, allocator<mat3f>> N_matrices;

	const mat4f matrix_V(renderer::camera()->global_transform());
	const mat4f matrix_P(renderer::matrix_P());
	mat4f matrix_VM;

	for (const auto& tuple : model)
	{
		PVM_matrices.clear();
		VM_matrices.clear();
		N_matrices.clear();
		
		for (const auto& transform : transforms)
		{
			t = std::get<0>(tuple);
			t %= transform;

			matrix_VM = matrix_V * mat4f(t);

			VM_matrices.emplace_back(matrix_VM);
			PVM_matrices.emplace_back(matrix_P * matrix_VM);
			N_matrices.emplace_back(matrix_VM.upper_left_33().invert().transpose());
		}

		// Bind the material
		std::get<2>(tuple).bind(true);

		// Draw the mesh
		std::get<1>(tuple)->draw(PVM_matrices, VM_matrices, N_matrices);
	}
}

void static_model_actor::draw_geometry_non_instanced() const BOOST_NOEXCEPT_OR_NOTHROW
{
	SQT t;

	for (const auto& tuple : model)
	{
		t = std::get<0>(tuple);
		t.scale *= transforms[0].scale;
		t.rotation *= transforms[0].rotation;
		t.translation += transforms[0].translation;

		renderer::set_model_matrix(mat4f(t));

		// Bind the material
		std::get<2>(tuple).bind(false);

		// Draw the mesh
		std::get<1>(tuple)->draw();
	}
}

} // namespace gintonic