#pragma once

#include "ForwardDeclarations.hpp"

#include <memory>
#include <vector>

namespace gintonic {

class FbxImporter
{
public:

	FbxImporter();

	~FbxImporter() noexcept;

	std::shared_ptr<Entity> loadFromFile(const char* filename);

private:

	struct ResultStructure
	{
		std::vector<std::shared_ptr<Material>> materials;
		std::vector<std::shared_ptr<Mesh>> meshes;
		std::vector<std::shared_ptr<Light>> lights;
		std::vector<std::shared_ptr<Camera>> cameras;
	};

	FBXSDK_NAMESPACE::FbxManager* mManager;
	FBXSDK_NAMESPACE::FbxScene* mScene = nullptr;
	FBXSDK_NAMESPACE::FbxImporter* mImporter;

	std::shared_ptr<Entity> traverse(FBXSDK_NAMESPACE::FbxNode*, ResultStructure&);
	std::shared_ptr<Material> processMaterial(FBXSDK_NAMESPACE::FbxNode*, ResultStructure&);
	std::shared_ptr<Mesh> processMesh(FBXSDK_NAMESPACE::FbxNode*, ResultStructure&);
	std::shared_ptr<Camera> processCamera(FBXSDK_NAMESPACE::FbxNode*, ResultStructure&);
	std::shared_ptr<Light> processLight(FBXSDK_NAMESPACE::FbxNode*, ResultStructure&);
};

} // namespace gintonic