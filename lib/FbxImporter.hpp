#pragma once

#include <memory>
#include <vector>
#include <fbxsdk.h>

namespace gintonic {

class entity;   // Forward declaration
class Material; // Forward declaration
class Mesh;     // Forward declaration
class Light;    // Forward declaration
class Camera;   // Forward declaration

class FbxImporter
{
public:

	struct ResultStructure
	{
		std::vector<std::shared_ptr<entity>> entities;
		std::vector<std::shared_ptr<Material>> materials;
		std::vector<std::shared_ptr<Mesh>> meshes;
		std::vector<std::shared_ptr<Light>> lights;
		std::vector<std::shared_ptr<Camera>> cameras;
		std::shared_ptr<entity> rootEntity;
	};

	FbxImporter();

	~FbxImporter();

	ResultStructure loadFromFile(const char* filename);

private:

	FbxManager* mManager;
	FbxScene* mScene = nullptr;
	::FbxImporter* mImporter;
	void traverse(FbxNode*, std::shared_ptr<entity>, ResultStructure&);
};

} // namespace gintonic