#include "actors.hpp"
#include "materials.hpp"
#include "mesh.hpp"
#include "shaders.hpp"
#include "lights.hpp"
#include "renderer.hpp"
#include "textures.hpp"

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

void actor::draw_geometry() const BOOST_NOEXCEPT_OR_NOTHROW
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
	sqt_transformf t;
	t.scale = fbx_scale[0];
	t.rotation.w = static_cast<float>(fbx_rotation[3]);
	t.rotation.x = static_cast<float>(fbx_rotation[0]);
	t.rotation.y = static_cast<float>(fbx_rotation[1]);
	t.rotation.z = static_cast<float>(fbx_rotation[2]);
	t.translation[0] = static_cast<float>(fbx_translation[0]);
	t.translation[1] = static_cast<float>(fbx_translation[1]);
	t.translation[2] = static_cast<float>(fbx_translation[2]);
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
	const sqt_transformf& t)
{
	std::shared_ptr<material> mat;
	const auto num_materials = fbx_mesh->GetNode()->GetSrcObjectCount<FbxSurfaceMaterial>();
	if (num_materials)
	{
		const auto fbx_mat = fbx_mesh->GetNode()->GetSrcObject<FbxSurfaceMaterial>(0);
		// const auto emissive_count = texture_count(fbx_mat, FbxSurfaceMaterial::sEmissive);
		// const auto ambient_count = texture_count(fbx_mat, FbxSurfaceMaterial::sAmbient);
		const auto diffuse_count = texture_count(fbx_mat, FbxSurfaceMaterial::sDiffuse);
		const auto specular_count = texture_count(fbx_mat, FbxSurfaceMaterial::sSpecular);
		// const auto bump_count = texture_count(fbx_mat, FbxSurfaceMaterial::sBump);
		// const auto normal_count = texture_count(fbx_mat, FbxSurfaceMaterial::sNormalMap);

		// boost::filesystem::path file_emissive, file_ambient, file_diffuse, file_specular, file_bump, file_normal;
		// float factor_emissive, factor_ambient, factor_diffuse, factor_specular, factor_bump, shininess;

		if (specular_count && diffuse_count)
		{
			auto diffuse_filename = get_texture_filename(fbx_mat, FbxSurfaceMaterial::sDiffuse, 0);
			auto specular_filename = get_texture_filename(fbx_mat, FbxSurfaceMaterial::sSpecular, 0);
			auto diffuse_factor = get_texture_factor(fbx_mat, FbxSurfaceMaterial::sDiffuseFactor);
			auto shininess = get_texture_factor(fbx_mat, FbxSurfaceMaterial::sShininess);
			std::cerr << fbx_mesh->GetNode()->GetName() << ": Constructing specular_diffuse_material.\n";
			mat = std::make_shared<specular_diffuse_material>(diffuse_filename, specular_filename, diffuse_factor, shininess);
		}
		else if (diffuse_count)
		{
			auto diffuse_filename = get_texture_filename(fbx_mat, FbxSurfaceMaterial::sDiffuse, 0);
			auto diffuse_factor = get_texture_factor(fbx_mat, FbxSurfaceMaterial::sDiffuseFactor);
			std::cerr << fbx_mesh->GetNode()->GetName() << ": Constructing diffuse_material.\n";
			mat = std::make_shared<diffuse_material>(diffuse_filename, diffuse_factor);
		}
		else
		{
			// As a last resort: does it have a diffuse color?
			const auto diffuse_color = get_material_color(fbx_mat, FbxSurfaceMaterial::sDiffuse);
			auto diffuse_factor = get_texture_factor(fbx_mat, FbxSurfaceMaterial::sDiffuseFactor);
			const vec4f color(diffuse_color[0], diffuse_color[1], diffuse_color[2], diffuse_factor);
			std::cerr << fbx_mesh->GetNode()->GetName() << ": Constructing material_c.\n";
			mat = std::make_shared<material_c>(color);
		}
	}
	else
	{
		// make a material that does nothing
		std::cerr << fbx_mesh->GetNode()->GetName() << ": Constructing material.\n";
		mat = std::make_shared<material>();
	}
	std::shared_ptr<mesh> m = std::make_shared<mesh>(fbx_mesh);
	model.emplace_back(t, m, mat);
}

void static_model_actor::process_light( 
	FbxLight* fbx_light,
	const sqt_transformf& t)
{
	const auto intensity = static_cast<float>(fbx_light->Intensity.Get());
	const auto fbx_color = fbx_light->Color.Get();
	const vec4f color(static_cast<float>(fbx_color[0]), 
		static_cast<float>(fbx_color[1]), 
		static_cast<float>(fbx_color[2]), 
		intensity);
	std::cerr << "Color: " << color << '\n';
	switch (fbx_light->LightType.Get())
	{
		case FbxLight::ePoint:
		{
			std::cerr << "Point light\n";
			vec4f attenuation(0.0f, 0.0f, 0.0f, 0.0f);

			switch (fbx_light->DecayType.Get())
			{
				case FbxLight::eNone:
				{
					attenuation[0] = 1.0f;
					break;
				}
				case FbxLight::eLinear:
				{
					attenuation[1] = 1.0f;
					break;
				}
				case FbxLight::eQuadratic:
				{
					attenuation[2] = 1.0f;
					break;
				}
				case FbxLight::eCubic:
				{
					attenuation[3] = 1.0f;
					break;
				}
				default:
				{
					attenuation[2] = 1.0f; // quadratic is default
				}
			}
			std::cerr << "Attenuation: " << attenuation << '\n';
			break;
		}
		case FbxLight::eDirectional:
		{
			std::cerr << "Directional light\n";
			const vec3f direction = t.rotation.direction();
			std::cerr << "Direction: " << direction << '\n';
			break;
		}
		case FbxLight::eSpot:
		{
			std::cerr << "Spot light\n";
			break;
		}
		case FbxLight::eArea:
		{
			std::cerr << "Area light\n";
			break;
		}
		case FbxLight::eVolume:
		{
			std::cerr << "Volume light\n";
			break;
		}
		default:
		{
			std::cerr << "Unknown light\n";
			break;
		}
	}
	// throw std::runtime_error("static_model_actor::process_light: not yet implemented.");
}

void static_model_actor::draw_geometry() const BOOST_NOEXCEPT_OR_NOTHROW
{
	sqt_transformf t;
	std::shared_ptr<const mesh> m;
	std::shared_ptr<const material> mat;
	
	for (const auto& tup : model)
	{
		std::tie(t, m, mat) = tup;

		t.scale *= transform.scale;
		t.rotation *= transform.rotation;
		t.translation += transform.translation;

		renderer::set_model_matrix(t.get_matrix());

		// mat->program->activate();

		// mat->program->bind_matrices();

		// mat->program->bind_material(mat);

		mat->bind();

		m->draw();
	}
}

void static_model_actor::draw_lights() const BOOST_NOEXCEPT_OR_NOTHROW
{
	sqt_transformf t;
	std::shared_ptr<const light> l;
	for (const auto& tup : lights)
	{
		std::tie(t, l) = tup;

		t.scale *= transform.scale;
		t.rotation *= transform.rotation;
		t.translation += transform.translation;

		l->shine(t);
	}
}

} // namespace gintonic