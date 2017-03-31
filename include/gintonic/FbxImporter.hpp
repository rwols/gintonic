#pragma once

#include <gintonic/ForwardDeclarations.hpp>

#include <memory>
#include <string>
#include <vector>

namespace gintonic
{

class FbxImporter
{
  public:
    FbxImporter(const char* filename, bool normalizeUnits = true,
                bool setAxisSystemToOpenGL = true,
                bool triangulateAllMeshes = true);

    ~FbxImporter() noexcept;

    std::shared_ptr<Entity>
    loadEntities(bool setRootEntityNameAsFilename = true,
                 bool setSkeletonNameAsFilename = true,
                 bool cleanUpEmptyNodes = true);

    std::shared_ptr<Entity>
    loadFromFile(const char* filename, bool setRootEntityNameAsFilename = true);

  private:
    struct ResultStructure
    {
        std::vector<std::shared_ptr<Material>> materials;
        std::vector<std::shared_ptr<Mesh>> meshes;
        std::vector<std::shared_ptr<Light>> lights;
        std::vector<std::shared_ptr<Camera>> cameras;
        std::vector<std::shared_ptr<AnimationClip>> animations;
        std::shared_ptr<Skeleton> skeleton;
    };

    FBXSDK_NAMESPACE::FbxManager* mManager;
    FBXSDK_NAMESPACE::FbxScene* mScene = nullptr;
    FBXSDK_NAMESPACE::FbxImporter* mImporter;
    std::string mFilename;

    std::shared_ptr<Skeleton> loadSkeleton();
    void processAnimations(ResultStructure&);
    std::shared_ptr<Entity> traverse(FBXSDK_NAMESPACE::FbxNode*,
                                     ResultStructure&);
    std::shared_ptr<Material> processMaterial(FBXSDK_NAMESPACE::FbxNode*,
                                              ResultStructure&);
    std::shared_ptr<Mesh> processMesh(FBXSDK_NAMESPACE::FbxNode*,
                                      ResultStructure&);
    std::shared_ptr<Camera> processCamera(FBXSDK_NAMESPACE::FbxNode*,
                                          ResultStructure&);
    std::shared_ptr<Light> processLight(FBXSDK_NAMESPACE::FbxNode*,
                                        ResultStructure&);
};

} // namespace gintonic
