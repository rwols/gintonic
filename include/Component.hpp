#pragma once

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

namespace gintonic {

class Entity;

class Component
{
public:
	Component(Entity& holder);
	
	virtual ~Component() noexcept = default;

	virtual void update() {}
	virtual void lateUpdate() {}

	inline       Entity& getEntity()       noexcept { return *mHolder; }
	inline const Entity& getEntity() const noexcept { return *mHolder; }

private:
	Component() = default;
	Entity* mHolder = nullptr;

	friend boost::serialization::access;

	template <class Archive> 
	void serialize(Archive& ar, const unsigned /*version*/)
	{
		ar & mHolder;
	}
};

} // namespace gintonic