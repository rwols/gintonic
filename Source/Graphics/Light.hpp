#pragma once

#include "../ForwardDeclarations.hpp"

#include "../Foundation/Object.hpp"

#include "../Math/vec4f.hpp"

#include <string>
#include <iosfwd>

namespace gintonic {

/**
 * @brief A light. This is an abstract class, meant for inheritance.
 */
class Light : public Object<Light, std::string>
{
public:

	/**
	 * @brief Construct a light from an FbxLight.
	 * @details This static method returns a shared pointer to a new light.
	 * The light is polymorphically constructed from the data found in
	 * the FbxLight.
	 * @param pFbxLight Pointer to an FbxLight.
	 * @return Shared pointer to a new light.
	 * @throws std::runtime_error when the decay type of the FbxLight is cubic,
	 * because we only support constant, linear and quadratic attenuation values.
	 */
	static std::shared_ptr<Light> create(const FBXSDK_NAMESPACE::FbxLight* pFbxLight);

	/// Default constructor.
	Light() = default;

	/**
	 * @brief Constructor.
	 * 
	 * @param intensity The intensity value.
	 */
	Light(const vec4f& intensity);
	
	/// Destructor.
	virtual ~Light() noexcept = default;
	
	/**
	 * @brief Shine the light using the global transform of the given Entity.
	 * @details Note that the renderer must be in the light pass stage.
	 * 
	 * @param e The Entity to shine from.
	 */
	virtual void shine(const Entity& e) const noexcept = 0;

	/**
	 * @brief Set the brightness.
	 * All lights have a brightness. It's simply the W-component of the intensity.
	 * @param brightness The new brightness value.
	 */
	virtual void setBrightness(const float brightness);

	/**
	 * @brief Get the brightness.
	 * All lights have a brightness. It's simply the W-component of the intensity.
	 * @return The current brightness, or intensity.
	 */
	virtual float getBrightness() const noexcept;

	/**
	 * @brief Get the attenuation value.
	 * Some lights do not have an attenuation.
	 * @return The attenuation value.
	 */
	virtual vec4f getAttenuation() const noexcept = 0;

	/**
	 * @brief Set the attenuation value.
	 * Some lights do not have an attenuation.
	 * @param attenuation The new attenuation value.
	 */
	virtual void setAttenuation(const vec4f& attenuation) = 0;

	/**
	 * @brief Get the intensity value.
	 * All lights have an intensity.
	 * @return The intensity value.
	 */
	inline virtual vec4f getIntensity() const noexcept
	{
		return mIntensity;
	}

	/**
	 * @brief Set the intensty value.
	 * All lights have an intensity.
	 * @param intensity The new intensity value.
	 */
	inline virtual void setIntensity(const vec4f& intensity)
	{
		mIntensity = intensity;
	}

	/**
	 * @brief Get the cosine of the half angle value.
	 * @details This property is used for a SpotLight.
	 * @return The cosine of the half angle.
	 */
	virtual float getCosineHalfAngle() const noexcept = 0;

	/**
	 * @brief Get the cosine of the half angle value.
	 * @details This property is used for a SpotLight.
	 * @param cosineHalfAngle The new cosine of the half angle.
	 */
	virtual void setCosineHalfAngle(const float cosineHalfAngle) = 0;

	/**
	 * @brief Get the distance at which the light is so dim that
	 * lighting the pixel becomes pointless. This value is only meaningful
	 * for a PointLight and a SpotLight.
	 * @return The cutoff radius.
	 */
	inline virtual float getCutoffRadius() const noexcept
	{
		return 0.0f;
	}

	/**
	 * @brief Initialize the shadow buffer.
	 * @param lightEntity The light entity that is attached
	 * to this light.
	 */
	virtual void initializeShadowBuffer(Entity& lightEntity) const = 0;

	/**
	 * @brief Polymorphic stream output operator.
	 */
	friend std::ostream& operator << (std::ostream&, const Light*);

	/**
	 * @brief Polymorphic stream output operator.
	 */
	friend std::ostream& operator << (std::ostream&, const std::unique_ptr<Light>&);

	/**
	 * @brief Polymorphic stream output operator.
	 */
	friend std::ostream& operator << (std::ostream&, const std::shared_ptr<Light>&);

	/**
	 * @brief Polymorphic stream output operator.
	 */
	friend std::ostream& operator << (std::ostream&, const Light&);

	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();

protected:

	/**
	 * @brief The intensity of the light.
	 * 
	 * @details The first three components, the X, Y and Z values, define the
	 * color of the light. The fourth component, the W value, defines the
	 * intensity of the light.
	 */
	vec4f mIntensity;

private:

	/**
	 * @brief Print the light to an outstream for debug purposes.
	 */
	virtual std::ostream& prettyPrint(std::ostream&) const noexcept;

	friend boost::serialization::access;

	template <class Archive> 
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_NVP(mIntensity);
	}
};

} // namespace gintonic