#ifndef gintonic_component_hpp
#define gintonic_component_hpp

#include "config.hpp"
#include <memory>
#include <boost/ptr_container/ptr_vector.hpp>
#include "entity.hpp" // Needed for boost::ptr_vector

namespace gintonic {

class component : public std::enable_shared_from_this<component>
{
public:
	typedef boost::ptr_vector<entity>::iterator iterator;
	typedef boost::ptr_vector<entity>::const_iterator const_iterator;
protected:
	boost::ptr_vector<entity> m_ents;
public:
	inline iterator begin() BOOST_NOEXCEPT_OR_NOTHROW { return m_ents.begin(); }
	inline iterator end() BOOST_NOEXCEPT_OR_NOTHROW { return m_ents.end(); }
	inline const_iterator begin() const BOOST_NOEXCEPT_OR_NOTHROW { return m_ents.begin(); }
	inline const_iterator end() const BOOST_NOEXCEPT_OR_NOTHROW { return m_ents.end(); }

	component() = default;
	virtual ~component() BOOST_NOEXCEPT_OR_NOTHROW;
	component(const component&) = default;
	component& operator = (const component&) = default;
	component(component&&) BOOST_NOEXCEPT_OR_NOTHROW;
	component& operator = (component&&) BOOST_NOEXCEPT_OR_NOTHROW;

	virtual void attach(entity&) = 0;
	virtual void detach(entity&) = 0;
};

} // namespace gintonic

#endif