#pragma once

#include "../ForwardDeclarations.hpp"

#include "../Foundation/Object.hpp"

#include "../Math/vec4f.hpp"

#include <string>
#include <iosfwd>

namespace gintonic {

/**
 * @brief A light. Virtual base component for inheritance.
 */
class Light : public Object<Light, std::string>
{
public:

	static std::shared_ptr<Light> create(const FBXSDK_NAMESPACE::FbxLight*);

	/**
	 * @brief The intensity of the light.
	 * 
	 * @details The first three components, the X, Y and Z values, define the
	 * color of the light. The fourth component, the W value, defines the
	 * intensity of the light.
	 */
	vec4f intensity;

	// virtual void attach(Entity&);

	// virtual void detach(Entity&);

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

	virtual void beginShadowPass(const Entity& light_ent)
	{
		/* Put me in a CPP file */
	}

	virtual void renderShadow(const Entity& geometry) const noexcept
	{
		/* Put me in a CPP file */
	}

	/**
	 * @brief Set the brightness, or intensity of the light.
	 * 
	 * @param brightness The new brightness value.
	 */
	virtual void setBrightness(const float brightness);

	/**
	 * @brief Get the brightness, or intensity of the light.
	 * @return The current brightness, or intensity.
	 */
	float brightness() const noexcept;

	virtual void initializeShadowBuffer(std::shared_ptr<Entity> lightEntity) const = 0;

	/**
	 * @brief Polymorphic stream output operator.
	 */
	friend std::ostream& operator << (std::ostream&, const Light*);

	/**
	 * @brief Polymorphic stream output operator.
	 */
	friend std::ostream& operator << (
		std::ostream&, 
		const std::unique_ptr<Light>&);

	/**
	 * @brief Polymorphic stream output operator.
	 */
	friend std::ostream& operator << (
		std::ostream&, 
		const std::shared_ptr<Light>&);

	/**
	 * @brief Polymorphic stream output operator.
	 */
	friend std::ostream& operator << (
		std::ostream&, 
		const Light&);

	//!@cond
	GINTONIC_DEFINE_SSE_OPERATOR_NEW_DELETE();
	//!@endcond

private:

	/**
	 * @brief Print the light to an outstream for debug purposes.
	 */
	virtual std::ostream& prettyPrint(std::ostream&) const 
		noexcept;

	//!@cond
	// We need to give boost::serialization access to this class.
	friend boost::serialization::access;
	//!@endcond

	template <class Archive> 
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & BOOST_SERIALIZATION_NVP(intensity);
	}
};

} // namespace gintonic