#include "Graphics/Material.hpp"

#include "Foundation/exception.hpp"
#include <eos/portable_iarchive.hpp>
#include <eos/portable_oarchive.hpp>

#include "Graphics/OpenGL/ShaderProgram.hpp"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/export.hpp>
#include <fstream>

namespace gintonic
{

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

Material::Material(const vec4f& diffuseColor)
    : diffuseColor(diffuseColor), specularColor(0.0f, 0.0f, 0.0f, 0.0f)
{
    /* Empty on purpose. */
}

Material::Material(const vec4f& diffuseColor, const vec4f& specularColor)
    : diffuseColor(diffuseColor), specularColor(specularColor)
{
    /* Empty on purpose. */
}

Material::Material(const vec4f& diffuseColor, const vec4f& specularColor,
                   const boost::filesystem::path& diffuse_texture)
    : diffuseColor(diffuseColor), specularColor(specularColor)
{
    Texture2D::ImageLoadOptions options;
    options.relativeFilename = diffuse_texture.string();
    diffuseTexture = Texture2D::fromImage(options);
}

Material::Material(const vec4f& diffuseColor, const vec4f& specularColor,
                   const boost::filesystem::path& diffuse_texture,
                   const boost::filesystem::path& specular_texture)
    : diffuseColor(diffuseColor), specularColor(specularColor)
{
    Texture2D::ImageLoadOptions options;
    options.relativeFilename = diffuse_texture.string();
    diffuseTexture = Texture2D::fromImage(options);
    options.relativeFilename = specular_texture.string();
    specularTexture = Texture2D::fromImage(options);
}

Material::Material(const vec4f& diffuseColor, const vec4f& specularColor,
                   const boost::filesystem::path& diffuse_texture,
                   const boost::filesystem::path& specular_texture,
                   const boost::filesystem::path& normal_texture)
    : diffuseColor(diffuseColor), specularColor(specularColor)
{
    Texture2D::ImageLoadOptions options;
    options.relativeFilename = diffuse_texture.string();
    diffuseTexture = Texture2D::fromImage(options);
    options.relativeFilename = specular_texture.string();
    specularTexture = Texture2D::fromImage(options);
    options.relativeFilename = normal_texture.string();
    normalTexture = Texture2D::fromImage(options);
}

Material::Material(const vec4f& diffuseColor, const vec4f& specularColor,
                   std::shared_ptr<Texture2D> diffuse_texture,
                   std::shared_ptr<Texture2D> specular_texture,
                   std::shared_ptr<Texture2D> normal_texture)
    : diffuseColor(diffuseColor), specularColor(specularColor),
      diffuseTexture(std::move(diffuse_texture)),
      specularTexture(std::move(specular_texture)),
      normalTexture(std::move(normal_texture))
{
    /* Empty on purpose. */
}

std::ostream& operator<<(std::ostream& os, const Material& lMaterial)
{
    os << "{ diffuseColor: " << lMaterial.diffuseColor
       << ", specularColor: " << lMaterial.specularColor;

    if (lMaterial.diffuseTexture)
    {
        os << ", diffuseTexture: " << lMaterial.diffuseTexture->name;
    }
    if (lMaterial.specularTexture)
    {
        os << ", specularTexture: " << lMaterial.specularTexture->name;
    }
    if (lMaterial.normalTexture)
    {
        os << ", normalTexture: " << lMaterial.normalTexture->name;
    }
    os << " }";
    return os;
}

} // namespace gintonic
