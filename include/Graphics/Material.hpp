/**
 * @file Material.hpp
 * @brief Defines a material for rendering.
 * @author Raoul Wols
 */

#pragma once

#include "ForwardDeclarations.hpp"
#include "Foundation/Object.hpp"
#include "Math/mat4f.hpp"
#include "Math/vec2f.hpp"
#include "Math/vec3f.hpp"
#include "Math/vec4f.hpp"
#include "Texture2D.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <list>
#include <unordered_map>

namespace gintonic
{

namespace OpenGL
{
class ShaderProgram;
}

/**
 * @brief A material describes the look of a rendered mesh.
 *
 * @details The material class manages two four-dimensional vectors
 * representing a diffuse color and a specular color. You can view
 * these as the base color value when a diffuse and specular are
 * attached to the material. A material also manages three optional
 * textures, namely a diffuse texture, a specular texture and a normal
 * map.
 *
 * To activate, or bind, a material you call the material::bind method.
 * If the mesh is rendered instanced, you must give the material::bind method
 * a value of true. Otherwise the default value of false is used to signal
 * that we are not rendering an instanced mesh.
 */
class Material : public Object<Material, std::string>, public Asset
{
    GINTONIC_ASSET(Material, "materials", ".mat")

  public:
    using Super = Object<Material, std::string>;

    std::shared_ptr<OpenGL::ShaderProgram> program;

    /**
     * @name Constructors and destructor
     */

    //@{

    template <class... Args> inline static SharedPtr create(Args &&... args)
    {
        return SharedPtr(new Material(std::forward<Args>(args)...));
    }

    /// Destructor.
    virtual ~Material() noexcept = default;

    template <class StringType>
    inline void set(StringType &&name, const float value)
    {
        mFloats[std::forward<StringType>(name)] = value;
    }

    template <class StringType>
    inline void set(StringType &&name, const GLint textureUnit,
                    std::shared_ptr<Texture2D> texture)
    {
        mTextures[std::forward<StringType>(name)] =
            std::make_pair(textureUnit, std::move(texture));
    }

    template <class StringType>
    inline void set(StringType &&name, const vec2f &value)
    {
        mVec2s[std::forward<StringType>(name)] = value;
    }

    template <class StringType>
    inline void set(StringType &&name, const vec3f &value)
    {
        mVec3s[std::forward<StringType>(name)] = value;
    }

    template <class StringType>
    inline void set(StringType &&name, const vec4f &value)
    {
        mVec4s[std::forward<StringType>(name)] = value;
    }

    template <class StringType>
    inline void set(StringType &&name, const mat4f &value)
    {
        mMat4s[std::forward<StringType>(name)] = value;
    }

    template <class StringType, class T>
    std::enable_if_t<std::is_same<T, float>::value, T>
    get(StringType &&name) const noexcept
    {
        const auto iter = mFloats.find(std::forward<StringType>(name));
        return iter == mFloats.end() ? 0.0f : iter->second;
    }

    template <class StringType, class T>
    std::enable_if_t<std::is_same<T, Texture2D>::value,
                     std::pair<GLint, std::shared_ptr<Texture2D>>>
    get(StringType &&name) const noexcept
    {
        const auto iter = mTextures.find(std::forward<StringType>(name));
        return iter == mTextures.end() ? std::make_pair(0, nullptr)
                                       : iter->second;
    }

    template <class StringType, class T>
    std::enable_if_t<std::is_same<T, vec2f>::value, T>
    get(StringType &&name) const noexcept
    {
        const auto iter = mVec2s.find(std::forward<StringType>(name));
        return iter == mVec2s.end() ? vec2f(0.0f, 0.0f) : iter->second;
    }

    template <class StringType, class T>
    std::enable_if_t<std::is_same<T, vec3f>::value, T>
    get(StringType &&name) const noexcept
    {
        const auto iter = mVec3s.find(std::forward<StringType>(name));
        return iter == mVec3s.end() ? vec3f(0.0f, 0.0f, 0.0f) : iter->second;
    }

    template <class StringType, class T>
    std::enable_if_t<std::is_same<T, vec4f>::value, T>
    get(StringType &&name) const noexcept
    {
        const auto iter = mVec4s.find(std::forward<StringType>(name));
        return iter == mVec4s.end() ? vec4f(0.0f, 0.0f, 0.0f, 0.0f)
                                    : iter->second;
    }

    template <class StringType, class T>
    std::enable_if_t<std::is_same<T, mat4f>::value, T>
    get(StringType &&name) const noexcept
    {
        const auto iter = mMat4s.find(std::forward<StringType>(name));
        return iter == mMat4s.end() ? mat4f::zero : iter->second;
    }

    void bind() const;

  private:
    std::unordered_map<std::string, float> mFloats;
    std::unordered_map<std::string,
                       std::pair<GLint, std::shared_ptr<Texture2D>>>
        mTextures;
    std::unordered_map<std::string, vec2f> mVec2s;
    std::unordered_map<std::string, vec3f> mVec3s;
    std::unordered_map<std::string, vec4f> mVec4s;
    std::unordered_map<std::string, mat4f> mMat4s;

    /**
     * @brief Default constructor.
     */
    Material() = default;

    /**
     * @brief Constructor.
     *
     * @param diffuseColor The base diffuse color property.
     */
    Material(const vec4f &diffuseColor);

    /**
     * @brief Constructor.
     *
     * @param diffuseColor The base diffuse color property.
     * @param specularColor The base specular color property.
     */
    Material(const vec4f &diffuseColor, const vec4f &specularColor);

    /**
     * @brief Constructor.
     *
     * @param diffuseColor The base diffuse color property.
     * @param specularColor The base specular color property.
     * @param diffuseTexture The filepath to the diffuse texture.
     */
    Material(const vec4f &diffuseColor, const vec4f &specularColor,
             const boost::filesystem::path &diffuseTexture);

    /**
     * @brief Constructor.
     *
     * @param diffuseColor The base diffuse color property.
     * @param specularColor The base specular color property.
     * @param diffuseTexture The filepath to the diffuse texture.
     * @param specularTexture The filepath to the specular texture.
     */
    Material(const vec4f &diffuseColor, const vec4f &specularColor,
             const boost::filesystem::path &diffuseTexture,
             const boost::filesystem::path &specularTexture);

    /**
     * @brief Constructor.
     *
     * @param diffuseColor The base diffuse color property.
     * @param specularColor The base specular color property.
     * @param diffuseTexture The filepath to the diffuse texture.
     * @param specularTexture The filepath to the specular texture.
     * @param normalTexture The filepath to the normal texture.
     */
    Material(const vec4f &diffuseColor, const vec4f &specularColor,
             const boost::filesystem::path &diffuseTexture,
             const boost::filesystem::path &specularTexture,
             const boost::filesystem::path &normalTexture);

    /**
     * @brief Constructor.
     *
     * @param diffuseColor The base diffuse color property.
     * @param specularColor The base specular color property.
     * @param diffuseTexture Shared pointer to the diffuse texture.
     * @param specularTexture Shared pointer to the specular texture.
     * @param normalTexture Shared pointer to the normal texture.
     */
    Material(const vec4f &diffuseColor, const vec4f &specularColor,
             std::shared_ptr<Texture2D> diffuseTexture,
             std::shared_ptr<Texture2D> specularTexture,
             std::shared_ptr<Texture2D> normalTexture);

    /**
     * @brief Constructor.
     *
     * @details Converts an FBX surface material into our own material.
     * The constructor will output various log information to the standard
     * output stream.
     *
     * @param fbxmat The FBX material.
     */
    Material(const FBXSDK_NAMESPACE::FbxSurfaceMaterial *fbxmat);

  public:
    //@}

    /**
     * @name Property management
     */

    //@{

    /// The diffuse color property.
    vec4f diffuseColor;

    /// The specular color property.
    vec4f specularColor;

    /// The diffuse texture.
    std::shared_ptr<Texture2D> diffuseTexture;

    /// The specular texture.
    std::shared_ptr<Texture2D> specularTexture;

    /// The normal texture.
    std::shared_ptr<Texture2D> normalTexture;

    /**
     * @brief Debug stream output.
     * @param os The output stream.
     * @param m The material.
     */
    friend std::ostream &operator<<(std::ostream &os, const Material &m);

    GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();

  private:
    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive &archive, const unsigned int /*version*/)
    {
        archive &BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
        archive &diffuseColor;
        archive &specularColor;
        archive &diffuseTexture;
        archive &specularTexture;
        archive &normalTexture;
    }
};

} // namespace gintonic

BOOST_CLASS_TRACKING(gintonic::Material, boost::serialization::track_always);
