#include "FbxImporter.hpp"

#include "Foundation/exception.hpp"

#include "Graphics/Material.hpp"
#include "Graphics/Light.hpp"
#include "Graphics/Mesh.hpp"

#include "Entity.hpp"
#include "Camera.hpp"

#include <fbxsdk.h>

/* anonymous */ namespace {

template <class ForwardIter>
typename ForwardIter::value_type 
checkForDuplicate(
	const FbxNode* currentNode, 
	const FbxObject* nodeAttribute, 
	ForwardIter first, 
	ForwardIter last)
{
	if (std::strcmp(nodeAttribute->GetName(), "") == 0)
	{
		while (first != last)
		{
			if ((*first)->name == currentNode->GetName())
			{
				return *first;
			}
			++first;
		}
	}
	else
	{
		while (first != last)
		{
			if ((*first)->name == nodeAttribute->GetName())
			{
				return *first;
			}
			++first;
		}
	}
	return nullptr;
}

} // anonymous namespace

namespace gintonic {

FbxImporter::FbxImporter()
: mManager(FbxManager::Create())
, mScene(FbxScene::Create(mManager, ""))
, mImporter(::FbxImporter::Create(mManager, ""))
{

}

FbxImporter::~FbxImporter() noexcept
{
	mManager->Destroy();
}

std::shared_ptr<Entity> FbxImporter::loadFromFile(const char* filename, bool setRootEntityNameAsFilename)
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
	auto lRootEntity = traverse(mScene->GetRootNode(), lResult);
	if (setRootEntityNameAsFilename)
	{
		lRootEntity->name = boost::filesystem::path(filename).stem().string();
	}
	return lRootEntity;
}

std::shared_ptr<Entity> FbxImporter::traverse(FbxNode* pNode, ResultStructure& result)
{
	auto lNewEntity = std::make_shared<Entity>(pNode);
	// result.entities.push_back(lNewEntity);

	std::cerr << "\nFound FBX Node: " << lNewEntity->name << "\n\n";
	std::cerr << "\tLocal Transform: " << lNewEntity->localTransform() << '\n';

	lNewEntity->mesh     = processMesh(pNode, result);
	lNewEntity->material = processMaterial(pNode, result);
	lNewEntity->light    = processLight(pNode, result);
	lNewEntity->camera   = processCamera(pNode, result);

	for (int i = 0; i < pNode->GetChildCount(); ++i)
	{
		auto lChildEntity = traverse(pNode->GetChild(i), result);
		std::cerr << "\tAdding child " << lChildEntity->name << " to parent " << lNewEntity->name << '\n';
		lNewEntity->addChild(std::move(lChildEntity));
	}
	return lNewEntity;
}

std::shared_ptr<Material> FbxImporter::processMaterial(FbxNode* pNode, ResultStructure& result)
{
	if (!pNode->GetMaterialCount())
	{
		return nullptr;
	}
	
	auto lMaterial = checkForDuplicate(
		pNode, 
		pNode->GetMaterial(0), 
		result.materials.begin(), 
		result.materials.end());

	if (!lMaterial)
	{
		lMaterial = std::make_shared<Material>(pNode->GetMaterial(0));
	}

	result.materials.push_back(lMaterial);
	
	return lMaterial;
}

std::shared_ptr<Mesh> FbxImporter::processMesh(FbxNode* pNode, ResultStructure& result)
{
	if (!pNode->GetMesh())
	{
		return nullptr;
	}
	
	auto lMesh = checkForDuplicate(
		pNode, 
		pNode->GetMesh(), 
		result.meshes.begin(), 
		result.meshes.end());

	if (!lMesh)
	{
		lMesh = std::make_shared<Mesh>(pNode->GetMesh());
	}

	result.meshes.push_back(lMesh);
	
	return lMesh;
}

std::shared_ptr<Camera> FbxImporter::processCamera(FbxNode* pNode, ResultStructure& result)
{
	if (!pNode->GetCamera())
	{
		return nullptr;
	}
	
	auto lCamera = checkForDuplicate(
		pNode, 
		pNode->GetCamera(), 
		result.cameras.begin(), 
		result.cameras.end());

	if (!lCamera)
	{
		lCamera = std::make_shared<Camera>(pNode->GetCamera());
	}

	result.cameras.push_back(lCamera);
	
	return lCamera;
}

std::shared_ptr<Light> FbxImporter::processLight(FbxNode* pNode, ResultStructure& result)
{
	if (!pNode->GetLight())
	{
		return nullptr;
	}
	
	auto lLight = checkForDuplicate(
		pNode, 
		pNode->GetLight(), 
		result.lights.begin(), 
		result.lights.end());

	if (!lLight)
	{
		lLight = Light::create(pNode->GetLight());
	}

	result.lights.push_back(lLight);
	
	return lLight;
}

} // namespace gintonic