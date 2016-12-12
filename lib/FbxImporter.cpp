#include "FbxImporter.hpp"

#include "Foundation/exception.hpp"

#include "Graphics/AnimationClip.hpp"
#include "Graphics/Material.hpp"
#include "Graphics/Light.hpp"
#include "Graphics/Mesh.hpp"
#include "Graphics/Skeleton.hpp"

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
		FbxAxisSystem::OpenGL.ConvertScene(mScene);
	}

	if (triangulateAllMeshes)
	{
		// Triangulate everything in the scene in-place.
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

void cleanUp(std::shared_ptr<Entity> entity)
{
	for (auto lChild : *entity)
	{
		if (!lChild->mesh && !lChild->material && !lChild->light)
		{
			entity->removeChild(lChild);
		}
	}
	// Recursive into the non-empty ones
	for (auto lChild : *entity)
	{
		cleanUp(lChild);
	}
}

std::shared_ptr<Entity> FbxImporter::loadEntities(
	bool setRootEntityNameAsFilename,
	bool setSkeletonNameAsFilename,
	bool cleanUpEmptyNodes)
{

	ResultStructure lResult;
	lResult.skeleton = loadSkeleton();
	if (lResult.skeleton)
	{
		std::cerr << "Found skeleton \"" << lResult.skeleton->name << "\" with " << lResult.skeleton->joints.size() << " joints.\n";
	}
	processAnimations(lResult);
	if (lResult.animations.empty())
	{
		std::cerr << "No animation clips present.\n";
	}
	else
	{
		for (const auto& lClip : lResult.animations)
		{
			std::cerr << "Found animation clip \"" << lClip->name << "\", " << lClip->frames[0].size() 
				<< " frames, " << lClip->framesPerSecond << " frames/second, ";
			if (!lClip->isLooping) std::cerr << "not ";
			std::cerr << "looping";
			if (lClip->skeleton == nullptr)
			{
				std::cerr << ".\n";
				std::cerr << "WARNING: No target skeleton!\n";
			}
			else
			{
				std::cerr << ", targeting \"" << lClip->skeleton->name << "\".\n";
			}
		}
	}
	auto lRootEntity = traverse(mScene->GetRootNode(), lResult);
	lRootEntity->animationClips = lResult.animations;
	if (lRootEntity->animationClips.empty() == false && lRootEntity->mesh == nullptr)
	{
		for (auto lChildEntity : *lRootEntity)
		{
			if (lChildEntity->mesh && lChildEntity->material)
			{
				lRootEntity->mesh = lChildEntity->mesh;
				lRootEntity->material = lChildEntity->material;
				lChildEntity->mesh = nullptr;
				lChildEntity->material = nullptr;
				break;
			}
		}
	}
	if (setRootEntityNameAsFilename)
	{
		lRootEntity->name = boost::filesystem::path(mFilename).stem().string();
	}
	if (setSkeletonNameAsFilename && lResult.skeleton)
	{
		lResult.skeleton->name = boost::filesystem::path(mFilename).stem().string();
	}
	if (cleanUpEmptyNodes)
	{
		// THROW_NOT_IMPLEMENTED_EXCEPTION();
	}
	return lRootEntity;
}

void FbxImporter::processAnimations(ResultStructure& result)
{
	const int lAnimStackCount = mScene->GetSrcObjectCount<FbxAnimStack>();
	for (int i = 0; i < lAnimStackCount; ++i)
	{
		auto lTheirAnimStack = mScene->GetSrcObject<FbxAnimStack>(i);
		auto lMyAnimationClip = std::shared_ptr<AnimationClip>(new AnimationClip(lTheirAnimStack, result.skeleton));
		result.animations.push_back(lMyAnimationClip);
	}
}

std::shared_ptr<Skeleton> FbxImporter::loadSkeleton()
{
	try
	{
		auto lSkeleton = std::shared_ptr<Skeleton>(new Skeleton(mScene));
		if (lSkeleton->joints.empty() == false) return lSkeleton; 
	}
	catch (const Skeleton::Exception& lException)
	{
		std::cerr << "ERRROR: FBX Node \"" << lException.getFbxNode()->GetName() 
			<< "\" caused an error involving skeleton joints.\n";
		throw;
	}
	return std::shared_ptr<Skeleton>(nullptr);
}

// std::vector<AnimStack> FbxImporter::loadAnimations()
// {
// 	const int lAnimStackCount = mScene->GetSrcObjectCount<FbxAnimStack>();
// 	if (lAnimStackCount <= 0)
// 	{
// 		return std::vector<AnimStack>(); // empty vector.
// 	}
// 	else
// 	{
// 		std::cerr << "\tFound " << lAnimStackCount << " animation stack";
// 		if (lAnimStackCount == 1) std::cerr << ".\n\n";
// 		else std::cerr << "s.\n\n";
// 	}
// 	std::vector<AnimStack> lResult;
// 	lResult.reserve(lAnimStackCount);
// 	FbxAnimEvaluator* lAnimEvaluator = mScene->GetAnimationEvaluator();

// 	for (int i = 0; i < lAnimStackCount; ++i)
// 	{
// 		auto lAnimStack = mScene->GetSrcObject<FbxAnimStack>(i);

// 		// std::cerr << "\tThere";
// 		// if (lAnimStack->GetMemberCount() == 1) std::cerr << " is ";
// 		// else std::cerr << " are ";
// 		// std::cerr << lAnimStack->GetMemberCount() << " layer";
// 		// if (lAnimStack->GetMemberCount() != 1) std::cerr << 's';
// 		// std::cerr << " in animation stack " << i << ", " << lAnimStack->GetName() << ".\n";

// 		FbxTime lFrameRate;
// 		lFrameRate.SetFrame(1, FbxTime::eFrames30);
// 		const auto lStart = lAnimStack->LocalStart.Get();
// 		const auto lStop = lAnimStack->LocalStop.Get();
// 		std::cerr << "Baking layers...\n";
// 		lAnimStack->BakeLayers(lAnimEvaluator, lStart, lStop, lFrameRate);
// 		auto lAnimLayer = lAnimStack->GetMember<FbxAnimLayer>(0);
// 		std::vector<SQT, allocator<SQT>> lAnimationClip;
// 		lAnimationClip.reserve((lStop - lStart).GetFrameCount());
// 		for (FbxTime t = lStart; t != lStop; t += lFrameRate)
// 		{
// 			// const auto& lAffineMatrix = lAnimEvaluator->GetNodeLocalTransform(lJoint, t);
// 			// lAnimationClip.emplace_back(lAffineMatrix);
// 		}

// 		std::cerr << "Animation clip size: " << lAnimationClip.size() << "\n\n";

// 		for (const auto& lSqt : lAnimationClip)
// 		{
// 			std::cerr << lSqt << '\n';
// 		}

// 		assert(false);
		

// 		for (auto lAnimLayer : FbxInCollection<FbxAnimLayer>(lAnimStack))
// 		{
// 			std::cerr << lAnimLayer->GetName() << '\n';
// 			for (auto lAnimCurveNode : FbxInCollection<FbxAnimCurveNode>(lAnimLayer))
// 			{
// 				std::cerr << '\t' << lAnimCurveNode->GetName() << '\n';
// 			}
// 		}
// 		assert(false);

// 		// processAnimationStack(lAnimStack, lAnimEvaluator, lResult);

// 		for (int j = 0; j < lAnimStack->GetMemberCount(); ++j)
// 		{
// 			FbxAnimLayer* lAnimLayer = (FbxAnimLayer*)lAnimStack->GetMember(j);

// 			std::cerr << "\tThere";
// 			if (lAnimLayer->GetMemberCount() == 1) std::cerr << " is ";
// 			else std::cerr << " are ";
// 			std::cerr << lAnimLayer->GetMemberCount() << " animation curves in layer "
// 				<< j << ", " << lAnimLayer->GetName() << ".\n";

// 			for (int k = 0; k < lAnimLayer->GetMemberCount(); ++k)
// 			{
// 				FbxAnimCurveNode* lAnimCurveNode = (FbxAnimCurveNode*)lAnimLayer->GetMember(k);

// 				std::cerr << "\tAnimation curve " << k << " is called \"" << lAnimCurveNode->GetName() << "\".\n";
// 				std::cerr << "\tChannel names:\n";

// 				for (unsigned int l = 0; l < lAnimCurveNode->GetChannelsCount(); ++l)
// 				{
// 					const auto lChannelName = lAnimCurveNode->GetChannelName(l);

// 					std::cerr << "\t\t\"" << lChannelName.Buffer() << "\", " << lAnimCurveNode->GetCurveCount(l) << " curve";
// 					if (lAnimCurveNode->GetCurveCount(l) == 1) std::cerr << ".\n";
// 					else std::cerr << "s.\n";

// 					for (int c = 0; c < lAnimCurveNode->GetCurveCount(l); ++c)
// 					{
// 						const auto lAnimCurve = lAnimCurveNode->GetCurve(l, c);

// 						std::cerr << "\t\t\tCurve " << c << " has " << lAnimCurve->KeyGetCount() << " key";
// 						if (lAnimCurve->KeyGetCount() == 1) std::cerr << ".\n";
// 						else std::cerr << "s.\n";

// 						for (int v = 0; v < lAnimCurve->KeyGetCount(); ++v)
// 						{
// 							const auto lAnimCurveKey = lAnimCurve->KeyGet(v);
// 							const auto lTime         = lAnimCurveKey.GetTime();
// 							const auto lValue        = lAnimCurveKey.GetValue();
// 							std::cerr << "{" << lTime.GetFrameCount() << ", " << lValue << "} ";
// 						}
// 						std::cerr << '\n';
// 					}
// 				}
// 			}
// 		}
// 	}
// 	THROW_NOT_IMPLEMENTED_EXCEPTION();
// 	return std::vector<AnimStack>();
// }

std::shared_ptr<Entity> FbxImporter::traverse(FbxNode* pNode, ResultStructure& result)
{
	if (!pNode) return nullptr;
	if (pNode->GetNodeAttribute() &&
		pNode->GetNodeAttribute()->GetAttributeType() &&
		pNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		// Skip skeletons.
		// They are processed by the Mesh class.
		std::cerr << "\nSkipping FBX Node: " << pNode->GetName() << " (this is a joint of a skeleton).\n\n";
		return nullptr;
	}

	auto lNewEntity = Entity::create(pNode);
	// result.entities.push_back(lNewEntity);

	std::cerr << "\nFound FBX Node: " << lNewEntity->name << "\n\n"
		<< "\tLocal Transform:\n"
		<< "\t        Scale: " << lNewEntity->localTransform().scale << '\n'
		<< "\t     Rotation: " << lNewEntity->localTransform().rotation << '\n'
		<< "\t  Translation: " << lNewEntity->localTransform().translation << '\n';

	lNewEntity->mesh     = processMesh(pNode, result);
	lNewEntity->material = processMaterial(pNode, result);
	lNewEntity->light    = processLight(pNode, result);
	lNewEntity->camera   = processCamera(pNode, result);

	if ((lNewEntity->mesh && lNewEntity->material) || lNewEntity->light)
	{
		lNewEntity->castShadow = true;
	}
	else
	{
		lNewEntity->castShadow = false;
	}

	for (int i = 0; i < pNode->GetChildCount(); ++i)
	{
		auto lChildEntity = traverse(pNode->GetChild(i), result);
		if (lChildEntity)
		{
			std::cerr << "\tAdding child " << lChildEntity->name << " to parent " << lNewEntity->name << '\n';
			lNewEntity->addChild(std::move(lChildEntity));
		}
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

	if (lMaterial)
	{
		std::cerr << "\tReusing existing material \"" << lMaterial->name << "\".\n";
	}
	else
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

	if (lMesh)
	{
		std::cerr << "\tReusing existing mesh \"" << lMesh->name << "\".\n";
	}
	else
	{
		if (result.skeleton)
		{
			lMesh = Mesh::create(pNode->GetMesh(), *result.skeleton);
		}
		else
		{
			lMesh = Mesh::create(pNode->GetMesh());	
		}
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

	if (lCamera)
	{
		std::cerr << "\tReusing existing camera \"" << lCamera->name << "\".\n";
	}
	else
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

	if (lLight)
	{
		std::cerr << "\tReusing existing light \"" << lLight->name << "\".\n";
	}
	else
	{
		lLight = Light::create(pNode->GetLight());
	}

	result.lights.push_back(lLight);
	
	return lLight;
}

} // namespace gintonic