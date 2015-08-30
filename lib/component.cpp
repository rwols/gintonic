#include "component.hpp"

namespace gintonic {

component::component(component&& other) BOOST_NOEXCEPT_OR_NOTHROW
: m_ents(std::move(other.m_ents))
{
	/* Empty on purpose. */
}

component& component::operator = (component&& other)
BOOST_NOEXCEPT_OR_NOTHROW
{
	m_ents = std::move(other.m_ents);
	return *this;
}

component::~component() BOOST_NOEXCEPT_OR_NOTHROW
{
	/* Empty on purpose. */
}

} // namespace gintonic