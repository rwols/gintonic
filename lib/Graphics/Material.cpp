#include <fbxsdk.h>
#include <gintonic/Foundation/exception.hpp>
#include <gintonic/Graphics/Material.hpp>
#include <gintonic/Graphics/OpenGL/ShaderProgram.hpp>

using namespace gintonic;

namespace
{ // anonymous namespace

std::size_t textureCount(const FbxSurfaceMaterial* mat, const char* property)
{
    const auto prop = mat->FindProperty(property);
    return prop.GetSrcObjectCount<FbxFileTexture>();
}

FbxTexture* getFbxTexture(const FbxSurfaceMaterial* pFbxMaterial,
                          const char* property, const std::size_t index)
{
    const auto prop = pFbxMaterial->FindProperty(property);
    return FbxCast<FbxTexture>(prop.GetSrcObject<FbxTexture>(index));
}

float getTextureFactor(const FbxSurfaceMaterial* mat, const char* property)
{
    const auto prop = mat->FindProperty(property);
    return static_cast<float>(prop.Get<FbxDouble>());
}

gintonic::vec3f getMaterialColor(const FbxSurfaceMaterial* mat,
                                 const char* property)
{
    const auto prop = mat->FindProperty(property);
    const auto fbx_color = prop.Get<FbxDouble3>();
    return gintonic::vec3f(static_cast<float>(fbx_color[0]),
                           static_cast<float>(fbx_color[1]),
                           static_cast<float>(fbx_color[2]));
}

} // anonymous namespace

void Material::bind() const
{
    if (!program) return;
    program->activate();
    for (const auto& pair : mTextures)
    {
        const auto textureUnit = pair.second.first;
        const auto& texture = pair.second.second;
        texture->bind(textureUnit);
        program->setUniform(pair.first.c_str(), textureUnit);
    }
    for (const auto& pair : mFloats)
    {
        program->setUniform(pair.first.c_str(), pair.second);
    }
    for (const auto& pair : mVec2s)
    {
        program->setUniform(pair.first.c_str(), pair.second);
    }
    for (const auto& pair : mVec3s)
    {
        program->setUniform(pair.first.c_str(), pair.second);
    }
    for (const auto& pair : mVec4s)
    {
        program->setUniform(pair.first.c_str(), pair.second);
    }
    for (const auto& pair : mMat4s)
    {
        program->setUniform(pair.first.c_str(), pair.second);
    }
}

// Material::Material(const vec4f& diffuseColor)
//     : diffuseColor(diffuseColor), specularColor(0.0f, 0.0f, 0.0f, 0.0f)
// {
//     /* Empty on purpose. */
// }

// Material::Material(const vec4f& diffuseColor, const vec4f& specularColor)
//     : diffuseColor(diffuseColor), specularColor(specularColor)
// {
//     /* Empty on purpose. */
// }

// Material::Material(const vec4f& diffuseColor, const vec4f& specularColor,
//                    const boost::filesystem::path& diffuse_texture)
//     : diffuseColor(diffuseColor), specularColor(specularColor)
// {
//     Texture2D::ImageLoadOptions options;
//     options.relativeFilename = diffuse_texture.string();
//     diffuseTexture = Texture2D::fromImage(options);
// }

// Material::Material(const vec4f& diffuseColor, const vec4f& specularColor,
//                    const boost::filesystem::path& diffuse_texture,
//                    const boost::filesystem::path& specular_texture)
//     : diffuseColor(diffuseColor), specularColor(specularColor)
// {
//     Texture2D::ImageLoadOptions options;
//     options.relativeFilename = diffuse_texture.string();
//     diffuseTexture = Texture2D::fromImage(options);
//     options.relativeFilename = specular_texture.string();
//     specularTexture = Texture2D::fromImage(options);
// }

// Material::Material(const vec4f& diffuseColor, const vec4f& specularColor,
//                    const boost::filesystem::path& diffuse_texture,
//                    const boost::filesystem::path& specular_texture,
//                    const boost::filesystem::path& normal_texture)
//     : diffuseColor(diffuseColor), specularColor(specularColor)
// {
//     Texture2D::ImageLoadOptions options;
//     options.relativeFilename = diffuse_texture.string();
//     diffuseTexture = Texture2D::fromImage(options);
//     options.relativeFilename = specular_texture.string();
//     specularTexture = Texture2D::fromImage(options);
//     options.relativeFilename = normal_texture.string();
//     normalTexture = Texture2D::fromImage(options);
// }

// Material::Material(const vec4f& diffuseColor, const vec4f& specularColor,
//                    std::shared_ptr<Texture2D> diffuse_texture,
//                    std::shared_ptr<Texture2D> specular_texture,
//                    std::shared_ptr<Texture2D> normal_texture)
//     : diffuseColor(diffuseColor), specularColor(specularColor),
//       diffuseTexture(std::move(diffuse_texture)),
//       specularTexture(std::move(specular_texture)),
//       normalTexture(std::move(normal_texture))
// {
//     /* Empty on purpose. */
// }

std::shared_ptr<Material>
Material::fromFbxMaterial(const FbxSurfaceMaterial* pFbxMaterial)
{
    std::cerr << "\tFound Material: " << pFbxMaterial->GetName() << '\n';

    const auto lNumDiffuseTextures =
        textureCount(pFbxMaterial, FbxSurfaceMaterial::sDiffuse);
    const auto lNumSpecularTextures =
        textureCount(pFbxMaterial, FbxSurfaceMaterial::sSpecular);
    const auto lNumNormalTextures =
        textureCount(pFbxMaterial, FbxSurfaceMaterial::sNormalMap);

    const auto lDiffuseFactor =
        getTextureFactor(pFbxMaterial, FbxSurfaceMaterial::sDiffuseFactor);
    const auto lSpecularFactor =
        getTextureFactor(pFbxMaterial, FbxSurfaceMaterial::sSpecularFactor);
    const auto lShininess =
        getTextureFactor(pFbxMaterial, FbxSurfaceMaterial::sShininess);

    diffuseColor =
        vec4f(getMaterialColor(pFbxMaterial, FbxSurfaceMaterial::sDiffuse),
              lDiffuseFactor);
    specularColor = vec4f(
        getMaterialColor(pFbxMaterial, FbxSurfaceMaterial::sSpecular), 1.0f);

    specularColor *= lSpecularFactor;
    specularColor.w = lShininess;

    std::cerr << "\tDiffuse color: " << diffuseColor << '\n';
    std::cerr << "\tSpecular color: " << specularColor << '\n';

    if (lNumDiffuseTextures)
    {
        auto pFbxTexture =
            getFbxTexture(pFbxMaterial, FbxSurfaceMaterial::sDiffuse, 0);
        diffuseTexture = Texture2D::fromFbxTexture(pFbxTexture);
        std::cerr << "\tDiffuse texture: " << diffuseTexture->name << '\n';
    }
    else
    {
        std::cerr << "\tNo diffuse texture present.\n";
    }
    if (lNumSpecularTextures)
    {
        auto pFbxTexture =
            getFbxTexture(pFbxMaterial, FbxSurfaceMaterial::sSpecular, 0);
        specularTexture = Texture2D::fromFbxTexture(pFbxTexture);
        std::cerr << "\tSpecular texture: " << specularTexture->name << '\n';
    }
    else
    {
        std::cerr << "\tNo specular texture present.\n";
    }
    if (lNumNormalTextures)
    {
        auto pFbxTexture =
            getFbxTexture(pFbxMaterial, FbxSurfaceMaterial::sNormalMap, 0);
        normalTexture = Texture2D::fromFbxTexture(pFbxTexture);
        std::cerr << "\tNormal texture: " << normalTexture->name << '\n';
    }
    else
    {
        std::cerr << "\tNo normal map present.\n";
    }
}

std::ostream& operator<<(std::ostream& os, const Material& lMaterial)
{
    return os << "<gintonic::Material at " << &lMaterial << '>';
}
