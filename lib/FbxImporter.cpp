#include "FbxImporter.hpp"
#include "entity.hpp"
#include "materials.hpp"
#include "camera.hpp"
#include "lights.hpp"
#include "mesh.hpp"
#include "exception.hpp"

namespace gintonic {

FbxImporter::FbxImporter()
: mManager(FbxManager::Create())
, mScene(FbxScene::Create(mManager, ""))
, mImporter(::FbxImporter::Create(mManager, ""))
{

}

FbxImporter::~FbxImporter()
{
	mManager->Destroy();
}

FbxImporter::ResultStructure FbxImporter::loadFromFile(const char* filename)
{
	// Import the scene.
	const auto lStatus = mImporter->Initialize(filename, -1, mManager->GetIOSettings());
	if (!lStatus)
	{
		exception lException(filename);
		lException.append(": ");
		lException.append(mImporter->GetStatus().GetErrorString());
		throw lException;
	}
	mImporter->Import(mScene);

	// Convert the scene to the correct axis system and orientation.
	// Also triangulate everything in the scene in-place, just in case.
	FbxAxisSystem::OpenGL.ConvertScene(mScene);
	FbxGeometryConverter lConverter(mManager);
	lConverter.Triangulate(mScene, true);

	ResultStructure lResult;
	traverse(mScene->GetRootNode(), nullptr, lResult);
	return lResult;
}

void FbxImporter::traverse(FbxNode* pNode, std::shared_ptr<entity> parent, ResultStructure& result)
{
	auto lNewEntity = Object<entity>::create(pNode);
	result.entities.push_back(lNewEntity);

	std::cerr << "\nFound FBX Node: " << lNewEntity->getName() << "\n\n";
	std::cerr << "\tLocal Transform: " << lNewEntity->local_transform() << '\n';

	if (!parent)
	{
		if (result.rootEntity)
		{
			std::cerr << "WARNING: Scene contains multiple root entities.\n";
		}

		// This is the root entity of the scene.
		result.rootEntity = lNewEntity;
	}
	else
	{
		lNewEntity->set_parent(parent);
		std::cerr << "\tParent node: " << parent->getName() << '\n';
	}

	if (pNode->GetMesh())
	{
		std::shared_ptr<Mesh> lMesh;
		for (std::size_t i = 0; i < result.meshes.size(); ++i)
		{
			if (result.meshes[i]->getLocalName() == pNode->GetMesh()->GetName())
			{
				lMesh = result.meshes[i];
				break;
			}
		}
		if (!lMesh)
		{
			lMesh = Object<Mesh>::create(pNode->GetMesh());
			result.meshes.push_back(lMesh);
		}

		lNewEntity->mesh = lMesh;

		if (pNode->GetMaterialCount())
		{
			std::shared_ptr<Material> lMaterial;
			for (std::size_t i = 0; i < result.materials.size(); ++i)
			{
				if (result.materials[i]->getLocalName() == pNode->GetMaterial(0)->GetName())
				{
					lMaterial = result.materials[i];
					break;
				}
			}
			if (!lMaterial)
			{
				lMaterial = Object<Material>::create(pNode->GetMaterial(0));
				result.materials.push_back(lMaterial);
			}

			lNewEntity->material = lMaterial;
		}
		else
		{
			exception lException("Mesh with name ");
			lException.append(lMesh->getName());
			lException.append(" has no material attached to it.");
			throw lException;
		}

		std::cerr << "\tMesh: " << lNewEntity->mesh->getName() << '\n';
		std::cerr << "\tMaterial: " << lNewEntity->material->getName() << '\n';
	}
	if (pNode->GetLight())
	{
		std::shared_ptr<Light> lLight;
		for (std::size_t i = 0; i < result.lights.size(); ++i)
		{
			if (result.lights[i]->getLocalName() == pNode->GetLight()->GetName())
			{
				lLight = result.lights[i];
				break;
			}
		}

		if (!lLight)
		{
			lLight = Light::create(pNode->GetLight());
			result.lights.push_back(lLight);
		}

		lNewEntity->light = lLight;

		std::cerr << "\tLight: " << lNewEntity->light->getName() << '\n';
	}
	if (pNode->GetCamera())
	{
		std::shared_ptr<Camera> lCamera;
		for (std::size_t i = 0; i < result.cameras.size(); ++i)
		{
			std::string lFullName;
			if (result.cameras[i]->getLocalName() == pNode->GetCamera()->GetName())
			{
				lCamera = result.cameras[i];
				break;
			}
		}

		if (!lCamera)
		{
			lCamera = Object<Camera>::create(pNode->GetCamera());
			result.cameras.push_back(lCamera);
		}

		lNewEntity->camera = lCamera;

		std::cerr << "\tCamera: " << lNewEntity->camera->getName() << '\n';

	}
	for (int i = 0; i < pNode->GetChildCount(); ++i)
	{
		traverse(pNode->GetChild(i), lNewEntity, result);
	}
}

} // namespace gintonic