/**
 * @file component.hpp
 * @brief Defines the base class for components.
 * @author Raoul Wols
 */

#ifndef gintonic_component_hpp
#define gintonic_component_hpp

#include <memory>
#include <vector>
#include "config.hpp"

namespace gintonic {

class entity; // Forward declaration.

/**
 * @brief Base class for a component type.
 * @details An entity carries various "plug-in" components, like
 * a mesh, a material, a light, and so forth. The base class for
 * a component is this class. It has pure virtual methods to
 * attach and detach from an entity. A component can be attached
 * to more than one entity. This allows one to reuse components
 * for multiple entities. For instance, it would be a waste of
 * memory if there are lots of entities in the scene with the same
 * mesh and every such entity had a copy of the mesh.
 */
class component
{
private:

	virtual void about_to_die() { /* nothing */ }

protected:

	/**
	 * @brief The attached entity of this component.
	 */
	std::weak_ptr<entity> m_attached_entity = std::shared_ptr<entity>(nullptr);

public:

	friend class entity;

	std::weak_ptr<entity> attached_entity() noexcept
	{
		return m_attached_entity;
	}

	std::weak_ptr<const entity> attached_entity() const noexcept
	{
		return m_attached_entity;
	}

	/**
	 * @name Constructors and destructor
	 */

	//@{

	enum e_type
	{
		kMesh,
		kMaterial,
		kLight,
		kCount
	};

	// const e_type type;

	/// constructor.
	// component(const e_type type) : type(type) {}
	
	component() = default;

	/// Destructor.
	virtual ~component() noexcept;

	/// Copy constructor.
	component(const component&) = default;

	/// Copy assignment operator.
	component& operator = (const component&) = default;

	/// Move constructor,
	component(component&&) noexcept;

	/// Move assignment operator.
	component& operator = (component&&) noexcept;

	//@}

	/**
	 * @name Entity management
	 */

	//@{

	/**
	 * @brief The beginning of the attached entities.
	 * @return An iterator to the beginning of the attached entities.
	 */
	// inline iterator begin() noexcept
	// {
	// 	return m_ents.begin();
	// }

	// /**
	//  * @brief One-past-the-end of the attached entities.
	//  * @return An iterator pointing to one-past-the-end of the attached
	//  * entities.
	//  */
	// inline iterator end() noexcept
	// {
	// 	return m_ents.end();
	// }

	// /**
	//  * @brief The beginning of the attached entities, constant version.
	//  * @return A constant iterator to the beginning of the attached entities.
	//  */
	// inline const_iterator begin() const noexcept
	// {
	// 	return m_ents.begin();
	// }

	// *
	//  * @brief One-past-the-end of the attached entities, constant version.
	//  * @return A constant iterator pointing to one-past-the-end of the
	//  * attached entities.
	 
	// inline const_iterator end() const noexcept 
	// {
	// 	return m_ents.end();
	// }

	// /**
	//  * @brief Attach this component to an entity.
	//  * @param e The entity to attach this component to.
	//  */
	// virtual void attach(entity& e) = 0;

	// /**
	//  * @brief Detach this component from an entity.
	//  * @details If the component is not attached to the given entity,
	//  * then the detachment will do nothing.
	//  * @param e The entity to detach this component from.
	//  */
	// virtual void detach(entity& e) = 0;

	//@}
};

} // namespace gintonic

#endif