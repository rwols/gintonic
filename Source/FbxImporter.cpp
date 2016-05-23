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

FbxImporter::FbxImporter(const char* filename,
	bool normalizeUnits, 
	bool setAxisSystemToOpenGL,
	bool triangulateAllMeshes)
: mManager(FbxManager::Create())
, mScene(FbxScene::Create(mManager, ""))
, mImporter(::FbxImporter::Create(mManager, ""))
, mFilename(filename)
{
	const auto lStatus = mImporter->Initialize(filename, -1, mManager->GetIOSettings());
	if (!lStatus)
	{
		exception lException(filename);
		lException.append(": ");
		lException.append(mImporter->GetStatus().GetErrorString());
		throw lException;
	}
	mImporter->Import(mScene);

	if (setAxisSystemToOpenGL)
	{
		// Convert the scene to the correct axis system and orientation.
		// Also triangulate everything in the scene in-place, just in case.
		FbxAxisSystem::OpenGL.ConvertScene(mScene);
		FbxGeometryConverter lConverter(mManager);
		lConverter.Triangulate(mScene, true);
	}

	if (normalizeUnits)
	{
		const FbxSystemUnit::ConversionOptions lConversionOptions = 
		{
			false, /* mConvertRrsNodes */
			true,  /* mConvertAllLimits */
			true,  /* mConvertClusters */
			true,  /* mConvertLightIntensity */
			true,  /* mConvertPhotometricLProperties */
			true   /* mConvertCameraClipPlanes */
		};

		// Convert the scene to meters using the defined options.
		FbxSystemUnit::m.ConvertScene(mScene, lConversionOptions);
	}


}

FbxImporter::~FbxImporter() noexcept
{
	mManager->Destroy();
}

std::shared_ptr<Entity> FbxImporter::loadEntities(
	bool setRootEntityNameAsFilename,
	bool cleanUpEmptyNodes)
{
	ResultStructure lResult;
	auto lRootEntity = traverse(mScene->GetRootNode(), lResult);
	if (setRootEntityNameAsFilename)
	{
		lRootEntity->name = boost::filesystem::path(mFilename).stem().string();
	}
	if (cleanUpEmptyNodes)
	{

	}
	return lRootEntity;
}

std::vector<AnimStack> FbxImporter::loadAnimations()
{
	THROW_NOT_IMPLEMENTED_EXCEPTION();
	// const int lAnimStackCount = mScene->GetSrcObjectCount<FbxAnimStack>();
	// std::vector<AnimStack> lResult;
	// lResult.reserve(lAnimStackCount);
	// for (int i = 0; i < lAnimStackCount; ++i)
	// {
	// 	FbxAnimStack* lAnimStack = (FbxAnimStack*)mScene->GetSrcObject<FbxAnimStack>(i);
	// 	FbxAnimEvaluator* lAnimEvaluator = mScene->GetAnimationEvaluator();
	// 	// lAnimStack->GetName(); // Get the name of the animation if needed
	// 	const int lLayerCount = lAnimStack->GetMemberCount();
	// 	for (int j = 0; j < lLayerCount; ++j)
	// 	{
	// 		FbxAnimLayer* lAnimLayer = (FbxAnimLayer*)lAnimStack->GetMember(j);
			
	// 	}
	// }
	return std::vector<AnimStack>();
}

std::shared_ptr<Entity> FbxImporter::traverse(FbxNode* pNode, ResultStructure& result)
{
	auto lNewEntity = Entity::create(pNode);
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
		lMaterial = Material::create(pNode->GetMaterial(0));
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
		lMesh = Mesh::create(pNode->GetMesh());
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
		lCamera = Camera::create(pNode->GetCamera());
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